#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>

/* Preprocessor directives */
#define MAX_PATH 500
#define MAX_PATTERN 100
#define MAX_LINE 1000
#define MAX_FILE 50

/* STRUCTS & GLOBAL VARIABLES */
sem_t thread_semaphore;

// struct defining command line information
struct cmdline_info {
    int tvalue;
    int evalue_num;
    char dvalue[MAX_PATH];
    int dinput;
    char **evalue;
    int case_sensitive; // 0 = not, 1 = yes
};

// thread arguments for grep function
struct thread_args {
    struct cmdline_info ci;
    char path[MAX_PATH];
    int tnum;
} thread_args;
struct thread_args ta[4];


// grep arguments
struct grep_info {
    char path[MAX_PATH];
    char *lines[MAX_LINE];
    int line_numbers[MAX_FILE];
    int grep_lines;
};

/* INITIALIZE STRUCTS TO SET DEFAULT VALUES */
/* Initializes the command line struct
 * sets the default directory to current and default threads to machines maximum
 * Input: pointer to cmd line struct
 * Returns: N/A */
void init_cmdline_info(struct cmdline_info *ci) {
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
    }
    strcpy(ci->dvalue, cwd);
    ci->tvalue = get_nprocs();
    ci->evalue_num = 0;
    ci->dinput = 0;
    ci->case_sensitive = 0;
}

/* Initializes the thread arguments struct
 * sets the path to empty and the cmd line struct
 * Input: pointer to thread args struct and the cmdline struct
 * Returns: N/A */
void init_thread_args(struct thread_args *ta, struct cmdline_info c) {
    ta->ci = c;
    ta->path[0] = '\0';
}

/* Initializes the grep struct
 * sets all values to empty and set the path to input
 * Input: pointer to grep struct and path
 * Returns: N/A */
void init_grep_info(struct grep_info *gi, char *path) {
    strcpy(gi->path, path);
    memset(gi->line_numbers, 0, sizeof(gi->line_numbers));
    memset(gi->lines, '\0', sizeof(gi->lines));
    gi->grep_lines = -1;
}


/* GREPPING FUNCTIONS */
/* determines the amount of punctuation in a string
 * Input: a string
 * Returns: number of  as an integer */
int check_punc(char *str) {
    int iter1 = 0;
    int punc = 0;
    for (iter1 = 0; iter1 < strlen(str); iter1++) {
        if (ispunct(str[iter1])) {  punc++;  }
    }
    return punc;
}

/* determines the length of str1, returning the length as an integer
 * Input: a string
 * Returns: length as an integer */
int stringlen(char* str1) {
    int strlen = 0;
    while (str1[strlen] != '\0' && str1[strlen] != '\n') {
        strlen++;
    }
    return strlen;
}

/* iterating each line and word to find, compares line to patterns, if matches saves to grep struct and prints all matched together
 * Input: thread argment struct, comtaining the path and cmdline information
 * Returns: N/A */
pthread_mutex_t lock;
void *grep_file(void *ta) {
    pthread_detach(pthread_self());
    struct cmdline_info ci = ((struct thread_args *)ta)->ci;
    char *path = ((struct thread_args *)ta)->path;
    struct grep_info gi;
    init_grep_info(&gi, path);
    char *saveptr1;
    FILE* fp;
    char tline[MAX_LINE];
    char *line = (char*) malloc(MAX_LINE);
    char *word = (char*) malloc(MAX_LINE);
    int linenum = 0, containsp = 0;
    int p, i, j;
    char *pattern;
    fp = fopen(path, "r");
    if (fp != NULL) {
        while (fgets(line, MAX_LINE, fp)) {
            linenum++;
            strcpy(tline, line);
            word = strtok_r(line, " ", &saveptr1);
            while (word != NULL) {
                p = 0;
                
                for (i = 0; i < stringlen(word); i++) {
                    if (!ispunct(word[i])) {
                        word[p] = word[i];
                        p++;
                    } else {
                        if (i != 0 || i == stringlen(word)-1) {
                            word[p] = ' ';
                            p++;
                        }
                    }
                }
                word[p] = '\0';
                for (i = 0; i < ci.evalue_num; i++) {
                    if (containsp == 0) {
                        pattern = ci.evalue[i];
                        if (ci.case_sensitive == 0) {
                            for(j = 0; word[j]; j++){
                                word[j] = tolower(word[j]);
                            }
                            for(j = 0; pattern[j]; j++){
                                pattern[j] = tolower(pattern[j]);
                            }
                        }
                        if (strcmp(word, pattern) == 0) {
                            containsp = 1;
                            gi.grep_lines++;
                            gi.lines[gi.grep_lines] = (char*) malloc(MAX_LINE);
                            strcpy(gi.lines[gi.grep_lines], tline);
                            gi.line_numbers[gi.grep_lines] = linenum;
                        }
                        
                    }
                }
                word = strtok_r(NULL, " ", &saveptr1);
            }
            tline[0] = '\0';
            containsp = 0;
        }
        pthread_mutex_lock(&lock);
        int iter1;
        if (gi.grep_lines > -1) {
            for (iter1 = 0; iter1 <= gi.grep_lines; iter1++) {
                printf("%s:%d:%s", gi.path, gi.line_numbers[iter1], gi.lines[iter1]);
                free(gi.lines[iter1]);
            }
        }
        pthread_mutex_unlock(&lock);
        fclose(fp);
    }
    free(word);
    free(line);
    init_thread_args(ta, ci);
    sem_post(&thread_semaphore);
    sem_destroy(&thread_semaphore);
    return 0;
}

