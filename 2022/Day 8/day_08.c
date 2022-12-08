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
    size_t rows = 0;
    size_t columns = 0;

    size_t columns_temp = 0;
    int next_char;
    while ( (next_char = fgetc(input)) != EOF )
    {
        if (next_char == '\n')
        {
            rows++;
            columns = columns_temp > columns ? columns_temp : columns;
            columns_temp = 0;
        }
        else
        {
            assert(isdigit(next_char));
            columns_temp++;
        }
    }

    rewind(input);

    uint8_t trees[columns][rows];
    uint8_t max_tree_col[columns];
    uint8_t max_tree_row[rows];
    memset(trees, 0, sizeof trees);
    memset(max_tree_col, 0, sizeof max_tree_col);
    memset(max_tree_row, 0, sizeof max_tree_row);
    
    for (size_t y = 0; y < rows; y++)
    {
        char *status = fgets(line, sizeof(line), input);
        for (size_t x = 0; x < columns; x++)
        {
            const uint8_t tree =  line[x] - '0';
            trees[y][x] = tree;
            if (max_tree_row[y] < tree) max_tree_row[y] = tree;
            if (max_tree_col[x] < tree) max_tree_col[x] = tree;
        }
    }
    
    fclose(input);
    
    uint8_t visible[rows][columns];
    memset(visible, 0, sizeof(visible));
    
    for (size_t y = 1; y < rows - 1; y++)
    {
        bool is_visible_left  = true;
        bool is_visible_right = true;
        uint8_t local_max_left  = trees[y][0];
        uint8_t local_max_right = trees[y][rows-1];
        
        for (size_t x = 1; x < columns - 1; x++)
        {
            const uint8_t tree_left  = trees[y][x];
            const uint8_t tree_right = trees[y][columns-x];
            if (is_visible_left && tree_left > local_max_left)
            {
                visible[y][x] += 1;
                local_max_left = tree_left;
                if (max_tree_row[y] == local_max_left) is_visible_left = false;
            }
            if (is_visible_right && tree_right > local_max_right)
            {
                visible[y][columns-x] += 1;
                local_max_right = tree_right;
                if (max_tree_row[y] == local_max_right) is_visible_right = false;
            }
            if (!is_visible_left && !is_visible_right) break;
        }
    }

    for (size_t x = 1; x < columns - 1; x++)
    {
        bool is_visible_top    = true;
        bool is_visible_bottom = true;
        uint8_t local_max_top    = trees[0][x];
        uint8_t local_max_bottom = trees[rows-1][x];
        
        for (size_t y = 1; y < rows - 1; y++)
        {
            const uint8_t tree_top    = trees[y][x];
            const uint8_t tree_bottom = trees[rows-y][x];
            
            if (is_visible_top && tree_top > local_max_top)
            {
                visible[y][x] += 1;
                local_max_top = tree_top;
                if (local_max_top == max_tree_col[x]) is_visible_top = false;
            }

            if (is_visible_bottom && tree_bottom > local_max_bottom)
            {
                visible[rows-y][x] += 1;
                local_max_bottom = tree_bottom;
                if (local_max_bottom == max_tree_col[x]) is_visible_bottom = false;
            }

            if (!is_visible_top && !is_visible_bottom) break;
        }
    }

    uint64_t visible_count = (rows * 2) + (columns * 2) - 4;
    for (size_t y = 0; y < rows; y++)
    {
        for (size_t x = 0; x < columns; x++)
        {
            if (visible[y][x] > 0) visible_count++;
        }
    }
    
    return 0;
}