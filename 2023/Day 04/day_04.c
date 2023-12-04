#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BUF_SIZE 256
#define MAX_NUMBER 100

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

    uint64_t solution_p1 = 0;
    char line[BUF_SIZE] = {0};      // A line from the input
    bool numbers[MAX_NUMBER] = {0}; // Keep track of the scored numbers

    // Parse the input line by line
    while (fgets(line, sizeof(line), file))
    {
        // Split the input line
        char *game = strtok(line, ":");     // Card count
        char *card = strtok(NULL, "|");     // Card's numbers
        char *win  = strtok(NULL, "\n");    // Winning numbers

        // Parse the numbers on the card
        char *card_start = card;
        char *card_end = card;
        while (*card_end != '\0')
        {
            size_t n = strtoll(card_start, &card_end, 10);
            if (n >= MAX_NUMBER)
            {
                fprintf(stderr, "Malformatted input.\n");
                exit(EXIT_FAILURE);
            }
            
            if (card_end != card_start)
            {
                numbers[n] = true;
                card_start = card_end;
            }
            else
            {
                break;
            }
        }
        
        // Check how many winning numbers are on the card
        uint64_t win_count = 0;
        char *win_start = win;
        char *win_end = win;
        while (*win_end != '\0')
        {
            size_t n = strtoll(win_start, &win_end, 10);
            if (n >= MAX_NUMBER)
            {
                fprintf(stderr, "Malformatted input.\n");
                exit(EXIT_FAILURE);
            }

            if (win_end != win_start)
            {
                if (numbers[n]) win_count++;
                win_start = win_end;
            }
            else
            {
                break;
            }
        }

        // Calculate the amount of points
        if (win_count > 0)
        {
            solution_p1 += 1 << (win_count - 1);
        }

        // Reset the card
        memset(numbers, 0, sizeof(numbers));
    }
    
    printf("Part 1: %lu\n", solution_p1);

    return 0;
}