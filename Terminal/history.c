/* linked list storing history */

#include "history.h"
#include "timer.h"

void init_history(struct history *hlist) {
    hlist->count = 0;
    hlist->head = NULL;
    hlist->tail = NULL;
}

void cpy_string(char *hn, char *tokens) {
    int i;
    for (i = 0; tokens[i] != '\0'; i++) { hn[i] = tokens[i]; }   
    hn[i] = '\0';
}

void cpy_tokens(struct history_node *hn, char *tokens[], struct history *hlist) {
    int i = 0;
    while (tokens[i] != NULL) {
        hn->tokens[i] =  malloc(strlen(tokens[i]) + 2);
        cpy_string(hn->tokens[i], tokens[i]);
        i++;
    }  
    hn->tokens[i] = NULL; 
}

void print_tokens(struct history_node *hn) {
    int i;
    for (i = 0; hn->tokens[i] != NULL; i++) { printf("%s ", hn->tokens[i]); }   
}

void init_history_node(struct history_node *hn, char *tokens[], struct history *hlist, double rn) {
    cpy_tokens(hn, tokens, hlist);
    hn->count = hlist->count;
    hn->next = NULL;
    hn->prev = NULL;
    hn->exec_time = (char *) malloc(10);
    get_time(&hn->exec_time);
    hn->run_time = rn;
}

bool isEmpty(struct history *hlist) {
    return hlist->head == NULL;
}

void pop_back_history (struct history *hlist) {
    struct history_node *hn = hlist->tail;
    hlist->tail = hn->prev;
    hn->prev->next = NULL;
    hn->prev = NULL;
}

void push_front_history(struct history *hlist, char* tokens[], double rn) {
    struct history_node *hn = (struct history_node*) malloc (sizeof (struct history_node));
    init_history_node(hn, tokens, hlist, rn);
    if (isEmpty(hlist)) {
        hlist->head = hn;
        hlist->tail = hn;
        hn->next = hlist->head;
    } else {
        hn->next = hlist->head;
        hlist->head->prev = hn;
        hlist->head = hn;
    }
    hlist->count++;
    if (hlist->count >= HIST_MAX) { pop_back_history(hlist); }
}

void print_history(struct history *hlist) {
    if (isEmpty(hlist)) {
        printf("History is empty.\n");
        return;
    }
    int count = 0;
    struct history_node *curr = hlist->tail;
    struct history_node *end = hlist->head;
    while (curr != end) {
        printf("     [%d|%s|%.2f] ", curr->count, curr->exec_time, (curr->run_time * 100));
        print_tokens(curr);
        printf("\n");
        curr = curr->prev;
        count++;
    }
    printf("     [%d|%s|%.2f] ", curr->count, curr->exec_time, (curr->run_time * 100));
    print_tokens(curr);
    printf("\n");
}

/* 
Purpose: check if a string is an integer
Input: string
Output: true if in and false if string
*/
bool checkint(char* val) {
    int i = 0;
    while (val[i] != '\0') {
        if (isalpha(val[i])) { return false; }
        i++;
    }
    return true;
}

/* 
Purpose: get most recent command in history, either by number or string
Input: user input command searching for 
Output: corresponding history node 
*/
struct history_node* get_history(struct history *hlist, char* val) {
    int count = 0;
    struct history_node *hn = hlist->head;
    struct history_node *end = hlist->tail;
    if (!isEmpty(hlist)) {
        if (!checkint(val)) {
            while (hn != end) {
                if (strcmp(hn->tokens[0], val) == 0) { return hn; }
                hn = hn->next;
            }
            if (strcmp(hn->tokens[0], val) == 0) { return hn; }
        } else {
            while (hn != end) {
                if (hn->count == atoi(val)) { return hn; }
                hn = hn->next;
                count++;
            }
        }
        printf("%s command is not in history.\n", val);
        return NULL;
    } else {
        printf("History is empty\n");
        return NULL;
    }

}

/* 
Purpose: free all memory in history
Input: linked list of history nodes
Output: none
*/
void free_history (struct history *hlist) {
    if (isEmpty(hlist)) {
        return;
    }
    struct history_node *curr = hlist->head;
    struct history_node *next = hlist->head;
    struct history_node *start = hlist->tail;
    for (curr = start; NULL != curr; curr = next) {
        next = curr->prev;
        free(curr);
    }
}

//        LOG("   REUSE %s %d\n", g_curr->description, g_curr->start);
//        if (g_curr->start == true) {
//            used_page = 0;
//            used_page += g_curr->used;
//            if (g_curr != NULL) {
//                g_curr = g_curr->next;
//            }
//        }
//        while (g_curr != NULL && g_curr->start == false) {
//            LOG("   Increase Usage %s %d\n", g_curr->description, g_curr->start);
//            iterate_memory();
//            used_page += g_curr->used;
//            g_curr = g_curr->next;
//        }
//
////        if (g_curr != NULL) {
////            LOG("   Increase Usage %s %d\n", g_curr->description, g_curr->start);
////        }
//
//        LOG("USAGE %zu <= %zu    %zu\n", size, (page_sz-used_page), used_page);
//        if (used_page > 0 && size <= page_sz-used_page) {
//            LOG("   REUSE %s\n", (g_curr->prev)->description);
//
//            return g_curr-1;
//        }

//FILE *f = fopen("x.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
//
//
//void make_file () {
//    f = fopen("x.log", "a+");
//}

//void log_memory (char *type, struct mem_block ) {
//    if (f == NULL) { return; }
//    fprintf(f, "Im logging somethig ..\n");
//}

