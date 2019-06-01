#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define HIST_MAX 101

struct history_node {
    char *tokens[10];
    int count;
    char *exec_time;
    double run_time;
    struct history_node *next;
    struct history_node *prev;
};

struct history {
    int count;
    struct history_node *head;
    struct history_node *tail;
};


void init_history(struct history *);
struct history_node* get_history(struct history *, char *);
void push_front_history(struct history *, char* tokens[], double);
void print_history(struct history *hlist);
void free_history(struct history *hlist);

#endif

