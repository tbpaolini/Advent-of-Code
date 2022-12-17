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
    int64_t column_height[7];   // Maximum height of each column in the board
    int64_t height_limit;       // Amount of available rows in the board
    int64_t trimmed_rows;       // Total amount of rows removed from the board (in order to save memory)
    bool *coord[];              // 2-D array of booleans to represent if some coordinate has a block: .coord[y][x]
} Board;

typedef struct Piece
{
    size_t size;                // How many blocks in the piece (4 or 5)
    struct BoardCoord block[5]; // The offsets of the blocks from the piece's origin (top left)
} Piece;

static Board* board_new(size_t height)
{
    static const bool wall[9]  = {1,0,0,0,0,0,0,0,1};
    static const bool floor[9] = {1,1,1,1,1,1,1,1,1};
    
    height += 1;
    size_t row_size = 9 * height * sizeof(bool);
    size_t board_size = sizeof(Board) + (height * sizeof(bool*)) + row_size;
    
    Board *board = (Board*)malloc(board_size);
    if (!board)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    memset(board, 0, sizeof(Board));
    board->height_limit = height;
    
    ptrdiff_t row_ptr = (ptrdiff_t)(&board->coord[0]);

    for (size_t i = 0; i < height; i++)
    {
        board->coord[i] = (bool*)row_ptr;
        row_ptr += sizeof(bool*);
        memcpy(board->coord[i], wall, row_size);
    }
    
    memcpy(board->coord[height-1], floor, row_size);

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