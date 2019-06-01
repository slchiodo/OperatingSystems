/* checks input string to see what type of function is being executed */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

#define MAX_CMD_LEN 512


bool is_cd(char *tokens[]) {
    if (strcmp(tokens[0], "cd") == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_redirection(char *tokens[]) {
    int i = 0;
    while (tokens[i] != NULL) {
        if (strcmp(tokens[i], ">") == 0) {
            return true;
        }
        i++;
    }
    return false;
}

bool is_pipe(char *tokens[]) {
    int i;    
    for (i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            return true;
        }
    }
    return false;
}

bool is_history(char *tokens[]) {
    if (strcmp(tokens[0], "history") == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_job(char *tokens[]) {
    if (strcmp(tokens[0], "job") == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_exit(char *tokens[]) {
    if (strcmp(tokens[0], "exit") == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_background(char *tokens[]) {
    int i;    
    for (i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "&") == 0) {
            return true;
        }
    }
    return false;
}

bool is_bang(char *tokens[]) {
    if (tokens[0][0] == '!') {
        return true;
    }
    return false;
}

/* 
Purpose: check all possible commands to perform
Input: user input array 
Output: integer representing command type
*/
int check_commands(char *tokens[]) {
    int rv = 0;    
    if(is_cd(tokens)) { rv = 1; }
    if(is_redirection(tokens)) { rv = 2; }
    if(is_pipe(tokens)) { rv = 3; }
    if(is_history(tokens)) { rv = 4; }
    if(is_job(tokens)) { rv = 5; }
    if(is_exit(tokens)) { rv = 6; }
    return rv;
}

