#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// (x, y) coordinates
typedef struct Coord2_i64 {
    int64_t x;
    int64_t y;
} Coord2_i64;

static void parse_input(char *line, Coord2_i64 *output)
{
    const char *direction = &line[0];
    const int64_t distance = atol(&line[2]);
    
    switch (direction[0])
    {
        case 'R':
            output->x = distance;
            output->y = 0;
            break;
        
        case 'U':
            output->x = 0;
            output->y = distance;
            break;
        
        case 'L':
            output->x = -1 * distance;
            output->y = 0;
            break;
        
        case 'D':
            output->x = 0;
            output->y = -1 * distance;
            break;
        
        default:
            fprintf(stderr, "Error: Malformatted input\n");
            abort();
            break;
    }
}

void coord_add(Coord2_i64 *coord_a, Coord2_i64 *coord_b)
{
    coord_a->x += coord_b->x;
    coord_a->y += coord_b->y;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[8];

    Coord2_i64 pos   = {0, 0};                  // Current position
    Coord2_i64 max   = {INT64_MIN, INT64_MIN};  // Maximum position on each axis
    Coord2_i64 min   = {INT64_MAX, INT64_MAX};  // Minumum position on each axis
    Coord2_i64 delta = {0, 0};                  // Distance traveled on each axis

    // Count the amount of movements
    size_t move_count = 0;
    while (fgets(line, sizeof(line), input)) move_count++;
    rewind(input);

    // Array to store the movements
    Coord2_i64 movements[move_count];
    memset(movements, 0, sizeof(movements));
    size_t move_index = 0;

    // Parse the movements from the input
    while (fgets(line, sizeof(line), input))
    {
        parse_input(line, &delta);
        movements[move_index++] = delta;
        coord_add(&pos, &delta);

        if (pos.x > max.x) max.x = pos.x;
        if (pos.y > max.y) max.y = pos.y;
        if (pos.x < min.x) min.x = pos.x;
        if (pos.y < min.y) min.y = pos.y;
    }
    
    fclose(input);
}