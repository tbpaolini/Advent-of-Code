#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

typedef struct MountainCoord
{
    int64_t x;
    int64_t y;
} MountainCoord;

typedef struct MountainNode
{
    struct MountainCoord coord;     // (x, y) coordinates of this node
    int64_t elevation;              // The cost to get here from the previous node
    int64_t total_cost;             // The total cost to get here from the start
    struct MountainNode *exits[4];  // The nodes this one connects to: {right, top, left, down} (NULL means no exit on a direction)
    bool visited;                   // Whether this node has been visited already
} MountainNode;

typedef struct MountainPath
{
    struct MountainNode *node;      // Current node on the path
    struct MountainPath *next;      // Next node on the path
    struct MountainPath *previous;  // Previous node on the path
} MountainPath;

typedef struct MountainMap
{
    struct MountainNode **map;      // A 2-D array that represents the terrain map: map[row][column]
    struct MountainCoord max;       // Maximum indices on tha map array (y = row; x = column)
    struct MountainNode *start;     // Staring node
    struct MountainNode *end;       // Destination node
    int64_t steps;                  // How many steps the path has
    int64_t total_cost;             // The total cost of the path
    MountainPath *path;             // Path from the start to the end
} MountainMap;

// Allocate memory on the heap for a 2-dimensional array
// Note: the returned array should be freed with 'free()'
static void** allocate_2Darray(size_t width, size_t height, size_t element_size)
{
    // Calculate the memory size to store the array
    // In C, a 2-D array begins with a sequence of pointers to the start of each row,
    // followed by rows themselves one after the other.
    size_t array_size = (sizeof(void*) * height) + (element_size * width * height);
    if (array_size > 100000000)
    {
        fprintf(stderr, "Error: Maximum array size is limited to 100 MB\n");
        abort();
    }
    
    // Allocate a memory region for the array (initialized to 0)
    void **array = (void**)calloc(1, array_size);
    if (!array)
    {
        fprintf(stderr, "Error: No enough memory.\n");
        abort();
    }

    // Memory address where the first row begins
    ptrdiff_t row = (ptrdiff_t)array + (sizeof(void*) * height);

    // Add the pointers to the rows
    for (size_t y = 0; y < height; y++)
    {
        // Store the row's pointer
        array[y] = (void*)row;

        // Move to the next row
        row += width * element_size;
    }

    return array;
}

// Create an empty MountainMap struct
static MountainMap* map_create_empty(size_t width, size_t height)
{
    // Allocate a memory region for the map (initialized to 0)
    MountainNode **map = (MountainNode**)allocate_2Darray(width, height, sizeof(MountainNode));

    // Allocate memory for the mountain struct
    MountainMap *mountain = (MountainMap*)calloc(1, sizeof(MountainMap));
    if (!mountain)
    {
        fprintf(stderr, "Error: No enough memory.\n");
        abort();
    }

    // Add the map to the mountain struct
    mountain->max = (MountainCoord){width - 1, height - 1}; // Map's maximum coordinates
    mountain->map = map;

    return mountain;
}

// Populate the MountainMap struct with the elevation values and the start/end positions.
// Also links the map nodes to their respective exits.
static void map_populate(MountainMap *empty_map, char **elevations)
{
    int64_t columns = empty_map->max.x + 1; // Width
    int64_t rows    = empty_map->max.y + 1; // Height
    
    // Loop through each coordinate
    for (int64_t y = 0; y < rows; y++)
    {
        for (int64_t x = 0; x < columns; x++)
        {
            // Get the current node
            MountainNode *node = &empty_map->map[y][x];
            node->coord = (MountainCoord){x, y};        // Store the coordinates
            node->total_cost = INT64_MAX;               // Cost to get here from the start (defaults to 'infinity')
            char elevation = elevations[y][x];          // Get the elevation

            // Store the elevation
            if (islower(elevation))
            {
                // We are at an intermediate node
                node->elevation = elevation - 'a';
            }
            else if (elevation == 'S')
            {
                // We are at the starting node
                node->elevation = 0;
                node->total_cost = 0;
                empty_map->start = node;
            }
            else if (elevation == 'E')
            {
                // We are at the destination node
                node->elevation = 25;
                empty_map->end = node;
            }
            else
            {
                // Invalid node
                fprintf(stderr, "Error: Invalid elevation of '%c'\n", elevation);
                abort();
            }

            // The coordinates of the exits of this node
            const MountainCoord my_exits[4] = {
                {x+1, y},   // Right
                {x, y-1},   // Up
                {x-1, y},   // Left
                {x, y+1}    // Down
            };
            
            // Store the exits of the current node
            for (size_t i = 0; i < 4; i++)
            {
                // Get the exit's coordinates
                const int64_t exit_x = my_exits[i].x;
                const int64_t exit_y = my_exits[i].y;

                // Skip if the exit is out of bounds
                if (exit_x < 0 || exit_y < 0 || exit_x >= columns || exit_y >= rows)
                {
                    continue;
                }
                
                // Pointer to the exit node
                MountainNode *exit = &empty_map->map[exit_y][exit_x];

                // Skip if the exit's elevation is too high
                if (exit->elevation - node->elevation > 1)
                {
                    continue;
                }

                // Link the node to the exit
                node->exits[i] = exit;
            }
        }
    }
}

