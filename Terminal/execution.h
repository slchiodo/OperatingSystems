#ifndef _EXECUTION_H_
#define _EXECUTION_H_

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void cd_cmd(char* tokens[]);
void redirection_cmd(char* tokens[], int );
void pipe_cmd(char* tokens[], int );
void job_cmd();
void term();
void basic_cmd(char* tokens[], int );
void procs();
void free_procs();


#endif