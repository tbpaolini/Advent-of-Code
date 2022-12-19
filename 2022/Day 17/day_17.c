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
    int64_t min_rows;           // Keep at least this amount of rows when trimming the board
    int64_t trimmed_rows;       // Total amount of rows removed from the board (in order to save memory)
    uint8_t row[];              // Array of bitmasks to represent the rows (8-bit each)
} Board;

// Bitmasks to represent a piece (8 bits per row)
typedef uint8_t Piece[4];

// All five pieces in order
static Piece pieces[5] = {
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


// Bitmasks to check if a piece is next to a wall

Piece RIGHT_WALL = {
    0b0000001,
    0b0000001,
    0b0000001,
    0b0000001,
};

Piece LEFT_WALL = {
    0b1000000,
    0b1000000,
    0b1000000,
    0b1000000,
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
    board->min_rows = height / 4;

    return board;
}

static void board_run(
    Board *board,       // Data structure of the board
    char *movements,    // Array with the movements to be performed on the pieces
    size_t mov_lenght,  // Amount of elements on the movement array
    uint64_t pieces     // For how many pieces to simulate the game
)
{

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
    if (input_size > 1000000)
    {
        fprintf(stderr, "Error: Maximum input size is 1 MB\n");
        abort();
    }
    
    rewind(input);              // Return to the start of the file
    char movements[input_size]; // Array to store the jet stream's directions
    
    for (size_t i = 0; i < input_size; i++)
    {
        next_char = fgetc(input);

        if (next_char == '<' || next_char == '>')
        {
            movements[i] = next_char;
        }
        else
        {
            fprintf(stderr, "Error: Malformatted input file\n");
            abort();
        }
    }

    fclose(input);

    return 0;
}