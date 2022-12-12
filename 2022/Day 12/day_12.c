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
    size_t x;
    size_t y;
} MountainCoord;

typedef struct MountainNode
{
    struct MountainCoord coord;     // (x, y) coordinates of this node
    uint64_t elevation;             // The cost to get here from the previous node
    uint64_t total_cost;            // The total cost to get here from the start
    struct MountainNode *exits[4];  // The nodes this one connects to: {right, top, left, down} (NULL means no exit on a direction)
    bool visited;                   // Whether this node has been visited already
} MountainNode;

typedef struct MountainMap
{
    struct MountainNode **map;      // A 2-D array that represents the terrain map: map[row][column]
    struct MountainCoord max;       // Maximum indices on tha map array (y = row; x = column)
    struct MountainCoord *start;    // Staring node
    struct MountainCoord *end;      // Destination node
    uint64_t steps;                 // How many steps the path has
    uint64_t total_cost;            // The total cost of the path
    struct
    {
        struct MountainNode *current;   // Current node on the path
        struct MountainNode *next;      // Next node on the path
    } path; // Path from the start to the end
} MountainMap;

static MountainMap* create_empty_map(size_t width, size_t height)
{
    // Calculate the memory size to store the mountain map
    // In C, a 2-D array begins with a sequence of pointers to the start of each row,
    // followed by rows themselves one after the other.
    size_t map_size = (sizeof(MountainNode*) * height) + (sizeof(MountainNode) * width);
    if (map_size > 100000000)
    {
        fprintf(stderr, "Error: Maximum map size is limited to 100 MB\n");
        abort();
    }
    
    // Allocate a memory region for the map (initialized to 0)
    MountainNode **map = (MountainNode**)calloc(1, map_size);
    if (!map)
    {
        fprintf(stderr, "Error: No enough memory.\n");
        abort();
    }

    // Memory address where the first row begins
    MountainNode *node = (MountainNode*)((ptrdiff_t)map + (sizeof(MountainNode*) * height));

    // Add the pointers to the rows
    for (size_t y = 0; y < height; y++)
    {
        // The address is incremented by the size of a single node
        map[y] = node++;
    }

    // Allocate memory for the path struct
    MountainMap *path = (MountainMap*)calloc(1, sizeof(MountainMap));
    
    // Add the map to the path struct
    path->map = map;

    return path;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
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

    MountainMap *mountain_p1 = create_empty_map(columns, rows);

    for (size_t y = 0; y < rows; y++)
    {
        for (size_t x = 0; x < columns; x++)
        {
            mountain_p1->map[y][x].coord = (MountainCoord){x, y};
            uint64_t elevation = fgetc(input);

            if (islower(elevation))
            {
                
            }
        }
    }
    
    return 0;
}