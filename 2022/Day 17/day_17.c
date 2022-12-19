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

// Initial sistance that the top left of a piece will be from the highest block in the board
static int64_t piece_h_offsets[5] = {3, 5, 5, 6, 4};

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
    uint64_t num_pieces // For how many pieces to simulate the game
)
{
    size_t piece_id = 0;    // Cycle through all 5 pieces
    size_t move_id = 0;     // Cycle through the movements
    
    // Load the current piece
    Piece my_piece;
    memcpy(my_piece, &pieces[piece_id], sizeof(Piece));
    
    // Vertical coordinate in which the piece spawns 
    int64_t piece_y = board->max_height + piece_h_offsets[piece_id];

    // Amount of pieces that landed so far
    int64_t landed_pieces = 0;
    
    while (landed_pieces < num_pieces)
    {
        // Check if the piece falls outside the board's height limit
        if (piece_y >= board->num_rows)
        {
            // Truncate the board if the piece would be above the limit
            const int64_t trim_size = board->num_rows - board->min_rows;  // Amount of rows to cut off
            memmove(&board->row[0], &board->row[trim_size], board->min_rows); // Move the remaining rows to the start
            memset(&board->row[board->min_rows], 0, trim_size);              // Clear the rows after the moved ones

            // Keep track of how many rows were removed
            board->trimmed_rows += trim_size;
            board->max_height -= trim_size;

            // Update the piece's horizontal coordinate
            piece_y -= trim_size;
        }

        // Get the next horizontal movement
        char my_move = movements[move_id];
        
        // Read the 4 bytes of the piece as one 32-bit unsigned integer
        uint32_t *my_piece_ui32 = (uint32_t*)(&my_piece);
        uint32_t tentative_move = 0;

        // Check if the piece is next to a wall on the direction it is being pushed.
        // If not, then calculate the position where the piece is trying to move horizontally.
        if ( my_move == '>' && !(*my_piece_ui32 & *(uint32_t*)(&RIGHT_WALL)) )
        {
            tentative_move = *my_piece_ui32 >> (uint32_t)1;
        }
        else if ( my_move == '<' && !(*my_piece_ui32 & *(uint32_t*)(&LEFT_WALL)) )
        {
            tentative_move = *my_piece_ui32 << (uint32_t)1;
        }

        // If the piece is not trying to move into a wall,
        // check if there is another piece on the destination
        if (tentative_move)
        {
            bool can_move = true;   // Whether the piece can move
            uint8_t *tentative_piece = (uint8_t*)(&tentative_move); // Each byte of the piece value represents a row
            
            // Loop through each row of the piece
            for (int64_t i = 0; i < 4; i++)
            {
                // Stop if we reach the end of the board
                if (piece_y - i < 0) break;
                assert(piece_y - i < board->num_rows);
                
                // Check if the piece overlaps with another piece
                if (tentative_piece[i] & board->row[piece_y - i])
                {
                    can_move = false;
                    break;
                }
            }

            // If there is no collision, then move the piece horizontally
            if (can_move)
            {
                *my_piece_ui32 = tentative_move;
            }
        }


        // Check if there is a block below the piece
        bool block_below = false;

        for (int64_t i = 0; i < 4; i++)
        {
            if (
                (my_piece[i] && piece_y - i == 0) ||        // Is the piece at the bottom?
                (my_piece[i] & board->row[piece_y - i - 1]) // If the piece moved down, would it overlap with a block?
            )
            {
                block_below = true;
                break;
            }
        }

        if (block_below)
        {
            // Land the piece if there is something below
            for (int64_t i = 0; i < 4; i++)
            {
                // Stop if we reach the end of the board
                if (piece_y - i < 0) break;
                
                // Add the piece to the board array
                board->row[piece_y - i] |= my_piece[i];
            }

            // Update the new maximum height of the board
            if (piece_y + 1 > board->max_height) board->max_height = piece_y + 1;
            
            // Load the next piece
            if (++piece_id == 5) piece_id = 0;
            memcpy(my_piece, &pieces[piece_id], sizeof(Piece));
            piece_y = board->max_height + piece_h_offsets[piece_id];

            // Increment the pieces counter
            landed_pieces++;
        }
        else
        {
            // Move the piece down by one unit, if there is nothing below
            piece_y--;
        }
        
        // Go to the next movement
        if (++move_id == mov_lenght) move_id = 0;
    }
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

    Board *board = board_new(1024);
    board_run(board, movements, input_size, 2022);

    printf("Part 1: %lu rocks\n", board->max_height + board->trimmed_rows);

    return 0;
}