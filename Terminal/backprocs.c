/* linked list storing processes running in the background */

#include "backprocs.h"

void init_procs(struct procs *p) {
    p->head = NULL;
    p->tail = NULL;
}

void init_procs_node(struct proc_node *p, pid_t id) {
    p->id = id;
    p->next = NULL;
    p->prev = NULL;
}

bool isEmptyP(struct procs *p) {
    return p->head == NULL;
}

void push_front_procs(struct procs *p, pid_t id) {
	struct proc_node *pn = (struct proc_node*) malloc (sizeof (struct proc_node));
    init_procs_node(pn, id);
    if (isEmptyP(p)) {
        p->head = pn;
        p->tail = pn;
        pn->next = p->head;
    } else {
        pn->next = p->head;
        p->head->prev = pn;
        p->head = pn;
    }
}

void print_procs(struct procs *p) {
    int status;
    if (isEmptyP(p)) {
        printf("No background processes running.\n");
        return;
    }
    struct proc_node *curr = p->tail;
    struct proc_node *end = p->head;
    while (curr != end) {
    	if (waitpid(curr->id, &status, WNOHANG) == 0) {
        	printf("     %d \n", curr->id);
    	}
        curr = curr->prev;
    }
    if (waitpid(curr->id, &status, WNOHANG) == 0) {
        printf("     %d \n", curr->id);
    }
}


/* 
Purpose: if exit is entered all processes are killed
Input: linked list of background processes
Output: none
*/
void terminate_processes(struct procs *p) {
    int status;
    if (isEmptyP(p)) {
        return;
    }
    struct proc_node *curr = p->tail;
    struct proc_node *end = p->head;
    while (curr != end) {
        if (waitpid(curr->id, &status, WNOHANG) == 0) {
            kill(curr->id, SIGKILL);
        }
        curr = curr->prev;
    }
    if (waitpid(curr->id, &status, WNOHANG) == 0) {
        kill(curr->id, SIGKILL);
    }  
}

/* 
Purpose: free all memory in background process nodes
Input: linked list of background nodes
Output: none
*/
void free_backgroundprocs (struct procs *p) {
    if (isEmptyP(p)) {
        return;
    }
    struct proc_node *curr = p->head;
    struct proc_node *next = p->head;
    struct proc_node *start = p->tail;
    for (curr = start; NULL != curr; curr = next) {
        next = curr->prev;
        free(curr);
    }
}
