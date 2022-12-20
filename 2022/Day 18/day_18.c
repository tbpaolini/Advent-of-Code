#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct LavaCoord
{
    int64_t x;
    int64_t y;
    int64_t z;
} LavaCoord;

typedef struct LavaList
{
    struct LavaCoord coord;
    struct LavaList  *next;
} LavaList;

// Create a new node for a linked list
static LavaList* list_new()
{
    LavaList *list = (LavaList*)calloc(1, sizeof(LavaList));
    if (!list)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return list;
}

// Given the first element of a linked list, free the memory of all elemets
static void list_destroy(LavaList *list_head)
{
    LavaList *node = list_head;
    while (node)
    {
        LavaList *next_node = node->next;
        free(node);
        node = next_node;
    }
}


int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char line[16];

    // List of coordinates
    LavaList *coord_list = NULL;
    LavaList *last_coord = NULL;
    LavaCoord max_coord = {0, 0, 0};

    while (fgets(line, sizeof(line), input))
    {
        // Create a new entry on the list
        LavaList *coord = list_new();
        if (!coord_list) coord_list = coord;        // Set the list's head if this is the first entry
        if (last_coord) last_coord->next = coord;   // Link the previous entry to the current one
        last_coord = coord;                         // Move the list's tail to the current entry

        // Split the line on the comma to get the (x,y,z) coordinates
        const char delimiter[] = ",";
        
        char *token = strtok(line, delimiter);
        assert(isdigit(token[0]));
        coord->coord.x = atol(token);
        
        token = strtok(NULL, delimiter);
        assert(isdigit(token[0]));
        coord->coord.y = atol(token);
        
        token = strtok(NULL, delimiter);
        assert(isdigit(token[0]));
        coord->coord.z = atol(token);

        // Check the maximum coordinate on all axes
        if (coord->coord.x > max_coord.x) max_coord.x = coord->coord.x;
        if (coord->coord.y > max_coord.y) max_coord.y = coord->coord.y;
        if (coord->coord.z > max_coord.z) max_coord.z = coord->coord.z;
    }
    
    fclose(input);

    // 3-D array to represent the coordinates that have a lava droplet
    bool lava[max_coord.z+1][max_coord.y+1][max_coord.x+1];
    memset(lava, 0, sizeof(lava));

    // Flag the coordinates that have a droplet
    LavaList *droplet = coord_list;
    while (droplet)
    {
        lava[droplet->coord.z][droplet->coord.y][droplet->coord.x] = true;
        droplet = droplet->next;
    }
    list_destroy(coord_list);
    
    // Axis orientation:
    //   -y
    //    |
    //    |___-x
    //    /
    //   /
    // -z
    
    // Check which faces are exposed
    int64_t faces_count = 0;
    for (int64_t z = 0; z <= max_coord.z; z++)
    {
        for (int64_t y = 0; y <= max_coord.y; y++)
        {
            for (int64_t x = 0; x <= max_coord.x; x++)
            {
                // Check if there is a droplet here
                if (!lava[z][y][x]) continue;
                
                // Top face
                if (y-1 < 0 || !lava[z][y-1][x]) faces_count++;

                // Bottom face
                if (y+1 > max_coord.y || !lava[z][y+1][x]) faces_count++;

                // Right face
                if (x-1 < 0 || !lava[z][y][x-1]) faces_count++;

                // Left face
                if (x+1 > max_coord.x || !lava[z][y][x+1]) faces_count++;

                // Back face
                if (z-1 < 0 || !lava[z-1][y][x]) faces_count++;

                // Front face
                if (z+1 > max_coord.z || !lava[z+1][y][x]) faces_count++;
            }
        }
    }

    printf("%ld\n", faces_count);

    return 0;
}