/* print and manipulate prompt */

#include "prompt.h"
#include "timer.h"

#define MAX_HOST 50
#define MAX_TIME 25

void last_path(char temp[]) {
    char home[500];
    getcwd(home, sizeof(home));
    char *token = strtok(home, "/");
    while (token != NULL) {
        strcpy(temp, token);
        token = strtok(NULL, "/");
    }    
}

/* 
Purpose: check if the current path is a sub directory of the starting position or not
Input: current path and starting directory
Output: path, either full is not in staart directory or shortened if subdirectory
*/
void get_path (char* curr_path,  char* start_dir) {
    char home[500];
    getcwd(home, sizeof(home));
    char *token = strtok(home, "/");
    int curr_dir = 1;
    while (token != NULL) {
    	if (curr_dir == 0) {
    		strcat(curr_path, "/");
        	strcat(curr_path, token);
    	} else if (strcmp(token, start_dir) == 0) {
    		curr_dir = 0;
    	}
        token = strtok(NULL, "/");
    }
    if (curr_dir == 1) {
        curr_path[0] = '\0';
        getcwd(home, sizeof(home));
        strcpy(curr_path, home); 
    }
}

/* 
Purpose: print the prompt to console 
Input: user input array, starting directory and the current path
Output: none, prints to console
*/
void print_prompt(int cmd_counter,  char* start_dir, char* curr_path) {
    curr_path[0] = '\0';
    strcpy(curr_path, "~"); 
    get_path(curr_path, start_dir);
    char *curr_user = getenv("USER");
    if (curr_user == NULL) {
        printf("Can not get user information.\n"); 
        exit(-1);
    }
    char curr_host[MAX_HOST];
    gethostname(curr_host, MAX_HOST);
    char *s =  malloc(MAX_TIME);
    get_time(&s);
    printf("[%d|%s|%s@%s:%s]--$ ", cmd_counter, s, curr_user, curr_host, curr_path);
    fflush(stdout);
    free(s);
}

