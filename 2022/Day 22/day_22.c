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
    struct BoardNode *exit_p1[4];   // Pointers to the nodes this one connects to on Part 1: {right, down, left, up}
    struct BoardNode *exit_p2[4];   // Pointers to the nodes this one connects to on Part 2: {right, down, left, up}
    int64_t dir_change[4];          // Part 2: To which direction taking the exit changes you
} BoardNode;

// An instruction for how to move on the board
typedef struct BoardInstruction
{
    int64_t value;          // Distance to move or the direction to turn
    enum {MOVE, TURN} type; // Whether to move or turn
} BoardInstruction;

// Current position on the board
typedef struct BoardPosition
{
    BoardNode *node;    // Pointer to the current node on the board's graph
    int64_t facing;     // Direction facing to (0 = right; 1 = down; 2 = left; 3 = top)
} BoardPosition;

// Directions
#define RIGHT_DIR 0 // >
#define DOWN_DIR  1 // v
#define LEFT_DIR  2 // <
#define UP_DIR    3 // ^

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

    // Starting node on the graph
    BoardNode *start_node = NULL;
    
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
                if (node_id == 0) start_node = &board[node_id]; // Node where we start from
                node_id++;
            }
            assert(node_id <= nodes_count);
        }
    }

    // Part 2: Determine the face size
    int64_t face_size = width / 3;
    assert(face_size == height / 4);
    assert(width % 3 == 0);
    assert(height % 4 == 0);
    /* Note: All inputs of this puzzle have the exact same shape and size. */

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
        for (int64_t exit = 0; exit < 4; exit++)
        {
            // Direction of the tentative movement
            const int64_t dir_x = my_dir[exit][0];
            const int64_t dir_y = my_dir[exit][1];
            
            // Coordinates of the tentative destination
            int64_t new_x = x + dir_x;
            int64_t new_y = y + dir_y;

            // Which direction the movement wraps to
            enum {W_RIGHT, W_DOWN, W_LEFT, W_UP, W_NONE} wrap = W_NONE;

            // Check if the tentative destination is past the borders of the map
            switch (exit)
            {
                case RIGHT_DIR:
                    if (new_x == width || temp_board[new_y][new_x] == ' ') wrap = W_LEFT;
                    break;
                
                case DOWN_DIR:
                    if (new_y == height || temp_board[new_y][new_x] == ' ') wrap = W_UP;
                    break;
                
                case LEFT_DIR:
                    if (new_x == -1 || temp_board[new_y][new_x] == ' ') wrap = W_RIGHT;
                    break;
                
                case UP_DIR:
                    if (new_y == -1 || temp_board[new_y][new_x] == ' ') wrap = W_DOWN;
                    break;
            }

            // Part 1: If beyond a border, move until the opposite border
            switch (wrap)
            {
                case W_RIGHT:
                    while (new_x != width-1 && temp_board[new_y][new_x+1] != ' ')
                    {
                        new_x += 1;
                    }
                    break;
                
                case W_DOWN:
                    while (new_y != height-1 && temp_board[new_y+1][new_x] != ' ')
                    {
                        new_y += 1;
                    }
                    break;
                
                case W_LEFT:
                    while (new_x != 0 && temp_board[new_y][new_x-1] != ' ')
                    {
                        new_x -= 1;
                    }
                    break;
                
                case W_UP:
                    while (new_y != 0 && temp_board[new_y-1][new_x] != ' ')
                    {
                        new_y -= 1;
                    }
                    break;
            }

            // Destination coordinates for Part 2
            int64_t cube_x = new_x;
            int64_t cube_y = new_y;
            
            // Part 2: If beyond a border, walk around the cube
            if (wrap != W_NONE)
            {
                // The coordinates on the face
                int64_t face_x = x % face_size;
                int64_t face_y = y % face_size;
                
                // Determine the quadrant of the current face
                int64_t quad_x = x / face_size; // Quadrant number counting from left
                int64_t quad_y = y / face_size; // Quadrant number counting from top 

                /* Quadrants: (quad_x, quad_y) 
                 *
                 *        (1,0) (2,0)
                 *        (1,1)
                 *  (0,2) (1,2)
                 *  (0,3)
                 *
                 * (each quadrant represents a face of the cube)
                 * */

                // By default, going through an exit does not change the direction
                // This value will be updated when we are moving into another face
                node->dir_change[exit] = -1;
                
                // Which exit we are going into
                switch (exit)
                {
                    case RIGHT_DIR:
                        switch (quad_y) // The quadrant counting from top
                        {
                            case 0:
                                cube_x = face_x + face_size;
                                cube_y = face_y + (2 * face_size);
                                node->dir_change[exit] = LEFT_DIR;
                                break;
                            case 1:
                                cube_x = face_y + (2 * face_size);
                                cube_y = face_x;
                                node->dir_change[exit] = UP_DIR;
                                break;
                            case 2:
                                cube_x = face_x + (2 * face_size);
                                cube_y = face_y;
                                node->dir_change[exit] = LEFT_DIR;
                                break;
                            case 3:
                                cube_x = face_y + face_size;
                                cube_y = face_x + (2 * face_size);
                                node->dir_change[exit] = UP_DIR;
                                break;
                        }
                        break;
                    
                    case DOWN_DIR:
                        switch (quad_x) // Quadrant counting from the left
                        {
                            case 0:
                                cube_x = face_y + (2 * face_size);
                                cube_y = face_x;
                                node->dir_change[exit] = DOWN_DIR;
                                break;
                            
                            case 1:
                                cube_x = face_y;
                                cube_y = face_x + (3 * face_size);
                                node->dir_change[exit] = LEFT_DIR;
                                break;
                            
                            case 2:
                                cube_x = face_y + face_size;
                                cube_y = face_x + face_size;
                                node->dir_change[exit] = LEFT_DIR;
                                break;
                        }
                        break;
                    
                    case LEFT_DIR:
                        switch (quad_y) // Quadrant counting from the top
                        {
                            case 0:
                                cube_x = face_x;
                                cube_y = face_y + (2 * face_size);
                                node->dir_change[exit] = RIGHT_DIR;
                                break;
                            
                            case 1:
                                cube_x = face_y;
                                cube_y = face_x + (2 * face_size);
                                node->dir_change[exit] = DOWN_DIR;
                                break;
                            
                            case 2:
                                cube_x = face_x + face_size;
                                cube_y = face_y;
                                node->dir_change[exit] = RIGHT_DIR;
                                break;
                            case 3:
                                cube_x = face_y + face_size;
                                cube_y = face_x;
                                node->dir_change[exit] = DOWN_DIR;
                                break;
                        }
                        break;
                    
                    case UP_DIR:
                        switch (quad_x) // Quadrant counting from the left
                        {
                            case 0:
                                cube_x = face_y + face_size;
                                cube_y = face_x + face_size;
                                node->dir_change[exit] = RIGHT_DIR;
                                break;
                            
                            case 1:
                                cube_x = face_y;
                                cube_y = face_x + (3 * face_size);
                                node->dir_change[exit] = RIGHT_DIR;
                                break;
                            
                            case 2:
                                cube_x = face_x;
                                cube_y = face_y + (3 * face_size);
                                node->dir_change[exit] = UP_DIR;
                                break;
                        }
                        break;
                }
            }

            // If arrived to an empty space, add a pointer to the exit node

            // Part 1's destination
            if (temp_board[new_y][new_x] == '.')
            {
                node->exit_p1[exit] = temp_exits[new_y][new_x];
            }
            else
            {
                assert(temp_board[new_y][new_x] == '#');
                node->exit_p1[exit] = NULL;
            }

            // Part 2's destination
            if (temp_board[cube_y][cube_x] == '.')
            {
                node->exit_p2[exit] = temp_exits[cube_y][cube_x];
            }
            else
            {
                assert(temp_board[cube_y][cube_x] == '#');
                node->exit_p2[exit] = NULL;
            }
        }
    }

    // Initial position
    BoardPosition current_pos_p1 = {
        .node = start_node, // The most top left available space of the board
        .facing = 0         // Facing right
    };

    // Process the instructions
    for (size_t i = 0; i < instructions_count; i++)
    {
        BoardInstruction movement = instruction[i];
        if (movement.type == MOVE)
        {
            int64_t steps = movement.value;
            for (int64_t i = 0; i < steps; i++)
            {
                const int64_t dir = current_pos_p1.facing;
                BoardNode *exit = current_pos_p1.node->exit_p1[dir];
                if (exit) current_pos_p1.node = exit;
            }
        }
        else if (movement.type == TURN)
        {
            current_pos_p1.facing += movement.value;
            if (current_pos_p1.facing < 0) current_pos_p1.facing = 3;
            if (current_pos_p1.facing > 3) current_pos_p1.facing = 0;
        }
        else
        {
            fprintf(stderr, "Error: Illegal movement\n");
            abort();
        }
    }

    int64_t password_p1 = 1000 * (current_pos_p1.node->y + 1) + 4 * (current_pos_p1.node->x + 1) + current_pos_p1.facing;

    free(instruction);
    free(board);
    
    return 0;
}

#undef RIGHT_DIR
#undef DOWN_DIR
#undef LEFT_DIR
#undef UP_DIR