#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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

void coord_add(Coord2_i64 *restrict coord_a, Coord2_i64 *restrict coord_b)
{
    coord_a->x += coord_b->x;
    coord_a->y += coord_b->y;
}

static inline Coord2_i64 coord_diff(Coord2_i64 coord_a, Coord2_i64 coord_b)
{
    return (Coord2_i64){
        coord_b.x - coord_a.x,
        coord_b.y - coord_a.y
    };
}

static inline int64_t coord_dist(Coord2_i64 coord_a, Coord2_i64 coord_b)
{
    return abs(coord_a.x - coord_b.x) + abs(coord_a.y - coord_b.y);
}

static inline int64_t get_sign(int64_t value)
{
    if (value > 0)
    {
        // Positive
        return 1;
    }
    else if (value < 0)
    {
        // Negative
        return -1;
    }
    else
    {
        // Zero
        return 0;
    }
}

static inline Coord2_i64 coord_dir(Coord2_i64 coord_a, Coord2_i64 coord_b)
{
    return (Coord2_i64){
        get_sign(coord_b.x - coord_a.x),
        get_sign(coord_b.y - coord_a.y)
    };
}

int main(int argc, char **argv)
{
    FILE *input = fopen("test.txt", "rt");
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

    // Parse the movements from the input
    
    for (size_t move_index = 0; move_index < move_count; move_index++)
    {
        char *status = fgets(line, sizeof(line), input);
        
        // Calculate the distance to move
        parse_input(line, &delta);
        movements[move_index] = delta;
        
        // Move the rope's head
        coord_add(&pos, &delta);

        // Check the maximum and minimum coordinates
        if (pos.x > max.x) max.x = pos.x;
        if (pos.y > max.y) max.y = pos.y;
        if (pos.x < min.x) min.x = pos.x;
        if (pos.y < min.y) min.y = pos.y;
    }
    
    fclose(input);
    assert(max.x > min.x && max.y > min.y);

    const int64_t width  = abs(max.x - min.x) + 1;
    const int64_t height = abs(max.y - min.y) + 1;
    bool locations[height][width];
    memset(locations, 0, sizeof(locations));

    struct Rope
    {
        Coord2_i64 head;
        Coord2_i64 tail;
    } rope = {{abs(min.x), abs(min.y)}, {abs(min.x), abs(min.y)}};

    locations[rope.tail.y][rope.tail.x] = true;

    for (size_t i = 0; i < move_count; i++)
    {
        delta = movements[i];
        Coord2_i64 head_destination = rope.head;
        coord_add(&head_destination, &delta);

        const int64_t head_distance = coord_dist(rope.head, head_destination);
        Coord2_i64 head_direction = coord_dir(rope.head, head_destination);

        for (size_t j = 0; j < head_distance; j++)
        {
            coord_add(&rope.head, &head_direction);
            const int64_t tail_distance = coord_dist(rope.tail, rope.head);
            Coord2_i64 tail_direction = coord_dir(rope.tail, rope.head);
            
            switch (tail_distance)
            {
                case 3: // Separated on the diagonal
                    coord_add(&rope.tail, &tail_direction);
                    break;
                
                case 2: // Diagonally adjascent or separated on one axis
                    
                    if (abs(tail_direction.x) == 1 && abs(tail_direction.y) == 1)
                    {
                        // Diagonally adjascent (do nothing)
                    }
                    else if (tail_direction.y == 1)
                    {
                        // Vertically separated (move up or down)
                        rope.tail.y += tail_direction.y;
                    }
                    else
                    {
                        // Horizontally separated (move right or left)
                        rope.tail.x += tail_direction.x;
                    }
                    
                    break;
                
                case 1: // Vertically or horizontally adjascent
                case 0: // Both at the same position
                    // (do nothing)
                    break;

                default:
                    fprintf(stderr, "Error: Head and tail got too far away.\n");
                    abort();
                    break;
            }

            // Update the positions where the rope has been
            locations[rope.tail.y][rope.tail.x] = true;
        }
    }
    
    FILE *output = fopen("output.txt", "wt");
    for (int64_t y = height-1; y >= 0; y--)
    {
        for (int64_t x = 0; x < width; x++)
        {
            const char text = locations[y][x] ? '#' : '.';
            fputc(text, output);
        }

        fputc('\n', output);
    }
    fclose(output);
    
}