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

static inline CaveCoordinate max_coord(CaveCoordinate coord_1, CaveCoordinate coord_2)
{
    return (CaveCoordinate){
        .x = (coord_1.x > coord_2.x) ? coord_1.x : coord_2.x,
        .y = (coord_1.y > coord_2.y) ? coord_1.y : coord_2.y
    };
}

static inline CaveCoordinate min_coord(CaveCoordinate coord_1, CaveCoordinate coord_2)
{
    return (CaveCoordinate){
        .x = (coord_1.x < coord_2.x) ? coord_1.x : coord_2.x,
        .y = (coord_1.y < coord_2.y) ? coord_1.y : coord_2.y
    };
}

enum {
    EMPTY  = 0,
    WALL   = 1,
    SAND   = 2,
    SOURCE = 3
};

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

    CaveCoordinate max = {INT64_MIN, INT64_MIN};
    CaveCoordinate min = {INT64_MAX, INT64_MAX};

    WallCoordinates *coord = NULL;
    WallCoordinates *coord_head = NULL;
    
    while (fgets(line, sizeof(line), input))
    {
        if (coord)
        {
            coord->next = coord_new();
            coord = coord->next;
        }
        else
        {
            coord = coord_new();
            coord_head = coord;
        }
        
        const char delimiters[] = ", ->";
        char *token = strtok(line, delimiters);
        
        int64_t x_start, y_start;
        int64_t x_end, y_end;

        x_start = atol(token);
        token = strtok(NULL, delimiters);
        y_start = atol(token);
        token = strtok(NULL, delimiters);

        max = max_coord(max, (CaveCoordinate){x_start, y_start});
        min = min_coord(min, (CaveCoordinate){x_start, y_start});
        
        while (token)
        {
            x_end = atol(token);
            token = strtok(NULL, delimiters);
            y_end = atol(token);

            coord->start = (CaveCoordinate){x_start, y_start};
            coord->end = (CaveCoordinate){x_end, y_end};

            max = max_coord(max, coord->end);
            min = min_coord(min, coord->end);

            x_start = x_end;
            y_start = y_end;
            
            token = strtok(NULL, delimiters);
            
            if (token)
            {
                coord->next = coord_new();
                coord = coord->next;
            }
            else
            {
                break;
            }
        }
    }

    fclose(input);

    WallCoordinates *current_wall = coord_head;

    if (max.x < 500) max.x = 500;
    if (min.y > 0) min.y = 0;
    
    int64_t width  = max.x - min.x + 1 + 4;
    int64_t height = max.y - min.y + 1 + 2;
    uint8_t map[height][width];
    memset(map, 0, sizeof(map));

    array_origin = (CaveCoordinate){0 - min.x + 2, 0 - min.y};
    current_wall = coord_head;

    while (current_wall)
    {
        CaveCoordinate wall_start = coord_convert(current_wall->start); 
        CaveCoordinate wall_end = coord_convert(current_wall->end);

        const int64_t dir_x = (wall_end.x >= wall_start.x) ? 1 : -1;
        const int64_t dir_y = (wall_end.y >= wall_start.y) ? 1 : -1;

        for (int64_t y = wall_start.y; y * dir_y <= wall_end.y * dir_y; y += dir_y)
        {
            for (int64_t x = wall_start.x; x * dir_x <= wall_end.x * dir_x; x += dir_x)
            {
                assert(y < height && x < width && y >= 0 && x >= 0);
                map[y][x] = WALL;
            }
        }
        
        current_wall = current_wall->next;
    }

    CaveCoordinate sand_source = coord_convert((CaveCoordinate){500, 0});   // Where the sand starts from
    CaveCoordinate sand_current = sand_source;  // Current position of the sand
    CaveCoordinate sand_previous = sand_source; // Previous position of the sand
    uint64_t sand_count = 0;                    // Amount of sand units so far

    while (true)
    {
        const int64_t x = sand_current.x;
        const int64_t y = sand_current.y;
        const uint8_t below = map[y+1][x];
        const uint8_t bottom_left = map[y+1][x-1];
        const uint8_t bottom_right = map[y+1][x+1];
        
        if (!below)
        {
            sand_previous = sand_current;
            sand_current.y += 1;
        }
        else if (!bottom_left)
        {
            sand_previous = sand_current;
            sand_current.x -= 1;
            sand_current.y += 1;
        }
        else if (!bottom_right)
        {
            sand_previous = sand_current;
            sand_current.x += 1;
            sand_current.y += 1;
        }
        else
        {
            map[y][x] = SAND;
            sand_count += 1;
            sand_current = sand_source;
        }

        if (sand_current.y > max.y) break;
    }

    printf("%lu\n", sand_count);
       
    return 0;
}