static MountainPath* new_path_node()
{
    MountainPath *path = calloc(1, sizeof(MountainPath));
    if (!path)
    {
        fprintf(stderr, "Error: No enough memory.\n");
        abort();
    }
    return path;
}

static void pathfind_dijkstra(MountainMap *mountain)
{
    // Starting position
    MountainNode *node = mountain->start;
    MountainPath *path = new_path_node();
    int64_t cost = 0;
    
    // Keep track of current position on the path
    MountainPath *position = path;
    position->node = node;

    // Move through the mountain until the destination node is reached
    while (node != mountain->end)
    {
        // Mark the node as 'visited'
        node->visited = true;
        
        cost++;                         // Moving to a node increases the cost by one
        int64_t min_cost = INT64_MAX;   // Smallest cost among all node's exits
        MountainNode *next_node = NULL; // Exit with the smallest cost
        
        // Check the exits and update their costs
        for (size_t i = 0; i < 4; i++)
        {
            MountainNode *exit = node->exits[i];
            if (exit)
            {
                // Skip the exit if it has already been visited
                if (exit->visited) continue;
                
                // Update the minimum cost to the exit
                if (exit->total_cost > cost)
                {
                    exit->total_cost = cost;
                }
                
                // Check if the exit has the minimum cost so far
                if (cost < min_cost)
                {
                    min_cost = cost;
                    next_node = exit;
                }
            }
        }

        // Check if there is any unvisited exit
        if (next_node)
        {
            // There is an exit: move to it
            
            // Add the next position on the path
            MountainPath *next_position = new_path_node();
            next_position->node = next_node;
            next_position->previous = position;
            position->next = next_position;
            position = next_position;
            
            // Move to the next node
            node = next_node;
        }
        else
        {
            // There is not an exit: move to the previous position
            cost--;
            
            // Remove the current step from the node
            MountainPath *old_position = position;
            position = position->previous;
            position->next = NULL;
            free(old_position);

            // Move to the previous node
            node = position->node;
        }
    }

    mountain->total_cost = cost;
    mountain->path = path;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("test.txt", "rt");
    char line[128];

    size_t rows = 0;
    size_t columns = 0;

    while (fgets(line, sizeof(line), input))
    {
        rows++;
        const size_t column_size = strlen(line) - 1;
        if (columns == 0)
        {
            columns = column_size;
        }
        else if (columns != column_size)
        {
            fprintf(stderr, "Error: Malformatted input file\n");
            abort();
        }
    }

    rewind(input);

    // Store the values of the input file
    char **raw_map = (char**)allocate_2Darray(columns, rows, sizeof(char));

    for (size_t y = 0; y < rows; y++)   // Line by line
    {
        for (size_t x = 0; x < columns; x++)    // Char by char on the line
        {
            raw_map[y][x] = fgetc(input);
        }
        int newline = fgetc(input); // Skip the newline character
        assert(newline == '\n');
    }

    fclose(input);

    MountainMap *mountain_p1 = map_create_empty(columns, rows);
    map_populate(mountain_p1, (char**)raw_map);
    pathfind_dijkstra(mountain_p1);
    
    return 0;
}