#ifndef _BACKPROCS_H_
#define _BACKPROCS_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

struct proc_node {
    pid_t id;
    struct proc_node *next;
    struct proc_node *prev;
};

struct procs {
    struct proc_node *head;
    struct proc_node *tail;
};

void init_procs(struct procs *);
void push_front_procs(struct procs *, pid_t);
void print_procs(struct procs *);
void terminate_processes(struct procs *);
void free_backgroundprocs(struct procs *);

#endif
