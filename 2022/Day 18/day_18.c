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

// Perform a Depth First Search to count how many faces of one cube are exposed to the outside
static int64_t surface_search(
    LavaCoord cube,         // Coordinates of the cube
    LavaCoord max_coord,    // Maximum coordinates of the search area
    bool lava[max_coord.z+1][max_coord.y+1][max_coord.x+1]  // Grid of cubes
)
{
    // If we are outside the grid, consider the current coordinate as exposed to the surface
    if (
        (cube.x < 0 || cube.x > max_coord.x) ||
        (cube.y < 0 || cube.y > max_coord.y) ||
        (cube.z < 0 || cube.z > max_coord.z)
    )
    {
        return 1;
    }
    
    // Amount of faces exposed to the outside
    int64_t surface_area = 0;
    
    // Spaces next to each of the six faces
    const LavaCoord face[6] = {
        {cube.x+1, cube.y, cube.z},   // Left
        {cube.x-1, cube.y, cube.z},   // Right
        {cube.x, cube.y+1, cube.z},   // Bottom
        {cube.x, cube.y-1, cube.z},   // Top
        {cube.x, cube.y, cube.z+1},   // Back
        {cube.x, cube.y, cube.z-1},   // Front
    };
    
    // Loop through the six faces
    for (size_t i = 0; i < 6; i++)
    {
        // Current coordinates
        const int64_t x = face[i].x;
        const int64_t y = face[i].y;
        const int64_t z = face[i].z;
        
        // If the current space is empty
        if (
            x < 0 || y < 0 || z < 0 ||
            x > max_coord.x || y > max_coord.y || z > max_coord.z ||
            !lava[z][y][x]
        )
        {
            // Recursively search from the current space
            const int64_t this_face = surface_search(
                (LavaCoord){x, y, z},
                max_coord,
                lava
            );

            // If this space is on the outside
            if (this_face)
            {
                // Count the exposed space
                surface_area += this_face;
                
                // Once a path to the outside has been found, stop searching if
                // we started from an empty space.
                // This prevents counting the same exposed face more than once,
                // while still searching through every face of a filled space.
                if (!lava[cube.z][cube.y][cube.x]) break;
            }
        }
    }
    
    return surface_area;
}


int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("test.txt", "rt");
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
    int64_t faces_count_p1 = 0;
    for (int64_t z = 0; z <= max_coord.z; z++)
    {
        for (int64_t y = 0; y <= max_coord.y; y++)
        {
            for (int64_t x = 0; x <= max_coord.x; x++)
            {
                // Check if there is a droplet here
                if (!lava[z][y][x]) continue;
                
                // Top face
                if (y-1 < 0 || !lava[z][y-1][x]) faces_count_p1++;

                // Bottom face
                if (y+1 > max_coord.y || !lava[z][y+1][x]) faces_count_p1++;

                // Right face
                if (x-1 < 0 || !lava[z][y][x-1]) faces_count_p1++;

                // Left face
                if (x+1 > max_coord.x || !lava[z][y][x+1]) faces_count_p1++;

                // Back face
                if (z-1 < 0 || !lava[z-1][y][x]) faces_count_p1++;

                // Front face
                if (z+1 > max_coord.z || !lava[z+1][y][x]) faces_count_p1++;
            }
        }
    }

    printf("%ld\n", faces_count_p1);

    int64_t faces_count_p2 = 0;
    for (int64_t z = 0; z <= max_coord.z; z++)
    {
        for (int64_t y = 0; y <= max_coord.y; y++)
        {
            for (int64_t x = 0; x <= max_coord.x; x++)
            {
                // Check if there is a droplet here
                if (!lava[z][y][x]) continue;
                
                faces_count_p2 += surface_search(
                    (LavaCoord){x, y, z},
                    max_coord,
                    lava
                );
            }
        }
    }

    printf("%ld\n", faces_count_p2);

    return 0;
}