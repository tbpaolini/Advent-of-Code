#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

int main(int arc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[128];
    size_t rows = 0;    // Amount of tree rows
    size_t columns = 0; // Amount of tree columns

    // Count how many rows and columns
    size_t columns_temp = 0;
    int next_char;
    while ( (next_char = fgetc(input)) != EOF )
    {
        if (next_char == '\n')
        {
            // A newline means a new column
            rows++;
            columns = columns_temp > columns ? columns_temp : columns;  // Largest column
            columns_temp = 0;
        }
        else
        {
            // A digit means a new row
            assert(isdigit(next_char));
            columns_temp++;
        }
    }

    // Return to the start of the file
    rewind(input);

    // Tree heights
    uint8_t trees[columns][rows];   // Height of the trees across both axes
    uint8_t max_tree_col[columns];  // Maximum tree height within each column
    uint8_t max_tree_row[rows];     // Maximum tree height within each row
    
    // Initialize all arrays to zero
    memset(trees, 0, sizeof trees);
    memset(max_tree_col, 0, sizeof max_tree_col);
    memset(max_tree_row, 0, sizeof max_tree_row);
    
    // Populate the arrays
    for (size_t y = 0; y < rows; y++)
    {
        char *status = fgets(line, sizeof(line), input);    // Read a line from the file
        for (size_t x = 0; x < columns; x++)                // Iterate over all digits on the line
        {
            const uint8_t tree =  line[x] - '0';    // Convert the text character to numeric
            trees[y][x] = tree;                     // Store the height of the tree
            if (max_tree_row[y] < tree) max_tree_row[y] = tree; // Store the maximum height in the row
            if (max_tree_col[x] < tree) max_tree_col[x] = tree; // Store the maximum height it the column
        }
    }
    
    // It is good practice to always close the file after finishing done with it :-)
    fclose(input);

    /************************ Part 1 ************************/
    
    // Map the visible trees
    uint64_t visible_count = (rows * 2) + (columns * 2) - 4;    // Count the trees on the perimeter
    bool visible[rows][columns];            // Which inner trees are visible
    memset(visible, 0, sizeof(visible));    // Initialize all values to 'false'
    
    // Iterate over the trees row by row
    for (size_t y = 1; y < rows - 1; y++)
    {
        bool is_visible_left  = true;               // If reached the highest tree from the left
        bool is_visible_right = true;               // If reached the highest tree from the right
        uint8_t local_max_left  = trees[y][0];      // Highest tree so far from the left
        uint8_t local_max_right = trees[y][rows-1]; // Highest tree so far from the right
        
        // Iterate on the column from both ends
        for (size_t x = 1; x < columns - 1; x++)
        {
            const uint8_t tree_left  = trees[y][x];             // Next tree from the left
            const uint8_t tree_right = trees[y][columns-x];     // Next tree from the right
            
            // Check if the left tree is bigger and visible
            if (is_visible_left && tree_left > local_max_left)
            {
                if (!visible[y][x]) visible_count++;    // Count this tree only once
                visible[y][x] = true;                   // Flag the tree as visible
                local_max_left = tree_left;             // Update the local maximum
                if (max_tree_row[y] == local_max_left) is_visible_left = false; // Have we reached the global maximum?
            }

            // Check if the right tree is bigger and visible
            if (is_visible_right && tree_right > local_max_right)
            {
                if (!visible[y][columns-x]) visible_count++;    // Count this tree only once
                visible[y][columns-x] = true;                   // Flag the tree as visible
                local_max_right = tree_right;                   // Update the local maximum
                if (max_tree_row[y] == local_max_right) is_visible_right = false;   // Have we reached the global maximum?
            }

            // If we reached the global maximum from both ends
            if (!is_visible_left && !is_visible_right) break;
        }
    }

    // Iterate over the trees row by row
    for (size_t x = 1; x < columns - 1; x++)
    {
        bool is_visible_top    = true;                  // If reached the highest tree from the top
        bool is_visible_bottom = true;                  // If reached the highest tree from the bottom
        uint8_t local_max_top    = trees[0][x];         // Highest tree so far from the top
        uint8_t local_max_bottom = trees[rows-1][x];    // Highest tree so far from the bottom
        
        // Iterate on the column from both ends
        for (size_t y = 1; y < rows - 1; y++)
        {
            const uint8_t tree_top    = trees[y][x];
            const uint8_t tree_bottom = trees[rows-y][x];
            
            // Check if the top tree is bigger and visible
            if (is_visible_top && tree_top > local_max_top)
            {
                if (!visible[y][x]) visible_count++;    // Count this tree only once
                visible[y][x] = true;                   // Flag the tree as visible
                local_max_top = tree_top;               // Update the local maximum
                if (local_max_top == max_tree_col[x]) is_visible_top = false;   // Have we reached the global maximum?
            }

            // Check if the bottom tree is bigger and visible
            if (is_visible_bottom && tree_bottom > local_max_bottom)
            {
                if (!visible[rows-y][x]) visible_count++;   // Count this tree only once
                visible[rows-y][x] = true;                  // Flag the tree as visible
                local_max_bottom = tree_bottom;             // Update the local maximum
                if (local_max_bottom == max_tree_col[x]) is_visible_bottom = false; // Have we reached the global maximum?
            }

            // If we reached the global maximum from both ends
            if (!is_visible_top && !is_visible_bottom) break;
        }
    }
    
    printf("Part 1: %lu visible trees\n", visible_count);

    /************************ Part 2 ************************/

    // The maximum scenic score
    uint64_t max_score = 0;

    // Iterate over all the inner visible trees
    // (the trees at the borders are guaranted to have a score of zero)
    for (size_t y1 = 1; y1 < rows - 1; y1++)
    {
        for (size_t x1 = 1; x1 < columns - 1; x1++)
        {
            if (!visible[y1][x1]) continue; // Skip the tree if not visible

            // Count the visible trees from each side
            uint64_t view_count[4] = {0, 0, 0, 0};  // {bottom, top, right, left}
            
            // Bottom side
            for (size_t y2 = y1+1; y2 < rows; y2++)
            {
                view_count[0]++;
                if (trees[y1][x1] == trees[y2][x1]) break;
            }
            
            // Top side
            for (int64_t y2 = y1-1; y2 >= 0; y2--)
            {
                view_count[1]++;
                if (trees[y1][x1] == trees[y2][x1]) break;
            }

            // Right side
            for (int64_t x2 = x1+1; x2 < columns; x2++)
            {
                view_count[2]++;
                if (trees[y1][x1] == trees[y1][x2]) break;
            }
            
            // Left side
            for (int64_t x2 = x1-1; x2 >= 0; x2--)
            {
                view_count[3]++;
                if (trees[y1][x1] == trees[y1][x2]) break;
            }

            // Calculate the score and update the maximum
            uint64_t my_score = view_count[0] * view_count[1] * view_count[2] * view_count[3];
            if (my_score > max_score) max_score = my_score;
        }
    }

    printf("Part 2: %lu scenic score\n", max_score);
}