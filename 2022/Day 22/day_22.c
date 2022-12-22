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
    int64_t x;  // Horizontal coordinate (0-indexed)
    int64_t y;  // Vertical coordinate (0-indexed)
    struct BoardNode *exit[4];  // Pointers to the nodes this one connects to: {right, down, left, up}
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
            if (next_char == '.') nodes_count++;
            next_char = fgetc(input);
        }
        
        // The width is the maximum line length (not counting the line break)
        if (char_count > width) width = char_count;

        // The height is the amount of lines before the blank line
        height++;
        
        next_char = fgetc(input);
    }

    // Count how many instructions
    int64_t instructions_count = 1;
    while ( (next_char = fgetc(input)) != EOF )
    {
        if (isupper(next_char)) instructions_count += 2;
    }

    // Temporary array for the board
    char temp_board[height][width];
    memset(temp_board, ' ', sizeof(temp_board));

    // Allocate memory for the graph and the instruction array
    BoardInstruction *instruction = (BoardInstruction*)calloc(instructions_count, sizeof(BoardInstruction));
    BoardNode *board = (BoardNode*)calloc(nodes_count, sizeof(BoardNode));

    if (!instruction || !board)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    rewind(input);

    // Parse the board
    for (int64_t y = 0; y < height; y++)
    {
        int64_t x = 0;
        while ( (next_char = fgetc(input)) != '\n')
        {
            if (next_char != ' ' && next_char != '.' && next_char != '#')
            {
                fprintf(stderr, "Error: Malformatted input file\n");
                abort();
            }
            temp_board[y][x++] = next_char;
        }
    }

    fgetc(input);   // Skip the blank line
    
    // Buffer for storing the instruction's value
    char buffer[22];    // Enough characters for a 64-bit signed integer
    size_t buff_id = 0; // Position on the buffer
    buffer[0] = '\0';
    
    size_t inst_id = 0; // Counter of instructions

    // Parse the instructions
    while (inst_id < instructions_count)
    {
        // Next character on the input
        next_char = fgetc(input);
        
        if (isdigit(next_char))
        {
            // Add the digits to the buffer in sequence
            buffer[buff_id++] = next_char;
        }
        else if (next_char == 'L' || next_char == 'R' || next_char == '\n')
        {
            // A non-digit character means that we have the whole numeric string
            buffer[buff_id++] = '\0';
            assert(buff_id <= sizeof(buffer));
            
            // Convert the string to numeric and add to the instruction array
            instruction[inst_id++] = (BoardInstruction){atol(buffer), MOVE};
            
            // Parse the direction to turn and add it to the instructions
            if (next_char == 'L')
            {
                instruction[inst_id++] = (BoardInstruction){-1, TURN};
            }
            else if (next_char == 'R')
            {
                instruction[inst_id++] = (BoardInstruction){+1, TURN};
            }

            // Reset the buffer
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

    fclose(input);

    // Temporary array to store the pointers to the nodes on the graph
    BoardNode *temp_exits[height][width];
    memset(temp_exits, 0, sizeof(temp_exits));
    int64_t node_id = 0;    // Position on the 'temp_exits' array
    
    // Loop through all spaces of the board
    for (int64_t y = 0; y < height; y++)
    {
        for (int64_t x = 0; x < width; x++)
        {
            // Check if this space is empty
            if (temp_board[y][x] == '.')
            {
                board[node_id] = (BoardNode){.x = x, .y = y};   // Create a node
                temp_exits[y][x] = &board[node_id];             // Store the node's pointer
                node_id++;
            }
            assert(node_id <= nodes_count);
        }
    }

    // Link the nodes to each other
    for (int64_t i = 0; i < nodes_count; i++)
    {
        // Retrieve the node and its coordinates
        BoardNode *node = &board[i];
        const int64_t x = node->x;
        const int64_t y = node->y;
        assert(temp_board[y][x] == '.');

        // The four possible exits of the node
        int64_t my_dir[4][2] = {
            {+1, 0},    // Right
            {0, +1},    // Down
            {-1, 0},    // Left
            {0, -1},    // Up
        };

        // Check if we can use the exits and where they lead to
        for (size_t exit = 0; exit < 4; exit++)
        {
            // Direction of the tentative movement
            const int64_t dir_x = my_dir[exit][0];
            const int64_t dir_y = my_dir[exit][1];
            
            // Coordinates of the tentative destination
            int64_t new_x = x + dir_x;
            int64_t new_y = y + dir_y;

            // Which direction the movement wraps to
            enum {RIGHT, DOWN, LEFT, UP, NONE} wrap = NONE;

            // Check if the tentative destination is past the borders of the map
            switch (exit)
            {
                case 0: // Right exit
                    if (new_x == width || temp_board[new_y][new_x] == ' ') wrap = LEFT;
                    break;
                
                case 1: // Down exit
                    if (new_y == height || temp_board[new_y][new_x] == ' ') wrap = UP;
                    break;
                
                case 2: // Left exit
                    if (new_x == -1 || temp_board[new_y][new_x] == ' ') wrap = RIGHT;
                    break;
                
                case 3: // Up exit
                    if (new_y == -1 || temp_board[new_y][new_x] == ' ') wrap = DOWN;
                    break;
            }

            // If beyond a border, move until the opposite border
            switch (wrap)
            {
                case RIGHT:
                    while (new_x != width-1 && temp_board[new_y][new_x+1] != ' ')
                    {
                        new_x += 1;
                    }
                    break;
                
                case DOWN:
                    while (new_y != height-1 && temp_board[new_y+1][new_x] != ' ')
                    {
                        new_y += 1;
                    }
                    break;
                
                case LEFT:
                    while (new_x != 0 && temp_board[new_y][new_x-1] != ' ')
                    {
                        new_x -= 1;
                    }
                    break;
                
                case UP:
                    while (new_y != 0 && temp_board[new_y-1][new_x] != ' ')
                    {
                        new_y -= 1;
                    }
                    break;
            }

            // If arrived to an empty space, add a pointer to the exit node
            if (temp_board[new_y][new_x] == '.')
            {
                node->exit[exit] = temp_exits[new_y][new_x];
            }
            else
            {
                assert(temp_board[new_y][new_x] == '#');
                node->exit[exit] = NULL;
            }
        }
    }

    free(instruction);
    free(board);
    
    return 0;
}