/* DIRECTORY FUNCTIONS */
/* removes a directory from the end of a path
 * Input: a path and cmdline struct
 * Returns: path with one less directory on the end */
void remove_last(char* *path,  struct cmdline_info ci) {
    int iter1 = 0, iter2;
    char *token;
    char *array[MAX_PATTERN];
    char* outpath = (char*) malloc(MAX_PATH);
    char *tpath = (char*) malloc(MAX_PATH);
    char *saveptr1;
    strcpy(tpath, *path);
    token = strtok_r(tpath, "/", &saveptr1);
    while(token != NULL) {
        array[iter1] = token;
        iter1++;
        token = strtok_r(NULL, "/", &saveptr1);
    }
    for (iter2 = 0; iter2 < iter1-1; iter2++) {
        if (iter2 < 1) {
            if (ci.dinput == 1) {
                strcpy(outpath, "/");
                strcat(outpath, array[iter2]);
            } else {
                strcpy(outpath, array[iter2]);
            }
        } else {
            strcat(outpath, "/");
            strcat(outpath, array[iter2]);
        }
    }
    free(tpath);
    *path[0] = '\0';
    strcpy(*path, outpath);
    free(outpath);
    return;
}

/* recursively transverse the directory, altering the path if a directory or grepping if a file
 * Input: path and cmd line struct
 * Returns: path */
char* transverse_directory (char *path, struct cmdline_info ci) {
    DIR* directpath;
    struct dirent* direct;
    directpath = opendir(path);
    char *catpath;
    char *rempath;
    int tnum = 0;
    if (directpath == NULL) {
        perror("Error: can't open path");
    } else {
        while ((direct = readdir(directpath))) {
            if (direct == NULL)
                break;
            if ((strcmp(direct->d_name, ".") != 0) && (strcmp(direct->d_name, "..")) != 0){
                if (DT_DIR == (direct->d_type) || DT_REG == (direct->d_type)){
                    if (DT_DIR == (direct->d_type)) {
                        rempath = (char*) malloc(MAX_PATH);
                        strcpy(rempath, path);
                        strcat(rempath, "/");
                        strcat(rempath, direct->d_name);
                        transverse_directory(rempath, ci);
                        remove_last(&rempath, ci);
                        path[0] = '\0';
                        strcpy(path, rempath);
                        free(rempath);
                    } else {
                        catpath = (char*) malloc(MAX_PATH);
                        strcpy(catpath, path);
                        strcat(catpath, "/");
                        strcat(catpath, direct->d_name);
                        tnum = 0;
                        pthread_t thread;
                        sem_wait(&thread_semaphore);
                        while (ta[tnum].path[0] != '\0') {
                            if (tnum == ci.tvalue) {
                                tnum = 0;
                            } else {
                               tnum++;
                            }
                        }
                        strcpy(ta[tnum].path, catpath);
                        ta[tnum].ci = ci;
                        int ret = pthread_create(&thread, NULL, grep_file, (void *) &ta[tnum]);
                        if (ret != 0) {
                            perror("pthread_create");
                        }
                        free(catpath);
                    }
                }
            }
        }
        closedir(directpath);
    }
    return path;
}

/* COMMAND LINE FUNCTIONS */
/* Prints usage information for the program, generally called when the user enters -h
 * Input: N/A
 * Returns: N/A, exits program */
void print_usage() {
    printf("\nUsage: ./prep [-eh] [-d directory] [-t threads] search_term1 search_term2 ... search_termN \n");
    printf("Options: \n");
    printf("     * -d directory    specify start directory (default: CWD)\n");
    printf("     * -e              print exact case matches only\n");
    printf("     * -h              show usage information\n");
    printf("     * -t threads      set maximum threads (default: num CPUs)\n");
    printf("Exiting.\n\n");
    exit(0);
}

/* Parses the command line for user input values and saves to a struct
 * Input: command input length, command inputs, pointer to command line information struct
 * Returns: N/A, values saves via pointers */
int parse_cmdline(int argc, char **argv, struct cmdline_info *ci) {
    int c, tval;
    opterr = 0;
    while ((c = getopt(argc, argv, "d:eht:")) != -1) {
        switch (c) {
            case 'd':
                strcpy(ci->dvalue, optarg);

                break;
            case 'e':
                ci->case_sensitive = 1;
                break;
            case 'h':
                print_usage();
                return 0;
            case 't':
                tval = atoi(optarg);
                if (tval > ci->tvalue) { exit(0); }
                if (tval < 0) {
                    printf("Number of threads entered is inoperative. \n");
                    exit(0);
                }
                ci->tvalue = tval;
                break;
            case '?':
                if (optopt == 'd' || optopt == 't') {
                    fprintf(stderr,
                            "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }
    if (ci->dvalue[0] == '/') {
        ci->dinput = 1;
    }
    ci->evalue_num = argc - optind;
    ci->evalue = &argv[optind];
    return 0;
}



int main(int argc, char *argv[]) {
// cmd line values
    struct cmdline_info ci;
    init_cmdline_info(&ci);
    parse_cmdline(argc, argv, &ci);
    
// initialize array of thread args, length of max threads
    int iter1 = 0;
    while (iter1 < ci.tvalue) {
        init_thread_args(&ta[iter1], ci);
        iter1++;
    }
    sem_init(&thread_semaphore, 0, ci.tvalue);
    
// transverse directory
    transverse_directory(ci.dvalue, ci);
    
// check all threads are finished and exit
    pthread_exit(NULL);
    return 1;
}





