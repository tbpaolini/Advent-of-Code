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

// A node of the queue
typedef struct BasinNode
{
    BasinCoord coord;       // Current coordinate on the map
    int64_t minute;         // Current minute on the path
    struct BasinNode *next; // Next node on the queue
} BasinNode;

// A queue of nodes (first in, last out)
typedef struct BasinQueue
{
    struct BasinNode *head; // Oldest node on the queue
    struct BasinNode *tail; // Newest node on the queue
    size_t length;
} BasinQueue;

// Create a new queue
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

// Append a node to the end of the queue
static inline void queue_push(BasinQueue *queue, BasinCoord coord, int64_t minute)
{
    // Create the node
    BasinNode *node = (BasinNode*)malloc(sizeof(BasinNode));
    if (!node)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    // Initialize the node
    *node = (BasinNode){
        .coord = coord,
        .minute = minute,
        .next = NULL
    };

    // Add the node to the end of the queue
    if (queue->tail)
    {
        // Link the previous node to the current, then set the tail to the current
        queue->tail->next = node;
        queue->tail = node;
    }
    else
    {
        // If this is the first node, set the head and tail to it
        assert(queue->head == NULL);
        queue->head = node;
        queue->tail = node;
    }

    // Increment the node counter
    queue->length++;
}

// Remove a node from the beginning of the queue
// The function writes the node's coordinates and minute to the arguments 'out_coord' and 'out_minute'.
// It returns 'true' if a node was popped, or 'false' if the queue was empty.
static inline bool queue_pop(BasinQueue *queue, BasinCoord *out_coord, int64_t *out_minute)
{
    // If there are no nodes
    if (!queue->head) return false;

    // Write the coordinates and minute
    *out_coord = queue->head->coord;
    *out_minute = queue->head->minute;

    // Move the queue's head to the next node, then free the popped node
    BasinNode *old_node = queue->head;
    queue->head = queue->head->next;
    free(old_node);
    if (!queue->head) queue->tail = NULL;

    // Decrement the node counter
    queue->length--;
    
    return true;
}

// Free the memory used by a queue
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

// Find the least common multiple between two values
static size_t least_common_multiple(size_t value1, size_t value2)
{
    // Sort the values
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

    // Accumulator to store the least common multiple
    size_t lcm = 1;
    
    // Loop through the values from 2 up to the smallest value
    for (size_t i = 2; i <= small; i++)
    {
        // Check if both values are divisible by the tested value
        if (big % i == 0 && small % i == 0)
        {
            // If so, divide both values by it, and multiply the accumulator by it
            big /= i;
            small /= i;
            lcm *= i;
        }
        /* Note:
            The loop can exit earlier if at any point the tested value is
            bigger than the small value (there's no need for testing beyond that).
        */
    }
    
    // Multiply the accumulator by both values in order to get the least common multiple
    return lcm * small * big;
}

// Calculate the coordinates of a blizzard at a given minute
static inline BasinCoord blizzard_position(Blizzard blizzard, int64_t minute, int64_t width, int64_t height)
{
    // The map is considered to have walls on all four corners
    // (the walls are removed before calculating the modulo)
    int64_t x = (blizzard.offset.x-1 + minute*blizzard.direction.x) % (width -2);
    int64_t y = (blizzard.offset.y-1 + minute*blizzard.direction.y) % (height-2);

    // If the resulting coordinate is negative, wrap around to the other side
    if (x < 0) x += width -2;
    if (y < 0) y += height-2;

    // Add back the walls
    x += 1;
    y += 1;

    // The blizzard's coordinates
    return (BasinCoord){x, y};
}

