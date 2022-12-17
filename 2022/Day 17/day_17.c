#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char next_char = '\0';

    // Get the file size
    fseek(input, 0, SEEK_END);
    long input_size = ftell(input);
    
    // Check if the file ends in a line break.
    // If yes, subtract the break from the size.
    fseek(input, -2, SEEK_CUR);
    next_char = fgetc(input);
    if (next_char == '\r') input_size -= 1;
    next_char = fgetc(input);
    if (next_char == '\n') input_size -= 1;

    // Sanity check for the file size
    if (input_size > 125000)
    {
        fprintf(stderr, "Error: Maximum input size is 125 kB\n");
        abort();
    }
    
    rewind(input);                  // Return to the start of the file
    int64_t direction[input_size];  // Array to store the jet stream's directions
    
    for (size_t i = 0; i < input_size; i++)
    {
        next_char = fgetc(input);
        
        switch (next_char)
        {
            case '<':
                direction[i] = -1;
                break;
            
            case '>':
                direction[i] = +1;
                break;
            
            default:
                fprintf(stderr, "Error: Malformatted input file\n");
                abort();
                break;
        }
    }

    fclose(input);

    return 0;
}