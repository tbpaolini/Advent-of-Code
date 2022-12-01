#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

// Helper function for sorting 64-bit unsigned integers
int compare_ui64(const void *value1, const void*value2)
{
    // Read the values that the pointers are referencing
    uint64_t v1 = *(uint64_t*)value1;
    uint64_t v2 = *(uint64_t*)value2;
    
    // Compare the values
    if (v1 > v2)
    {
        return 1;
    }
    else if (v1 < v2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");

    char line[50];                  // Line buffer
    uint64_t max_calories = 0;      // Maximum elf
    uint64_t current_calories = 0;  // Current elf
    uint64_t top_calories[3] = {0, 0, 0};   // Store the top 3 elfs

    while (fgets(line, sizeof(line), input) != NULL)
    {
        if (line[0] == '\n')    // At line break
        {
            // Check the maximum
            if (current_calories > max_calories) max_calories = current_calories;

            // Sort the current top 3 values
            uint64_t values[4] = {top_calories[0], top_calories[1], top_calories[2], current_calories};
            qsort((void*)values, 4, sizeof(uint64_t), &compare_ui64);

            for (size_t i = 0; i < 3; i++)
            {
                // Store the current 3 biggest values
                top_calories[i] = values[3-i];
            }
            
            // Reset the current elf's value
            current_calories = 0;
        }
        else
        {
            // Convert the text to integer, and add it to the counter
            current_calories += atol(line);
        }    
    }
    
    // Process the last value
    if (current_calories > max_calories) max_calories = current_calories;
    uint64_t values[4] = {top_calories[0], top_calories[1], top_calories[2], current_calories};
    qsort((void*)values, 4, sizeof(uint64_t), &compare_ui64);
    for (size_t i = 0; i < 3; i++)
    {
        top_calories[i] = values[3-i];
    }

    printf("Part 1: %lu calories\n", max_calories);
    printf("Part 2: %lu calories\n", top_calories[0] + top_calories[1] + top_calories[2]);
    fclose(input);
}