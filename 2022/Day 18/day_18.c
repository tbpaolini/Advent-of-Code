#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct LavaCoord
{
    int64_t x;
    int64_t y;
    int64_t z;
} LavaCoord;

typedef struct LavaList
{
    struct LavaCoord coord;
    struct LavaList  *next;
} LavaList;

// Create a new node for a linked list
static LavaList* list_new()
{
    LavaList *list = (LavaList*)calloc(1, sizeof(LavaList));
    if (!list)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return list;
}

// Given the first element of a linked list, free the memory of all elemets
static void list_destroy(LavaList *list_head)
{
    LavaList *node = list_head;
    while (node)
    {
        LavaList *next_node = node->next;
        free(node);
        node = next_node;
    }
}


int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char line[16];

    // List of coordinates
    LavaList *coord_list = NULL;
    LavaList *last_coord = NULL;

    while (fgets(line, sizeof(line), input))
    {
        // Create a new entry on the list
        LavaList *coord = list_new();
        if (!coord_list) coord_list = coord;        // Set the list's head if this is the first entry
        if (last_coord) last_coord->next = coord;   // Link the previous entry to the current one
        last_coord = coord;                         // Move the list's tail to the current entry

        // Split the line on the comma to get the (x,y,z) coordinates
        const char delimiter[] = ",";
        
        char *token = strtok(line, delimiter);
        assert(isdigit(token[0]));
        coord->coord.x = atol(token);
        
        token = strtok(NULL, delimiter);
        assert(isdigit(token[0]));
        coord->coord.y = atol(token);
        
        token = strtok(NULL, delimiter);
        assert(isdigit(token[0]));
        coord->coord.z = atol(token);
    }
    
    fclose(input);

    return 0;
}