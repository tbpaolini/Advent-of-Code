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

// Perform a Depth First Search to check if there is a path to the water outside the area
static int64_t pathfind_dfs(
    LavaCoord coord,        // Coordinates of the starting point
    LavaCoord max_coord,    // Maximum coordinates of the search area
    bool lava[max_coord.z+1][max_coord.y+1][max_coord.x+1],     // Grid of cubes
    bool visited[max_coord.z+1][max_coord.y+1][max_coord.x+1]   // Visited spaces on the grid (initialized to zero)
)
{
    if (visited[coord.z][coord.y][coord.x]) return 0;
    
    // If we are outside the grid, consider the current coordinate as exposed to the surface
    if (
        (coord.x < 0 || coord.x > max_coord.x) ||
        (coord.y < 0 || coord.y > max_coord.y) ||
        (coord.z < 0 || coord.z > max_coord.z)
    )
    {
        return 1;
    }

    // Mark this space as 'visited'
    visited[coord.z][coord.y][coord.x] = true;
    
    // Amount of faces exposed to the outside
    int64_t surface_area = 0;
    
    // Spaces next to each of the six exits
    const LavaCoord exit[6] = {
        {coord.x+1, coord.y, coord.z},   // Left
        {coord.x-1, coord.y, coord.z},   // Right
        {coord.x, coord.y+1, coord.z},   // Bottom
        {coord.x, coord.y-1, coord.z},   // Top
        {coord.x, coord.y, coord.z+1},   // Back
        {coord.x, coord.y, coord.z-1},   // Front
    };
    
    // Loop through the six exits
    for (size_t i = 0; i < 6; i++)
    {
        // Current coordinates
        const int64_t x = exit[i].x;
        const int64_t y = exit[i].y;
        const int64_t z = exit[i].z;
        
        // If the current space is empty and not yet visited
        if (
            !visited[z][y][x] ||
            x < 0 || y < 0 || z < 0 ||
            x > max_coord.x || y > max_coord.y || z > max_coord.z ||
            !lava[z][y][x]
        )
        {
            // Recursively search from the current space
            const int64_t this_exit = pathfind_dfs(
                (LavaCoord){x, y, z},
                max_coord,
                lava,
                visited
            );

            // If this exit has a path to outside
            if (this_exit) return 1;
        }
    }
    
    return 0;
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

    // 3-D array for keeping track of which spaces were already visited during pathfinding
    bool visited[max_coord.z+1][max_coord.y+1][max_coord.x+1];
    
    // Check which faces are exposed
    int64_t faces_count_p1 = 0;
    int64_t faces_count_p2 = 0;
    for (int64_t z = 0; z <= max_coord.z; z++)
    {
        for (int64_t y = 0; y <= max_coord.y; y++)
        {
            for (int64_t x = 0; x <= max_coord.x; x++)
            {
                // Axis orientation:
                //   -y
                //    |
                //    |___-x
                //    /
                //   /
                // -z
                
                // Check if there is a droplet here
                if (!lava[z][y][x]) continue;
                
                // The six faces of the cube
                const LavaCoord face[6] = {
                    {x+1, y, z},   // Left
                    {x-1, y, z},   // Right
                    {x, y+1, z},   // Bottom
                    {x, y-1, z},   // Top
                    {x, y, z+1},   // Back
                    {x, y, z-1},   // Front
                };

                for (size_t i = 0; i < 6; i++)
                {
                    if (
                        face[i].x < 0 || face[i].y < 0 || face[i].z < 0 ||
                        face[i].x > max_coord.x ||face[i].y > max_coord.y ||face[i].z > max_coord.z ||
                        !lava[face[i].z][face[i].y][face[i].x]
                    )
                    {
                        faces_count_p1++;
                        
                        memset(visited, 0, sizeof(visited));
                        faces_count_p2 += pathfind_dfs(face[i], max_coord, lava, visited);
                    }
                }
                
            }
        }
    }

    printf("%ld\n", faces_count_p1);
    printf("%ld\n", faces_count_p2);

    return 0;
}