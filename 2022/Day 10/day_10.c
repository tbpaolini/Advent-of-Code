#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <locale.h>

int main(int argc, char **argv)
{
    setlocale(LC_ALL, ".utf8");
    
    FILE *input = fopen("input.txt", "rt");
    char line[16];
    
    int64_t accumulator = 1;    // Total of the sum
    int64_t value       = 0;    // Value to be summed
    int64_t cooldown    = 0;    // How many cycles to complete the operation
    int64_t cycle       = 0;    // Count of elapsed cycles
    int64_t signal      = 0;    // Signal strength

    // The pixels on the screen
    bool screen[6][40];
    memset(screen, 0, sizeof(screen));
    size_t row    = 0;  // The scanline's index
    size_t column = 0;  // The pixel's index on the scanline

    // Parse the file line by line
    while (fgets(line, sizeof(line), input))
    {
        // Remove the newline character at the end
        line[strlen(line)-1] = '\0';
        
        // Count the cycles
        while (cooldown-- > 0)
        {
            // Increment the cycle counter
            cycle++;
            if (cycle > 240) break;
            
            // Calculate the signal strength
            if ( (cycle <= 220) && (cycle - 20) % 40 == 0 )
            {
                signal += accumulator * cycle;
            }

            // Draw the current pixel
            row    = (cycle - 1) / 40;
            column = (cycle - 1) % 40;
            if (column >= accumulator -1 && column <= accumulator + 1)
            {
                screen[row][column] = true;
            }
            
            // Add the value to the register at the end of the operation
            if (cooldown == 0)
            {
                accumulator += value;
            }
        }
        
        // Get the next instruction
        char delimiter[] = " ";
        char *instruction = strtok(line, delimiter);

        if (strcmp(instruction, "addx") == 0)
        {
            // Add the value
            cooldown = 2;
            value = atol(strtok(NULL, delimiter));
        }
        else if (strcmp(instruction, "noop") == 0)
        {
            // Do nothing for 1 cycle
            cooldown = 1;
            value = 0;
        }
        else
        {
            // We should never reach this branch
            fprintf(stderr, "Error: Illegal instruction '%s'\n", instruction);
            abort();
        }
    }
    
    fclose(input);

    printf("Part 1: %lu\n", signal);

    // Print the screen to the terminal
    printf("\nPart 2:\n\n");
    for (size_t row = 0; row < 6; row++)
    {
        for (size_t column = 0; column < 40; column++)
        {
            const char *pixel = screen[row][column] ? u8"█" : u8" ";
            printf("%s", pixel);
        }
        printf(u8"\n");
    }

    return 0;
}