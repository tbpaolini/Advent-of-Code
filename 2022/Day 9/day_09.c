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

// Extract the movement from a line of the input file
static void parse_input(char *line, Coord2_i64 *output)
{
    const char *direction = &line[0];           // Direction to move
    const int64_t distance = atol(&line[2]);    // Distance in that direction
    
    switch (direction[0])
    {
        case 'R':
            // Move right
            output->x = distance;
            output->y = 0;
            break;
        
        case 'U':
            // Move up
            output->x = 0;
            output->y = distance;
            break;
        
        case 'L':
            // Move left
            output->x = -1 * distance;
            output->y = 0;
            break;
        
        case 'D':
            // Move down
            output->x = 0;
            output->y = -1 * distance;
            break;
        
        default:
            // We should never reach this branch
            fprintf(stderr, "Error: Malformatted input\n");
            abort();
            break;
    }
}

// Takes 2 coordinates, and do in-place addition of the second to the first
void coord_add(Coord2_i64 *restrict coord_a, Coord2_i64 *restrict coord_b)
{
    coord_a->x += coord_b->x;
    coord_a->y += coord_b->y;
}

// Difference between two coordinates on each axis
static inline Coord2_i64 coord_diff(Coord2_i64 coord_a, Coord2_i64 coord_b)
{
    return (Coord2_i64){
        coord_b.x - coord_a.x,
        coord_b.y - coord_a.y
    };
}

// Absolute taxicab distance between two coordinates
static inline int64_t coord_dist(Coord2_i64 coord_a, Coord2_i64 coord_b)
{
    return abs(coord_a.x - coord_b.x) + abs(coord_a.y - coord_b.y);
}

// Returns whether a signed integer is positive (+1), negative (-1), or zero (0)
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

