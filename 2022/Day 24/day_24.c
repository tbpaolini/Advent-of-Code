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

static int64_t pathfind_bfs(
    size_t width,                       // Width of the map
    size_t height,                      // Height of the map
    int64_t map[height][width],         // 2-D array representing the map
    BasinCoord start,                   // Initial position of the map
    BasinCoord end,                     // Destination on the map
    size_t blizz_count,                 // Amount of blizzards
    Blizzard blizzards[blizz_count],    // Array of blizzards
    size_t empty_count,                 // Amount of empty spaces on the map
    int64_t minute                      // Minute in which the map is currently on
)
{
    BasinCoord coord = start;
    
    struct Queue
    {
        BasinCoord coord;
        int64_t minute;
    } queue[empty_count];

    size_t queue_tail = 0;

    queue[queue_tail++] = (struct Queue){
        .coord = start,
        .minute = minute+1
    };

    while (coord.x != end.x && coord.y != end.y)
    {
        const int64_t x = coord.x;
        const int64_t y = coord.y;
        
        BasinCoord exits[4] = {
            {x, y+1},
            {x+1, y},
            {x-1, y},
            {x, y-1},
        };

        for (size_t i = 0; i < 4; i++)
        {
            const BasinCoord new = exits[i];
            if (new.y > 0 && new.y < height && map[y][x] == 0)
            {
                queue[queue_tail++] = (struct Queue){{x, y}, minute+1};
            }
        }
        
    }
    
}

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

    BasinCoord start_coord = {1, 0};
    BasinCoord end_coord   = {width - 2, height - 1};
    assert(map[start_coord.x][start_coord.y] == 0 && map[end_coord.x][end_coord.y] == 0);

    return 0;
}