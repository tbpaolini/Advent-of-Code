#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

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

    bool is_first_digit = true;
    int first_digit = 0;
    int last_digit = 0;
    uint64_t solution = 0;
    int my_char = 0;
    
    while ( (my_char = fgetc(file)) != EOF )
    {
        // Update the solution's digits when a numeric character is found
        if (isdigit(my_char))
        {
            int my_digit = my_char - '0';
            if (is_first_digit)
            {
                first_digit = my_digit;
                is_first_digit = false;
            }
            last_digit = my_digit;
        }

        // On a new line, update the solution and switch to the next value
        if (my_char == '\n')
        {
            solution += (10 * first_digit) + last_digit;
            is_first_digit = true;
            first_digit = 0;
            last_digit = 0;
        }
    }

    fclose(file);

    printf("Part 1: %lu\n", solution);
}