// The relative direction, on each axis, between two coordinates:
// +1: above (y) or right (x)
// -1: below (y) or left  (x)
//  0: on the same axis position
static inline Coord2_i64 coord_dir(Coord2_i64 coord_a, Coord2_i64 coord_b)
{
    return (Coord2_i64){
        get_sign(coord_b.x - coord_a.x),
        get_sign(coord_b.y - coord_a.y)
    };
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

    // Calculate the dimensions of the board
    const int64_t width  = abs(max.x - min.x) + 1;
    const int64_t height = abs(max.y - min.y) + 1;

    // Store the locations where the rope has been
    bool locations_p1[height][width];   // Part 1's locations
    bool locations_p2[height][width];   // Part 2's locations
    memset(locations_p1, 0, sizeof(locations_p1));
    memset(locations_p2, 0, sizeof(locations_p1));

    // Part 1's rope
    struct Rope
    {
        Coord2_i64 head;
        Coord2_i64 tail;
    } rope_p1 = {{abs(min.x), abs(min.y)}, {abs(min.x), abs(min.y)}};

    locations_p1[rope_p1.tail.y][rope_p1.tail.x] = true;
    uint64_t location_count_p1 = 1;

    // Part 2's rope
    Coord2_i64 rope_p2[10];
    for (size_t i = 0; i < 10; i++)
    {
        // Starting position of all knots
        rope_p2[i] = (Coord2_i64){abs(min.x), abs(min.y)};
    }

    locations_p2[rope_p2[9].y][rope_p2[9].x] = true;
    uint64_t location_count_p2 = 1;

    // Perform all movements on both ropes
    for (size_t i = 0; i < move_count; i++)
    {
        // Get the destination of the ropes' head
        delta = movements[i];
        Coord2_i64 head_destination = rope_p1.head;
        coord_add(&head_destination, &delta);

        // Get the direction and distance the head will move
        const int64_t head_distance = coord_dist(rope_p1.head, head_destination);
        Coord2_i64 head_direction = coord_dir(rope_p1.head, head_destination);

        // Move the head one step at a time
        for (size_t j = 0; j < head_distance; j++)
        {
            /******************** Part 1 ********************/
            
            // Move the head
            coord_add(&rope_p1.head, &head_direction);
            
            // Get the distance and orientation of the tail in relation to the head
            const int64_t tail_distance_p1 = coord_dist(rope_p1.tail, rope_p1.head);
            Coord2_i64 tail_direction_p1 = coord_dir(rope_p1.tail, rope_p1.head);
            
            // Move the tail if it is not asjascent to the head
            switch (tail_distance_p1)
            {
                case 3: // Separated on the diagonal
                    coord_add(&rope_p1.tail, &tail_direction_p1);
                    break;
                
                case 2: // Diagonally adjascent or separated on one axis
                    
                    if (abs(tail_direction_p1.x) == 1 && abs(tail_direction_p1.y) == 1)
                    {
                        // Diagonally adjascent (do nothing)
                    }
                    else if (abs(tail_direction_p1.y) == 1)
                    {
                        // Vertically separated (move up or down)
                        rope_p1.tail.y += tail_direction_p1.y;
                    }
                    else
                    {
                        // Horizontally separated (move right or left)
                        assert(abs(tail_direction_p1.x) == 1 && tail_direction_p1.y == 0);
                        rope_p1.tail.x += tail_direction_p1.x;
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

            // Update the positions where the tail has been
            if (!locations_p1[rope_p1.tail.y][rope_p1.tail.x]) location_count_p1++;
            locations_p1[rope_p1.tail.y][rope_p1.tail.x] = true;

            /******************** Part 2 ********************/

            // Move the head
            coord_add(&rope_p2[0], &head_direction);
            
            // Loop through the 9 knots of the tail
            for (int64_t k = 1; k < 10; k++)
            {
                // Get the distance and orientation of the knot in relation to the previous
                const int64_t tail_distance_p2 = coord_dist(rope_p2[k], rope_p2[k-1]);
                Coord2_i64 tail_direction_p2 = coord_dir(rope_p2[k], rope_p2[k-1]);

                // Move the knot if it is not asjascent to the previous
                // Note: When there are more than 2 knots, there is te possibility that one knot moves
                //       diagonally in relation to the next one. Because of that, we are also checking
                //       the distance of 4 (which is where the next knot will land is this case).
                switch (tail_distance_p2)
                {
                    case 3: // Separated on the diagonal
                    case 4: // Separated on the diagonal
                        coord_add(&rope_p2[k], &tail_direction_p2);
                        break;
                    
                    case 2: // Diagonally adjascent or separated on one axis
                        
                        if (abs(tail_direction_p2.x) == 1 && abs(tail_direction_p2.y) == 1)
                        {
                            // Diagonally adjascent (do nothing)
                        }
                        else if (abs(tail_direction_p2.y) == 1)
                        {
                            // Vertically separated (move up or down)
                            rope_p2[k].y += tail_direction_p2.y;
                        }
                        else
                        {
                            // Horizontally separated (move right or left)
                            assert(abs(tail_direction_p2.x) == 1 && tail_direction_p2.y == 0);
                            rope_p2[k].x += tail_direction_p2.x;
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
            }

            // Update the positions where the last knot has been
            if (!locations_p2[rope_p2[9].y][rope_p2[9].x]) location_count_p2++;
            locations_p2[rope_p2[9].y][rope_p2[9].x] = true;
        }
    }
    
    // Save the visualizations to text files
    FILE *output_p1 = fopen("output_p1.txt", "wt");
    FILE *output_p2 = fopen("output_p2.txt", "wt");
    
    for (int64_t y = height-1; y >= 0; y--) // From top to bottom
    {
        for (int64_t x = 0; x < width; x++) // From left to right
        {
            // Write '#' if the rope has been on the position, '.' otherwise
            const char text_p1 = locations_p1[y][x] ? '#' : '.';
            const char text_p2 = locations_p2[y][x] ? '#' : '.';
            fputc(text_p1, output_p1);
            fputc(text_p2, output_p2);
        }

        // Line break at the end of the row
        fputc('\n', output_p1);
        fputc('\n', output_p2);
    }

    fclose(output_p1);
    fclose(output_p2);

    printf("Part 1: %lu positions\n", location_count_p1);
    printf("Part 2: %lu positions\n", location_count_p2);

    return 0;
}