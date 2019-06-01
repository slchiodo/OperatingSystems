/* executes user inputted commands */

#include "backprocs.h"
#include "execution.h"
#include "command.h"
#include "timer.h"

struct procs p;

/* 
Purpose: change current directory
Input: user input array of directory to change to
Output: none
*/
void cd_cmd (char* tokens[]) {
    if (tokens[1] == NULL) {
        chdir(getenv("HOME"));
        printf("Directory changed to home.\n");
    } else {
        chdir(tokens[1]);
        printf("Directory changed to input path.\n");
    }
}

/* 
Purpose: seperate an array by specific value into two
Input: user input array, two new arrays and deliminating character
Output: two new arrays
*/
void seperate_arrays (char* tokens[], char* tokensL[], char* tokensR[], char* delim) {
    int i = 0;
    int j = 0;
    while (strcmp(tokens[i], delim) != 0) { 
        tokensL[i] = tokens[i];
        i++; 
    }
    tokensL[i] = NULL;
    i++; 
    while (tokens[i] != NULL) { 
        tokensR[j] = tokens[i];
        i++; 
        j++;
    }
    tokensR[j] = NULL;
}

/* 
Purpose: redirect to file
Input: user input array and if background or not
Output: a file with exeuted command to console
*/
void redirection_cmd (char* tokens[], int background) {
    int fd[2];
    int fdO;
    char *output_args[512];
    char* file[10];
    seperate_arrays(tokens, output_args, file, ">");
    if ((fdO = open(file[0], O_CREAT | O_WRONLY, 0644)) < 0) {
        printf("cannot open out\n");
        exit(1);
    }
    if (pipe(fd) == -1) { perror("pipe"); }
    pid_t pid = fork();
    if (background == 1) { push_front_procs(&p, pid); }
    if (pid == 0) {
        close(fd[1]);
        dup2(fdO, STDOUT_FILENO);
        execvp(output_args[0], output_args);
        printf("%s command does not exist. \n", tokens[0]);
        exit(-1);
    } else {
        close(fd[0]);
        close(fdO);
        if (background == 0) {
            int status;
            wait(&status);
        }
    }
}

/* 
Purpose: pipe information between processes, supporting redirection
Input: user input array and if background or not
Output: none, exectutes user input to console
*/
void pipe_cmd (char* tokens[], int background) {
    int fd[2];
    char* tokensL[10];
    char* tokensR[10];
    seperate_arrays(tokens, tokensL, tokensR, "|");
    if (pipe(fd) == -1) { perror("pipe"); }
    pid_t pid = fork();
    if (background == 1) { push_front_procs(&p, pid); }
    if (pid == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(tokensL[0], tokensL);
        printf("%s command does not exit. \n", tokensL[0]);
        exit(1);
    } else { 
        pid = fork();
        if (background == 1) { push_front_procs(&p, pid); }
        if (pid == 0) {
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            if (is_redirection(tokensR)) {
                redirection_cmd(tokensR, background);
            } else {
                execvp(tokensR[0], tokensR);
                printf("%s command does not exit. \n", tokensR[0]);
            }
            exit(1);
        } else {
            close(fd[0]);
            close(fd[1]);
            if (background == 0) {
                int status;
                wait(&status);
                wait(&status);
            } 
        }
    }
}

/* 
Purpose: init linked list of background processes
Input: none
Output: empty linked list of procs
*/
void procs() {
    init_procs(&p);
}

/* 
Purpose: kill all processes 
Input: none
Output: none
*/
void term() {
    terminate_processes(&p);
}

/* 
Purpose: print all the current processes running in the background
Input: user input array
Output: none, prints to console
*/
void job_cmd (char* tokens) {
    print_procs(&p);
}

void free_procs() {
    free_backgroundprocs(&p);
}

/* 
Purpose: run basic shell cmds
Input: user input array and if background or not
Output: none, prints to console
*/
void basic_cmd (char* tokens[], int background) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(tokens[0], tokens);
        printf("%s command does not exit. \n", tokens[0]);
        exit(-1);
    } else {
        if (background == 0) {
            int status;
            wait(&status);
        } else {
            push_front_procs(&p, pid);
        }
    }
}
