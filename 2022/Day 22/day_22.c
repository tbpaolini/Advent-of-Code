#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char next_char = fgetc(input);

    int64_t width  = 0;
    int64_t height = 0;

    // Determine the width and height of the map
    while (next_char != '\n')
    {
        // Amount of characters in the line
        size_t char_count = 0;
        while (next_char != '\n')
        {
            char_count++;
            next_char = fgetc(input);
        }
        
        // The width is the maximum line length (not counting the line break)
        if (char_count > width) width = char_count;

        // The height is the amount of lines before the blank line
        height++;
        
        next_char = fgetc(input);
    }

    char temp_board[height][width];
    memset(temp_board, ' ', sizeof(temp_board));
    rewind(input);

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

    next_char = fgetc(input);
    
    return 0;
}