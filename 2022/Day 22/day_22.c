#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

// A node on the board
typedef struct BoardNode
{
    int64_t row;                // Row number
    int64_t column;             // Column number
    struct BoardNode *exit[4];  // Pointers to the nodes this one connects to: {right, down, left, top}
} BoardNode;

// An instruction for how to move on the board
typedef struct BoardInstruction
{
    int64_t value;          // Distance to move or the direction to turn
    enum {MOVE, TURN} type; // Whether to move or turn
} BoardInstruction;

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char next_char = fgetc(input);

    int64_t width  = 0;
    int64_t height = 0;
    int64_t nodes_count = 0;

    // Determine the width and height of the map
    while (next_char != '\n')
    {
        // Amount of characters in the line
        size_t char_count = 0;
        while (next_char != '\n')
        {
            char_count++;
            if (next_char != ' ') nodes_count++;
            next_char = fgetc(input);
        }
        
        // The width is the maximum line length (not counting the line break)
        if (char_count > width) width = char_count;

        // The height is the amount of lines before the blank line
        height++;
        
        next_char = fgetc(input);
    }

    int64_t instructions_count = 1;
    while ( (next_char = fgetc(input)) != EOF )
    {
        if (isupper(next_char)) instructions_count += 2;
    }

    char temp_board[height][width];
    memset(temp_board, ' ', sizeof(temp_board));
    rewind(input);

    BoardInstruction *instruction = (BoardInstruction*)calloc(instructions_count, sizeof(BoardInstruction));
    BoardNode *board = (BoardNode*)calloc(nodes_count, sizeof(BoardNode));

    if (!instruction || !board)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    int64_t x = 0;
    int64_t y = 0;
    
    for (int64_t y = 0; y < height; y++)
    {
        while ( (next_char = fgetc(input)) != '\n')
        {
            temp_board[y][x++] = next_char;
        }
        x = 0;
    }

    fgetc(input);
    
    char buffer[21];
    size_t buff_id = 0;
    buffer[0] = '\0';
    size_t inst_id = 0;

    while (inst_id < instructions_count)
    {
        next_char = fgetc(input);
        if (isdigit(next_char))
        {
            buffer[buff_id++] = next_char;
        }
        else if (next_char == 'L' || next_char == 'R' || next_char == '\n')
        {
            buffer[buff_id++] = '\0';
            assert(buff_id <= sizeof(buffer));
            instruction[inst_id++] = (BoardInstruction){atol(buffer), MOVE};
            if (next_char == 'L')
            {
                instruction[inst_id++] = (BoardInstruction){-1, TURN};
            }
            else if (next_char == 'R')
            {
                instruction[inst_id++] = (BoardInstruction){+1, TURN};
            }
            buff_id = 0;
            buffer[0] = '\0';
        }
        else
        {
            fprintf(stderr, "Error: Malformatted input file\n");
            abort();
        }
        assert(buff_id <= sizeof(buffer));
    }

    free(instruction);
    free(board);
    
    return 0;
}