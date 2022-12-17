#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

static const bool wall[9]  = {1,0,0,0,0,0,0,0,1};
static const bool floor[9] = {1,1,1,1,1,1,1,1,1};

// Coordinates of the blocks on the board
typedef struct BoardCoord
{
    int64_t x;
    int64_t y;
} BoardCoord;

typedef struct Board
{
    int64_t column_height[7];   // Maximum height of each column in the board
    int64_t height_limit;       // Amount of available rows in the board
    int64_t trimmed_rows;       // Total amount of rows removed from the board (in order to save memory)
    bool *board[];              // 2-D array to represent the board
} Board;

typedef struct Piece
{
    size_t size;            // How many blocks in the piece (4 or 5)
    BoardCoord block[5];    // The offsets of the blocks from the piece's origin (top left)
};

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