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
    
    uint64_t visible_count = (rows * 2) + (columns * 2) - 4;
    bool visible[rows][columns];
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
                if (!visible[y][x]) visible_count++;
                visible[y][x] = true;
                local_max_left = tree_left;
                if (max_tree_row[y] == local_max_left) is_visible_left = false;
            }
            if (is_visible_right && tree_right > local_max_right)
            {
                if (!visible[y][columns-x]) visible_count++;
                visible[y][columns-x] = true;
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
                if (!visible[y][x]) visible_count++;
                visible[y][x] = true;
                local_max_top = tree_top;
                if (local_max_top == max_tree_col[x]) is_visible_top = false;
            }

            if (is_visible_bottom && tree_bottom > local_max_bottom)
            {
                if (!visible[rows-y][x]) visible_count++;
                visible[rows-y][x] = true;
                local_max_bottom = tree_bottom;
                if (local_max_bottom == max_tree_col[x]) is_visible_bottom = false;
            }

            if (!is_visible_top && !is_visible_bottom) break;
        }
    }
    
    printf("Part 1: %lu visible trees\n", visible_count);

    uint64_t max_score = 0;

    for (size_t y1 = 1; y1 < rows - 1; y1++)
    {
        for (size_t x1 = 1; x1 < columns - 1; x1++)
        {
            if (!visible[y1][x1]) continue;

            uint64_t view_count[4] = {0, 0, 0, 0};
            
            for (size_t y2 = y1+1; y2 < rows; y2++)
            {
                view_count[0]++;
                if (trees[y1][x1] == trees[y2][x1]) break;
            }
            
            for (int64_t y2 = y1-1; y2 >= 0; y2--)
            {
                view_count[1]++;
                if (trees[y1][x1] == trees[y2][x1]) break;
            }

            for (int64_t x2 = x1+1; x2 < columns; x2++)
            {
                view_count[2]++;
                if (trees[y1][x1] == trees[y1][x2]) break;
            }
            
            for (int64_t x2 = x1-1; x2 >= 0; x2--)
            {
                view_count[3]++;
                if (trees[y1][x1] == trees[y1][x2]) break;
            }

            uint64_t my_score = view_count[0] * view_count[1] * view_count[2] * view_count[3];
            if (my_score > max_score) max_score = my_score;
        }
    }

    printf("Part 2: %lu scenic score\n", max_score);
}