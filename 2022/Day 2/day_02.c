#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t RPS_Play;
static const RPS_Play ROCK     = 0;
static const RPS_Play PAPER    = 1;
static const RPS_Play SCISSORS = 2;

// Calculate your Rock Pape Scissors score
static uint64_t rps_score(RPS_Play yours, RPS_Play other)
{
    if (yours == other)
    {
        // Draw (both chose the same)
        return yours + 1 + 3;
    }
    else if ( (yours + 1) % 3 == other )    
    {
        // Loss (you chose the next play on the RPS sequence)
        return yours + 1;
    }
    else
    {
        // Win (you chose the previous play on the RPS sequence)
        return yours + 1 + 6;
    }
};

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[5];
    uint64_t score_p1 = 0;
    uint64_t score_p2 = 0;

    // Read the file line by line until the end
    while (fgets(line, sizeof(line), input) != NULL)
    {
        // Part 1
        RPS_Play yours_p1 = line[2] - 'X';          // Our play (third character on the line)
        RPS_Play other_p1 = line[0] - 'A';          // Opponent's play (first character)
        score_p1 += rps_score(yours_p1, other_p1);  // Our score

        // Part 2
        RPS_Play yours_p2;  // Our play will be determined by the opponent's play
        RPS_Play other_p2 = line[0] - 'A';  // Opponent's play
        
        switch (line[2])    // Instruction
        {
            case 'X':   // Force a loss
                yours_p2 = (other_p2 + 3 - 1) % 3;
                // Note: adding 3 in order to prevent an underflow
                break;
            
            case 'Y':   // Force a draw
                yours_p2 = other_p2;
                break;
            
            case 'Z':   // Force a win
                yours_p2 = (other_p2 + 1) % 3;
                break;
        }
        
        score_p2 += rps_score(yours_p2, other_p2);  // Our actual score
    }

    fclose(input);

    printf("Part 1: %lu points\n", score_p1);
    printf("Part 2: %lu points\n", score_p2);
}