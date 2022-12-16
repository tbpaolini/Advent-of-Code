#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

typedef struct CaveCoordinate {
    int64_t x;
    int64_t y;
} CaveCoordinate;

typedef struct WallCoordinates
{
    struct CaveCoordinate  start;
    struct CaveCoordinate  end;
    struct WallCoordinates *next;
} WallCoordinates;

// Create new wall coordinates
// Note: should be freed with 'coord_destroy()'
static WallCoordinates* coord_new()
{
    WallCoordinates *coord = (WallCoordinates*)calloc(1, sizeof(WallCoordinates));
    if (!coord)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return coord;
}

// The indices on the map array where the (x = 0; y = 0) are
static CaveCoordinate array_origin;

// Convert (x, y) corrdinates from the map space to the array space
static inline CaveCoordinate coord_convert(CaveCoordinate map_coordinate)
{
    return (CaveCoordinate){
        .x = map_coordinate.x + array_origin.x,
        .y = map_coordinate.y + array_origin.y
    };
}

// Maximum coordinates of two points
static inline CaveCoordinate max_coord(CaveCoordinate coord_1, CaveCoordinate coord_2)
{
    return (CaveCoordinate){
        .x = (coord_1.x > coord_2.x) ? coord_1.x : coord_2.x,
        .y = (coord_1.y > coord_2.y) ? coord_1.y : coord_2.y
    };
}

// Minimum coordinates of two points
static inline CaveCoordinate min_coord(CaveCoordinate coord_1, CaveCoordinate coord_2)
{
    return (CaveCoordinate){
        .x = (coord_1.x < coord_2.x) ? coord_1.x : coord_2.x,
        .y = (coord_1.y < coord_2.y) ? coord_1.y : coord_2.y
    };
}

// Free the memory of all elements on a list of coordinates
static void coord_destroy(WallCoordinates *list_head)
{
    WallCoordinates *coord = list_head;
    while (coord)
    {
        WallCoordinates *next_coord = coord->next;
        free(coord);
        coord = next_coord;
    }
}

// The contents of the spaces on the map
enum {
    EMPTY  = 0,
    WALL   = 1,
    SAND   = 2,
    SOURCE = 3
};

