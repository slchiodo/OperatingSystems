#include "allocator.c"

/**
 * Add your allocator test cases here. You can run 'make test' to compile
 * everything and run this mini program.
 */
int main(void) {
    int *my_num = malloc(sizeof(int));
    printf("My num: %d\n", *my_num);

    char *str = malloc(25);
    strcpy(str, "Hooray for malloc!");
    printf("str -> %s\n", str);

    char *big = malloc(4900);
    strcpy(big, "Whoa, big allocation!");
    printf("big -> %s\n", big);

    malloc(1);

    return 0;
}
