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

typedef struct BasinNode
{
    BasinCoord coord;
    int64_t minute;
    struct BasinNode *next;
} BasinNode;

typedef struct BasinQueue
{
    struct BasinNode *head;
    struct BasinNode *tail;
} BasinQueue;

static BasinQueue* queue_new()
{
    BasinQueue *queue = (BasinQueue*)calloc(1, sizeof(BasinQueue));
    if (!queue)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return queue;
}

static inline void queue_push(BasinQueue *queue, BasinCoord coord, int64_t minute)
{
    BasinNode *node = (BasinNode*)malloc(sizeof(BasinNode));
    if (!node)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    *node = (BasinNode){
        .coord = coord,
        .minute = minute,
        .next = NULL
    };

    if (queue->tail)
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    else
    {
        assert(queue->head == NULL);
        queue->head = node;
        queue->tail = node;
    }
}

static inline bool queue_pop(BasinQueue *queue, BasinCoord *out_coord, int64_t *out_minute)
{
    if (!queue->head) return false;

    *out_coord = queue->head->coord;
    *out_minute = queue->head->minute;

    BasinNode *old_node = queue->head;
    queue->head = queue->head->next;
    free(old_node);
    
    return true;
}

static inline void queue_destroy(BasinQueue *queue)
{
    BasinNode *node = queue->head;
    while (node)
    {
        BasinNode *next_node = node->next;
        free(node);
        node = next_node;
    }
    free(queue);
}

static size_t least_common_multiple(size_t value1, size_t value2)
{
    size_t small, big;
    if (value1 < value2)
    {
        small = value1;
        big = value2;
    }
    else
    {
        small = value2;
        big = value1;
    }

    size_t lcm = 1;
    
    for (size_t i = 2; i <= small; i++)
    {
        if (big % i == 0 && small % i == 0)
        {
            big /= i;
            small /= i;
            lcm *= i;
        }
    }
    
    return lcm * small * big;
}

static inline BasinCoord blizzard_position(Blizzard blizzard, int64_t minute, size_t width, size_t height)
{
    int64_t x = (blizzard.offset.x-1 + minute*blizzard.direction.x) % (width -2);
    int64_t y = (blizzard.offset.y-1 + minute*blizzard.direction.y) % (height-2);

    if (x < 0) x += width -2;
    if (y < 0) y += height-2;

    x += 1;
    y += 1;

    return (BasinCoord){x, y};
}

static int64_t pathfind_bfs(
    size_t width,                       // Width of the map
    size_t height,                      // Height of the map
    bool map[height][width],            // 2-D array representing the map
    BasinCoord start,                   // Initial position of the map
    BasinCoord end,                     // Destination on the map
    size_t blizz_count,                 // Amount of blizzards
    Blizzard blizzards[blizz_count],    // Array of blizzards
    int64_t minute                      // Minute in which the map is currently on
)
{
    // Amount of different states for the blizzards
    const size_t num_states = least_common_multiple(width - 2, height - 2);
    
    // Cache for the possible blizzard states
    bool map_states[num_states][height][width];
    memset(map_states, 0, sizeof(map_states));

    // Simulate all different states for all blizzards
    for (size_t time = 0; time < num_states; time++)
    {
        for (size_t i = 0; i < blizz_count; i++)
        {
            // Calculate the blizzard's position at the current minute,
            // then store its position on the map
            const BasinCoord my_coord = blizzard_position(blizzards[i], time, width, height);
            map_states[time][my_coord.y][my_coord.x] = true;

        }
        
        // Add the walls to the simulated map
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                if (map[y][x]) map_states[time][y][x] = true;
            }
        }
    }
    
    BasinCoord coord = start;
    BasinQueue *queue = queue_new();

    while (coord.x != end.x && coord.y != end.y)
    {
        // TO DO: Update blizzards' positions
        
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
            if (new.y > 0 && new.y < height && map[new.y][new.x] == 0)
            {
                queue_push(queue, new, minute + 1);
            }
        }
        
        bool has_exit = queue_pop(queue, &coord, &minute);
        if (!has_exit)
        {
            fprintf(stderr, "Error: Path not found\n");
            return INT64_MAX;
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

    // 2-D array of booleans to represent the map
    // (a value of 'true' means that the position is blocked)
    bool map[height][width];

    // Array for storing the initial positions of the blizzards
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
                    map[y][x] = true;
                    break;

                case '<':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {0, -1}};
                    map[y][x] = true;
                    break;

                case 'v':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {+1, 0}};
                    map[y][x] = true;
                    break;

                case '^':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {-1, 0}};
                    map[y][x] = true;
                    break;
                
                case '.':
                    map[y][x] = false;
                    break;
                
                case '#':
                    assert(y == 0 || x == 0 || x == width - 1 || y == height -1);
                    map[y][x] = true;
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
    assert(map[start_coord.y][start_coord.x] == 0 && map[end_coord.y][end_coord.x] == 0);

    pathfind_bfs(width, height, map, start_coord, end_coord, blizz_count, blizzards, 0);

    return 0;
}