// Debugging function
static void map_print(int64_t width, int64_t height, uint8_t map_array[height][width])
{
    // Print the map when debugging
    CaveCoordinate sand_source = {0,0};
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            char text;
            switch (map_array[y][x])
            {
                case EMPTY:
                    text = '.';
                    break;
                
                case WALL:
                    text = '#';
                    break;
                
                case SAND:
                    text = 'o';
                    break;
                
                case SOURCE:
                    text = '+';
                    break;
            }
            putchar(text);
        }
        putchar('\n');
    }
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[512];

    // Maximum absolute coordinates
    CaveCoordinate max = {INT64_MIN, INT64_MIN};
    CaveCoordinate min = {INT64_MAX, INT64_MAX};

    // List of the wall's coordinates
    WallCoordinates *coord = NULL;      // Current coordinate
    WallCoordinates *coord_head = NULL; // Head of the coordinate list
    
    // Parse the wall coordinates from the input
    while (fgets(line, sizeof(line), input))
    {
        if (coord)
        {
            // Second coordinate and beyond
            coord->next = coord_new();
            coord = coord->next;
        }
        else
        {
            // First coordinate being parsed
            coord = coord_new();
            coord_head = coord;
        }
        
        // Split the line on the delimiters
        const char delimiters[] = ", ->";
        char *token = strtok(line, delimiters);
        
        // Start and end coordinates of the wall
        int64_t x_start, y_start;
        int64_t x_end, y_end;

        // Parse the start coordinates
        x_start = atol(token);
        token = strtok(NULL, delimiters);
        y_start = atol(token);
        token = strtok(NULL, delimiters);

        // Keep track of the maximum and minimum coordinates
        max = max_coord(max, (CaveCoordinate){x_start, y_start});
        min = min_coord(min, (CaveCoordinate){x_start, y_start});
        
        // Parse the remaining walls
        while (token)
        {
            // End point of the wall
            x_end = atol(token);
            token = strtok(NULL, delimiters);
            y_end = atol(token);

            coord->start = (CaveCoordinate){x_start, y_start};
            coord->end = (CaveCoordinate){x_end, y_end};

            // Keep track of the maximum and minimum coordinates
            max = max_coord(max, coord->end);
            min = min_coord(min, coord->end);

            // The end point of this wall is the start of the next wall
            x_start = x_end;
            y_start = y_end;
            
            // Next wall
            token = strtok(NULL, delimiters);
            
            if (token)
            {
                // There are still more coordinates to be parsed
                coord->next = coord_new();
                coord = coord->next;
            }
            else
            {
                // We got to the end of the input's line
                break;
            }
        }
    }

    fclose(input);

    // Ensure that we have room on the map for the sand's source
    if (max.x < 500) max.x = 500;
    if (min.y > 0) min.y = 0;
    
    // Calculate the boundaries of the map
    int64_t width  = max.x - min.x + 1 + 4 + 2 * max.y;
    int64_t height = max.y - min.y + 1 + 2;
    uint8_t map[height][width];
    memset(map, 0, sizeof(map));
    /* Note:
        The map's dimensions were calculated in order to ensure there is enough room for
        the simulation of both parts of the puzzle:
        
        A border of 2 spaces was added to the right, left, and bottom.
        
        The width was expanded to both sides by the value of the height. Because when the
        sand fills the map to the top, it forms a equilateral triangle. Each layer of the
        triangle has two more units than the previous, thus at the base it extends from
        the center to both sides by the value of the height.
    */

    // Offset for converting the absolute coordinates to coordinates on the map array
    array_origin = (CaveCoordinate){0 - min.x + 2 + max.y, 0 - min.y};

    // Draw the bottom  floor to the map
    for (int64_t x = 0; x < width; x++)
    {
        map[max.y+2][x] = WALL;
    }
    
    // Draw the walls to the map
    WallCoordinates *current_wall = coord_head;
    while (current_wall)
    {
        // The vertexes of the wall
        CaveCoordinate wall_start = coord_convert(current_wall->start); 
        CaveCoordinate wall_end = coord_convert(current_wall->end);

        // The direction in which the wall is draw
        const int64_t dir_x = (wall_end.x >= wall_start.x) ? 1 : -1;
        const int64_t dir_y = (wall_end.y >= wall_start.y) ? 1 : -1;

        // Draw the wall from the start point to the end point
        for (int64_t y = wall_start.y; y * dir_y <= wall_end.y * dir_y; y += dir_y)
        {
            for (int64_t x = wall_start.x; x * dir_x <= wall_end.x * dir_x; x += dir_x)
            {
                assert(y < height && x < width && y >= 0 && x >= 0);
                map[y][x] = WALL;
            }
        }
        
        // Move to the next wall
        current_wall = current_wall->next;
    }

    // Starting state of the map
    CaveCoordinate sand_source = coord_convert((CaveCoordinate){500, 0});   // Where the sand starts from
    CaveCoordinate sand_current = sand_source;  // Current position of the sand
    uint64_t sand_count = 0;                    // Amount of sand units so far

    // Add the sand's source
    map[sand_source.y][sand_source.x] = SOURCE;

    uint64_t solution_p1 = 0;   // Part 1
    uint64_t solution_p2 = 0;   // Part 2
    bool p1_solved = false;     // Whether Part 1 was finished

    while (true)
    {
        // Check the contents of the 3 adjascent spaces below the sand
        const int64_t x = sand_current.x;
        const int64_t y = sand_current.y;
        const uint8_t below = map[y+1][x];
        const uint8_t bottom_left = map[y+1][x-1];
        const uint8_t bottom_right = map[y+1][x+1];
        
        if (!below)
        {
            // Nothing straight bellow: move down
            sand_current.y += 1;
        }
        else if (!bottom_left)
        {
            // Nothing to the bottom left: move diagonally to the left
            sand_current.x -= 1;
            sand_current.y += 1;
        }
        else if (!bottom_right)
        {
            // Nothing to the bottom right: move diagonally to the right
            sand_current.x += 1;
            sand_current.y += 1;
        }
        else
        {
            // No free space: stop
            map[y][x] = SAND;
            sand_count += 1;
            if (sand_current.y == sand_source.y && sand_current.x == sand_source.x)
            {
                // Part 2: The sand covered the source
                solution_p2 = sand_count;
                break;
            }
            sand_current = sand_source; // Next sand unit
        }

        // Part 1: The sand went to the "abyss"
        if (!p1_solved && sand_current.y > max.y)
        {
            solution_p1 = sand_count;
            p1_solved = true;
        }
        
    }

    printf("Part 1: %5lu units\n", solution_p1);
    printf("Part 2: %5lu units\n", solution_p2);

    coord_destroy(coord_head);
       
    return 0;
}