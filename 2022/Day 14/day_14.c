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
WallCoordinates* coord_new()
{
    WallCoordinates *coord = (WallCoordinates*)calloc(1, sizeof(WallCoordinates));
    if (!coord)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return coord;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[512];

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
        
        while (token)
        {
            x_end = atol(token);
            token = strtok(NULL, delimiters);
            y_end = atol(token);

            coord->start = (CaveCoordinate){x_start, y_start};
            coord->end = (CaveCoordinate){x_end, y_end};

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
    CaveCoordinate max = {current_wall->start.x, current_wall->start.y};
    CaveCoordinate min = {current_wall->start.x, current_wall->start.y};

    while (current_wall)
    {
        CaveCoordinate current = current_wall->end;
        if (current.x > max.x) max.x = current.x;
        if (current.y > max.y) max.y = current.y;
        if (current.x < min.x) min.x = current.x;
        if (current.y < min.y) min.y = current.y;
        current_wall = current_wall->next;
    }

    return 0;
}