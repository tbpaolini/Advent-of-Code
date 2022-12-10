#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[16];
    
    int64_t accumulator = 1;    // Total of the sum
    int64_t value       = 0;    // Value to be summed
    int64_t cooldown    = 0;    // How many cycles to complete the operation
    int64_t cycle       = 0;    // Count of elapsed cycles
    int64_t signal      = 0;    // Signal strength

    while (fgets(line, sizeof(line), input))
    {
        // Remove the newline character at the end
        line[strlen(line)-1] = '\0';
        
        while (cooldown-- > 0)
        {
            cycle++;
            
            if ( (cycle <= 220) && (cycle - 20) % 40 == 0 )
            {
                signal += accumulator * cycle;
            }
            
            if (cooldown == 0)
            {
                accumulator += value;
            }
        }
        
        char delimiter[] = " ";
        char *instruction = strtok(line, delimiter);

        if (strcmp(instruction, "addx") == 0)
        {
            cooldown = 2;
            value = atol(strtok(NULL, delimiter));
        }
        else if (strcmp(instruction, "noop") == 0)
        {
            cooldown = 1;
            value = 0;
        }
        else
        {
            fprintf(stderr, "Error: Illegal instruction '%s'\n", instruction);
            abort();
        }
    }
    
    fclose(input);

    return 0;
}