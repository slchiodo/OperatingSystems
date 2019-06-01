#ifndef _COMMAND_H_
#define _COMMAND_H_ 

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int check_commands(char *tokens[]);
bool is_background(char *tokens[]);
bool is_redirection(char *tokens[]);
bool is_bang(char *tokens[]);


#endif