/*  entry point for program to execute */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "command.h"
#include "execution.h"
#include "history.h"
#include "prompt.h"
#include "timer.h"
#include "backprocs.h"

#define MAX_PATH 500

/* 
Purpose: seperate the user input to an array
Input: empty array
Output: populates empty array
*/
void parse_cmdline(char *tokens[]) {
    char *line = NULL;
    int i = 0;
    size_t line_sz = 0;
    getline(&line, &line_sz, stdin);
    char *token = strtok(line, " \t\n\r");
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    tokens[i] = (char *) NULL;
}


void remove_front(char *tokens) {
    int iter1 = 0;
    int iter2 = 1;
    while (tokens[iter2] != '\0') {
        tokens[iter1] = tokens[iter2];
        iter1++;
        iter2++;
    }
    tokens[iter1] = '\0';
}

/* 
Purpose: copt tokens from previos history command to array 
Input: user input array and history node
Output: array of histoy command values
*/
void copy_tokens(struct history_node *hn, char *tokens[]) {
    int iter1 = 0;
    if (hn != NULL) {
        while (hn->tokens[iter1] != NULL) {
            tokens[iter1] = hn->tokens[iter1];
            iter1++;
        }
        tokens[iter1] = NULL; 
    } else {
        tokens[0] = NULL;
    }
}

/* 
Purpose: ignores signal CNTRL calls
Input: signal int
Output: none
*/
void ignore(int sig) {
    return;
}


void removelast(char *tokens[]) {
    int i;    
    for (i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "&") == 0) {
            tokens[i] = NULL;
        }
    }
}

int main(void) {
    int cmd_counter = 0;
    struct history hlist;
    // init history list
    init_history(&hlist);
    procs();
    // get statring directory
    char start_dir[MAX_PATH];
    last_path(start_dir);
    char curr_path[MAX_PATH];
    int max_args = (int) sysconf(_SC_ARG_MAX);
    char **tokens =  malloc(max_args);
    // signal CNTRL inputs
    signal(SIGINT, ignore);
    while (true) {     
        double start = 0.0, end = 0.0, ttime = 0.0;
        char *start_time = (char *) malloc(10);
        char *end_time = (char *) malloc(10);
        int bckgrnd = 0;
        // print prompt
        print_prompt(cmd_counter, start_dir, curr_path);
        cmd_counter++;
        // tokenize cmd line
        parse_cmdline(tokens);
        if (tokens[0] != NULL) {
            // check if bang
            if (is_bang(tokens)) {
                remove_front(tokens[0]);
                copy_tokens(get_history(&hlist, tokens[0]), tokens);
            } 
            // check if comment of empty line
            if (tokens[0] != NULL && tokens[0][0] != '#' && tokens[0][0] != '\n') { 
                // check if background process
                if (is_background(tokens)) {
                    removelast(tokens);
                    bckgrnd = 1;
                }
                start = run_time(start_time);
                // check cmd type
                switch(check_commands(tokens)) {                    
                    case 1 :
                        cd_cmd(tokens);
                        break;
                    case 2 :
                        redirection_cmd(tokens, bckgrnd);
                        break;
                    case 3 :
                        pipe_cmd(tokens, bckgrnd);
                        break;
                    case 4 :
                        print_history(&hlist);
                        break;
                    case 5 :
                        job_cmd();
                        break;
                    case 6 :
                        term();
                        free_history(&hlist);
                        free_procs();
                        printf("Exiting.\n");
                        exit(0);
                    default :
                        basic_cmd(tokens, bckgrnd);
                        break;  
                } 
                bckgrnd = 0;
                end = run_time(end_time);
                ttime = end - start;
                // add to history
                push_front_history(&hlist, tokens, ttime); 
                free(start_time);
                free(end_time);
            }
        } 
    }
    free(tokens);
    return 0;
}

