#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct MountainNode
{
    struct MountainCoord coord;     // (x, y) coordinates of this node
    uint64_t elevation;             // The cost to get here from the previous node
    uint64_t total_cost;            // The total cost to get here from the start
    struct MountainNode *exits[4];  // The nodes this one connects to: {right, top, left, down} (NULL means no exit on a direction)
    bool visited;                   // Whether this node has been visited already
} MountainNode;

typedef struct MountainPath
{
    struct MountainCoord *start;    // Staring node
    struct MountainCoord *end;      // Destination node
    uint64_t steps;                 // How many steps the path has
    uint64_t total_cost;            // The total cost of the path
    struct
    {
        struct MountainNode *current;   // Current node on the path
        struct MountainNode *next;      // Next node on the path
    } path; // Path from the start to the end
};

typedef struct MountainCoord
{
    size_t x;
    size_t y;
} MountainCoord;


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

    for (size_t i = 0; i < rows; i++)
    {
        /* code */
    }
    
    
    return 0;
}