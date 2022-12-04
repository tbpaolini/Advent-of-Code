#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

int main (int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[16];
    uint64_t overlaps_full = 0; // Sections that fully overlap
    uint64_t overlaps_any = 0;  // Sections that overlap on at least one element
    
    uint64_t assignment[4] = {0, 0, 0, 0};  // {start_1, end_1, start_2, end_2}
    
    // Read the file line by line until the end
    while (fgets(line, sizeof(line), input) != NULL)
    {
        // Split the line string
        const char delimiters[] = "-,";         // Characters where to split the string
        char *token = strtok(line, delimiters); // Substring splitted from the string
        
        // Parse the values from the line
        for (size_t i = 0; i < 4; i++)
        {
            assignment[i] = atol(token);        // Convert the token string to numeric
            token = strtok(NULL, delimiters);   // Get the token until the next delimiter
        }
        
        // Check if the assignments overlap
        bool overlap1 = assignment[0] >= assignment[2] && assignment[1] <= assignment[3];   // If section 1 is fully in 2
        bool overlap2 = assignment[2] >= assignment[0] && assignment[3] <= assignment[1];   // If section 2 is fully in 1
        bool no_overlap = assignment[0] > assignment[3] || assignment[1] < assignment[2];   // If the sections do not intersect at all

        if (overlap1 || overlap2) overlaps_full++;  // One section is fully within another
        if (!no_overlap) overlaps_any++;            // The sections intersect in at least one element
    }

    fclose(input);

    printf("Part 1: %lu overlaps\n", overlaps_full);
    printf("Part 2: %lu overlaps\n", overlaps_any);
}