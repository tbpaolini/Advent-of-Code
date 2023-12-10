#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <float.h>
#include <math.h>

#define MAX_RACES 4
#define MAX_BUFFER 256

// Calculate in how many ways you can win a race
static uint64_t get_win_count(uint64_t time, uint64_t distance)
{
    if (time == 0 || distance == 0) return 0;

    /* Let T being the time, D the distance, and x how long the button was held.
       In order to win, this inequality has to be satisfied:
       (T - x) * x > D
       
       Solving it for x, we get:
       (T - sqrt(T*T - 4*D)) / 2  <  x  <  (T + sqrt(T*T - 4*D)) / 2

       Then we are going to get how many integers are in that range.
    */
    
    const double time_d = (double)time;
    const double distance_d = (double)distance;
    const double delta = (time_d * time_d) - (4.0 * distance_d);
    if (delta < 0.0) return 0;

    // Minimum held time
    double min_val = (time_d - sqrt(delta)) / 2.0;
    if (min_val < 0.0) min_val = 0.0;
    double intg_min = 0.0;
    double fract_min = modf(min_val, &intg_min);    // Break into inregral and fractional parts

    // Set the value to the next integer
    if (fabs(fract_min) < DBL_EPSILON)
    {
        // If the value was already an integer, just add 1
        min_val = min_val + 1;
    }
    else
    {
        min_val = ceil(min_val);
    }

    
    // Maximum held time
    double max_val = (time_d + sqrt(delta)) / 2.0;
    double intg_max = 0.0;
    double fract_max = modf(max_val, &intg_max);    // Break into inregral and fractional parts
    
    // Set the value to the previous integer
    if (fabs(fract_max) < DBL_EPSILON)
    {
        // If the value was already an integer, just subtract 1
        max_val = max_val - 1;
    }
    else
    {
        max_val = floor(max_val);
    }

    
    // Return the count of integers in the range
    return max_val - min_val + 1.0;
}

// Parse a line of input text from Part 1 into an array
// (It returns the amount of parsed values)
static size_t parse_line_p1(char *line, uint64_t *out, size_t max_count)
{
    char *start = strchr(line, ':');
    if (!start)
    {
        return 0;
    }
    else
    {
        start++;
    }

    char *end = NULL;
    size_t count = 0;
    
    while (count < max_count)
    {
        uint64_t val = strtoll(start, &end, 10);
        if (start == end) break;
        out[count++] = val;
        start = end;
    }

    return count;
}

// Parse a line of input text from Part 2 into a value
// (It returns 'true' on success)
static bool parse_line_p2(char *line, uint64_t *out)
{
    char *start = strchr(line, ':');
    if (!start) return false;
    char value_txt[MAX_BUFFER] = {0};   // Store the concatenated numbers
    size_t count = 0;

    // Parse all digits into the buffer
    while (*start != '\0' && count < MAX_BUFFER)
    {
        const char my_char = *start++;
        if (isdigit(my_char))
        {
            value_txt[count++] = my_char;
        }
    }
    
    char *end = NULL;

    // Convert the buffer into an integer
    uint64_t val = strtoll(value_txt, &end, 10);
    if (value_txt != end)
    {
        *out = val;
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Please run the program with the input file's path as the only argument.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "rt");
    if (!file)
    {
        fprintf(stderr, "Could not open '%s'.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // A line of text from the input
    char line[MAX_BUFFER] = {0};

    // Data for both parts
    uint64_t time[MAX_RACES] = {0};
    uint64_t distance[MAX_RACES] = {0};
    uint64_t time_p2 = 0;
    uint64_t distance_p2 = 0;
    bool success = false;

    // Parse the time
    char *status = fgets(line, sizeof(line), file);
    if (!status) goto parse_fail;
    size_t race_count = parse_line_p1(line, time, MAX_RACES);
    bool status_p2 = parse_line_p2(line, &time_p2);
    if (race_count == 0 || !status_p2) goto parse_fail;

    // Parse the distance
    status = fgets(line, sizeof(line), file);
    if (!status) goto parse_fail;
    size_t race_count_2 = parse_line_p1(line, distance, MAX_RACES);
    status_p2 = parse_line_p2(line, &distance_p2);
    if (race_count != race_count_2 || !status_p2) goto parse_fail;
    
    success = true;

    // Exit if failed parsing the input
    parse_fail:
    if (!success)
    {
        fclose(file);
        fprintf(stderr, "Invalid input file\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    // Part 1: Multiply together the amount of ways of winning each race
    uint64_t solution_p1 = 1;
    for (size_t i = 0; i < race_count; i++)
    {
        solution_p1 *= get_win_count(time[i], distance[i]);
    }
    printf("Part 1: %lu\n", solution_p1);

    // Part 2: The amount of ways of winning (after interpreting each line as a single value)
    uint64_t solution_p2 = get_win_count(time_p2, distance_p2);
    printf("Part 2: %lu\n", solution_p2);

    return 0;
}