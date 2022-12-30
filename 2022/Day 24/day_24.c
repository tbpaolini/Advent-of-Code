#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// A coordinate on the map
typedef struct BasinCoord
{
    int64_t x;  // Increases from left to right
    int64_t y;  // Increases from top to bottom
} BasinCoord;

// A blizzard on the map
typedef struct Blizzard
{
    BasinCoord offset;      // Starting position on the map
    BasinCoord direction;   // Which axis and direction it moves on each step
} Blizzard;
/* The direction can only be:
    {0, +1}: right
    {0, -1}: left
    {+1, 0}: bottom
    {-1, 0}: top
*/

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[256];

    int64_t width  = 0;
    int64_t height = 0;
    
    size_t empty_count = 0;
    size_t blizz_count = 0;

    while (fgets(line, sizeof(line), input))
    {
        if (width == 0)
        {
            width = strlen(line) - 1;
        }
        else
        {
            assert(width == strlen(line) - 1);
        }

        height++;

        for (int64_t i = 0; i < width; i++)
        {
            const char next_char = line[i];
            switch (next_char)
            {
                case '>':
                case '<':
                case 'v':
                case '^':
                    blizz_count++;
                    break;
                
                case '.':
                    empty_count++;
                    break;
                
                case '#':
                    break;
                
                case '\n':
                    assert(i == width - 1);
                    break;
                
                default:
                    assert("Invalid input character");
                    break;
            }
        }
    }

    rewind(input);

    int64_t map[height][width];
    Blizzard blizzards[blizz_count];

    {
        BasinCoord my_coord = {-1, 0};
        size_t blizz_id = 0;
        char next_char;
        
        while ( (next_char = fgetc(input)) != EOF )
        {
            my_coord.x += 1;

            const int64_t x = my_coord.x;
            const int64_t y = my_coord.y;
            
            switch (next_char)
            {
                case '>':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {0, +1}};
                    map[y][x] = 1;
                    break;

                case '<':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {0, -1}};
                    map[y][x] = 1;
                    break;

                case 'v':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {+1, 0}};
                    map[y][x] = 1;
                    break;

                case '^':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {-1, 0}};
                    map[y][x] = 1;
                    break;
                
                case '.':
                    map[y][x] = 0;
                    break;
                
                case '#':
                    assert(y == 0 || x == 0 || x == width - 1 || y == height -1);
                    map[y][x] = -1;
                    break;
                
                case '\n':
                    my_coord.y += 1;
                    my_coord.x = -1;
                    break;
            }
        };
    }

    fclose(input);

    return 0;
}