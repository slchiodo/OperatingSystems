/**
 * allocator.c
 *
 * Explores memory management at the C runtime level.
 *
 * Author: <your team members go here>
 *
 * Compile: gcc -o allocator.so -Wall -fPIC -shared allocator.c
 *
 * To use (one specific command):
 * LD_PRELOAD=$(pwd)/allocator.so command
 * ('command' will run with your allocator)
 *
 * To use (all following commands):
 * export LD_PRELOAD=$(pwd)/allocator.so
 * (Everything after this point will use your custom allocator -- be careful!)
 */

 /*

current porblems 
- mallocing big blocks of space, when to deallocate
    - remove using munmap and from linked list

- allocation search for a free block
    - loop through linked list and find space in pages, block->spaceused
    - first 
    - best
    - worst

- logging to a file
    - debug to 0

- scribbling 

 */

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEBUG 1
#define LOG(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

struct mem_block {
    /* Size of the memory region */
    size_t size;

    /* Space used */
    size_t used;

    /* Whether or not this region has been freed */
    bool free;

    /* Name/description for our block of memory. */
    char description[64];

    /* Next block in the chain */
    struct mem_block *next;
};

struct mem_block *g_head = NULL;
unsigned long g_allocations = 0;


/*  */
void first_fit () {

}

/*  */
void best_fit () {

}

/*  */
void worst_fit () {

}


/*  */
void *malloc_description (size_t size, char *desc) {

}

/*  */
void *malloc_lookup (char *desc) {

}

/*  */
int algo_type () {

}

/*  */
void iterate_memory () {

}

/*  */
void find_memory (struct mem_block *block) {

}

/*  */
void add_memory (struct mem_block *block) {

}

/*  */
void remove_memory (struct mem_block *block) {

}

/*  */
void *malloc (size_t size) {

}

/*  */
void free (void *ptr) {

}

/*  */
void *calloc (size_t nmemb, size_t size) {

}

/*  */
void *realloc (void *ptr, size_t size) {

}



