#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BUF_SIZE 256
#define MAX_NUMBER 100
#define MAX_CARDS 202

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

    // Data for Part 1
    uint64_t solution_p1 = 0;
    char line[BUF_SIZE] = {0};      // A line from the input
    bool numbers[MAX_NUMBER] = {0}; // Keep track of the scored numbers

    // Data for Part 2
    uint64_t solution_p2 = 0;
    uint64_t num_count[MAX_CARDS] = {0};    // Counts of how many numbers each card scored
    uint64_t card_count[MAX_CARDS] = {0};   // Counts of how many cards of each

    /********** Part 1 ***********/

    // Parse the input line by line
    while (fgets(line, sizeof(line), file))
    {
        // Split the input line
        char *game = strtok(line, ":");     // Card count
        char *card = strtok(NULL, "|");     // Card's numbers
        char *win  = strtok(NULL, "\n");    // Winning numbers

        // Parse the card's index
        char *game_end = game;
        size_t card_id = strtoll(game+5, &game_end, 10);
        if (card_id >= MAX_CARDS || game == game_end)
        {
            fprintf(stderr, "Invalid card index.\n");
            exit(EXIT_FAILURE);
        }

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

        num_count[card_id] = win_count;
        card_count[card_id] += 1;

        // Reset the card
        memset(numbers, 0, sizeof(numbers));
    }
    
    fclose(file);
    printf("Part 1: %lu\n", solution_p1);

    /********** Part 2 ***********/

    // Loop through the indexes of all cards
    for (size_t i = 0; i < MAX_CARDS; i++)
    {
        // Loop through the obtained cards
        for (size_t j = i+1; j <= i + num_count[i]; j++)
        {
            if (j >= MAX_CARDS)
            {
                fprintf(stderr, "Card list overflow.\n");
                exit(EXIT_FAILURE);
            }
            
            // The amount of each next card that you win is the amount of the current card that you have
            card_count[j] += card_count[i];
        }
    }

    for (size_t i = 0; i < MAX_CARDS; i++)
    {
        solution_p2 += card_count[i];
    }
    printf("Part 2: %lu\n", solution_p2);

    return 0;
}