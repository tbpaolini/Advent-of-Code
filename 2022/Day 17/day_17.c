#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

// Coordinates of the blocks on the board
typedef struct BoardCoord
{
    int64_t x;
    int64_t y;
} BoardCoord;

typedef struct Board
{
    int64_t max_height;         // Maximum height among all columns in the board
    int64_t num_rows;           // Amount of available rows in the board
    int64_t trimmed_rows;       // Total amount of rows removed from the board (in order to save memory)
    uint8_t row[];              // Array of bitmasks to represent the rows (8-bit each)
} Board;

// Bitmasks to check if a piece is next to a wall
static const uint8_t RIGHT_BOUND = 0b0000001;   // Next to the right wall
static const uint8_t LEFT_BOUND  = 0b1000000;   // Next to the left wall

// All five pieces in order
static uint8_t pieces[5][4] = {
    // − piece
    {
        0b0011110,
        0b0000000,
        0b0000000,
        0b0000000,
    },

    // + piece
    {
        0b0001000,
        0b0011100,
        0b0001000,
        0b0000000,
    },

    // J piece
    {
        0b0000100,
        0b0000100,
        0b0011100,
        0b0000000,
    },

    // I piece
    {
        0b0010000,
        0b0010000,
        0b0010000,
        0b0010000,
    },

    // o piece
    {
        0b0011000,
        0b0011000,
        0b0000000,
        0b0000000,
    },
};

// Allocate memory for an empty game board
static Board* board_new(size_t height)
{
    
    size_t board_size = sizeof(Board) + (height * sizeof(uint8_t));
    Board *board = (Board*)calloc(1, board_size);
    
    if (!board)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    
    board->num_rows = height;

    return board;
}

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