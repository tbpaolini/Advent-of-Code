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
    EMPTY = 0,
    WALL  = 1,
    SAND  = 2
};

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

        for (int64_t y = wall_start.y; y <= wall_end.y; y++)
        {
            for (int64_t x = wall_start.x; x <= wall_end.x; x++)
            {
                assert(y < height && x < width && y >= 0 && x >= 0);
                map[y][x] = WALL;
            }
        }
        
        current_wall = current_wall->next;
    }

    uint64_t sand_count = 0;
    CaveCoordinate sand_origin = coord_convert((CaveCoordinate){500, 0});

    #ifdef _DEBUG
        // Print the map when debugging
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                char text = map[y][x] ? '#' : '.';
                if (x == sand_origin.x && y == sand_origin.y) text = '+';
                putchar(text);
            }
            putchar('\n');
        }
    #endif

    return 0;
}