// Perform a Breadth-First Search in order to find the shortest path between two points on the map.
// The function returns the amount of minutes that the path took.
static int64_t pathfind_bfs(
    size_t width,                       // Width of the map
    size_t height,                      // Height of the map
    bool map[height][width],            // 2-D array representing the map
    BasinCoord start,                   // Initial position of the map
    BasinCoord end,                     // Destination on the map
    size_t blizz_count,                 // Amount of blizzards
    Blizzard blizzards[blizz_count],    // Array of blizzards
    size_t minute                       // Minute in which the map is currently on
)
{
    // Amount of different states for the blizzards
    const size_t num_states = least_common_multiple(width - 2, height - 2);
    
    // Cache for the possible blizzard states
    bool map_states[num_states][height][width];
    memset(map_states, true, sizeof(map_states));

    // Simulate all different states for all blizzards
    for (size_t time = 0; time < num_states; time++)
    {
        for (size_t i = 0; i < blizz_count; i++)
        {
            // Calculate the blizzard's position at the current minute,
            // then store its position on the map
            const BasinCoord my_coord = blizzard_position(blizzards[i], time, width, height);
            map_states[time][my_coord.y][my_coord.x] = false;
        }
        
        // Add the walls to the simulated map
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                if (!map[y][x]) map_states[time][y][x] = false;
            }
        }
    }

    // Seen positions on the map
    // Note: For the purpose of flagging a node as "seen",
    //       I am considering each map state as a separate map.
    bool seen[num_states][height][width];
    memset(seen, 0, sizeof(seen));
    
    // Starting position and state on the map
    BasinCoord coord = start;
    BasinQueue *queue = queue_new();
    size_t state = minute % num_states;

    // Flag the starting position as "seen"
    seen[state][coord.y][coord.x] = true;

    // Keep searching for paths until we arrive to the destination
    while ( !(coord.x == end.x && coord.y == end.y) )
    {
        // Current coordinates on the map
        const int64_t x = coord.x;
        const int64_t y = coord.y;
        
        // Next map state
        minute += 1;
        state = minute % num_states;
        
        // Available exits on that state
        BasinCoord exits[5] = {
            {x, y+1},   // Bottom
            {x+1, y},   // Left
            {x-1, y},   // Right
            {x, y-1},   // Top
            {x,   y},   // Wait (do not move)
        };

        // Check all exits
        for (size_t i = 0; i < 5; i++)
        {
            const BasinCoord new = exits[i];
            if (
                new.y >= 0 && new.y < height        // Bound check (do not go outside the map)
                && map_states[state][new.y][new.x]  // Is the exit unblocked?
                && !seen[state][new.y][new.x]       // Was this exit not seen yet?
            )
            {
                // Flag the current position as "visited"
                seen[state][new.y][new.x] = true;
                
                // Enqueue the exit node to be visited
                queue_push(queue, new, minute);
            }
        }
        
        // Dequeue the next node to be visited
        // (the oldest enqueued nodes are dequeued first)
        bool has_exit = queue_pop(queue, &coord, &minute);
        if (!has_exit)
        {
            queue_destroy(queue);
            fprintf(stderr, "Error: Path not found\n");
            return INT64_MAX;
        }

        // Sanity check to limit the queue size to 100 MB
        // (to prevent mistakes from crashing the system)
        assert(queue->length <= 100000000 / sizeof(BasinNode));
        /* Note:
            The actual size will be far less than that,
            if the algorithm is working and the map is not too big.
        */
    }

    queue_destroy(queue);
    
    return minute;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[256];

    // Map's dimensions
    int64_t width  = 0;
    int64_t height = 0;
    
    size_t empty_count = 0; // Amount of free spaces on the map
    size_t blizz_count = 0; // Amount of blizzards on the map

    // Determine the map's dimensions
    while (fgets(line, sizeof(line), input))
    {
        if (width == 0)
        {
            // The width is the amount of characters on the line,
            // not considering the newline character
            width = strlen(line) - 1;
        }
        else
        {
            // All lines must have the same length
            assert(width == strlen(line) - 1);
        }

        // The height is the amount of newline characters
        height++;

        // Validate the characters on the line
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

    // Return to the beginning of the input file
    rewind(input);

    // 2-D array of booleans to represent the map
    // (a value of 'true' means that the position is free)
    bool map[height][width];
    memset(map, true, sizeof(map));
    /* Note:
        This map only keep track of the walls.
        The blizzards are going to be checked by a different map inside the pathfind function.
    */

    // Array for storing the initial positions of the blizzards
    Blizzard blizzards[blizz_count];

    {
        // Current coordinate on the map
        BasinCoord my_coord = {-1, 0};
        size_t blizz_id = 0;
        char next_char;
        
        while ( (next_char = fgetc(input)) != EOF )
        {
            // Each new character moves the x coordinate to the right
            my_coord.x += 1;

            const int64_t x = my_coord.x;
            const int64_t y = my_coord.y;
            
            // Process each character on the line
            switch (next_char)
            {
                // Set the starting positions and direction of the blizzards
                case '>':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {+1, 0}};
                    break;

                case '<':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {-1, 0}};
                    break;

                case 'v':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {0, +1}};
                    break;

                case '^':
                    blizzards[blizz_id++] = (Blizzard){my_coord, {0, -1}};
                    break;
                
                // Empty space
                case '.':
                    break;
                
                // Wall
                case '#':
                    assert(y == 0 || x == 0 || x == width - 1 || y == height -1);
                    map[y][x] = false;
                    break;
                
                // New row
                case '\n':
                    my_coord.y += 1;
                    my_coord.x = -1;
                    break;
            }
        };
    }

    fclose(input);

    /******************** Part 1 ********************/

    // Set the starting position and the destination
    BasinCoord start_coord = {1, 0};
    BasinCoord end_coord   = {width - 2, height - 1};
    assert(map[start_coord.y][start_coord.x] && map[end_coord.y][end_coord.x]);

    // Find the shortest path between those two positions
    int64_t minute_p1 = pathfind_bfs(width, height, map, start_coord, end_coord, blizz_count, blizzards, 0);
    printf("Part 1: %ld minutes\n", minute_p1);

    /******************** Part 2 ********************/

    // Current minute on the path (we start where we left from the Part 1)
    int64_t minute_p2 = minute_p1;

    // Path from the destination back to the start
    minute_p2 = pathfind_bfs(width, height, map, end_coord, start_coord, blizz_count, blizzards, minute_p2);

    // Path from the start to back to the destination
    minute_p2 = pathfind_bfs(width, height, map, start_coord, end_coord, blizz_count, blizzards, minute_p2);

    printf("Part 2: %ld minutes\n", minute_p2);

    return 0;
}