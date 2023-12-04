#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Allocate memory and set zero all its bytes, while also checking if the returned point is not NULL
static void* aoc_calloc(size_t count, size_t size)
{
    void *const my_ptr = calloc(count, size);
    if (!my_ptr)
    {
        fprintf(stderr, "Not enough memory.\n");
        exit(EXIT_FAILURE);
    }
    return my_ptr;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Please run the program with the input file's path as the only argument.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "rt");
    if (!file)
    {
        fprintf(stderr, "Could not open '%s'.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Code...

    return 0;
}