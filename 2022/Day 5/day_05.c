#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

// Linked list for representing the stack of boxes
typedef struct BoxStack {
    char box;               // The character that represents the box
    struct BoxStack *next;  // The box below on the stack
} BoxStack;

// Create a new entry for the stack
static BoxStack *box_new(char box)
{
    // Allocate memory for the entry
    BoxStack *stack = (BoxStack*)malloc(sizeof(BoxStack));
    if (!stack)
    {
        fprintf(stderr, "Error: No enough memory.\n");
        abort();
    }
    
    // Initialize the values
    *stack = (BoxStack){
        .box = box,
        .next = NULL
    };
}

// Free the memory of a box and all the others below it
static void stack_destroy(BoxStack *top_box)
{
    while (top_box != NULL)
    {
        BoxStack *next_box = top_box->next;
        free(top_box);
        top_box = next_box;
    }
}

// Add the box to the stack
static void box_add(
    BoxStack *box,      // Pointer to the new entry
    BoxStack **stack    // Double pointer to the top entry of the stack
)
{
    box->next = *stack; // Point the box to the one below
    *stack = box;       // Set the stack's top to the new box
}

// Parse the data from the input file
static void parse_instructions(
    char *line,             // INPUT: line from the file (string)
    size_t *amount,         // OUTPUT: amount of boxes
    size_t *origin_id,      // OUTPUT: index of the origin stack
    size_t *destination_id  // OUTPUT: index of the destination stack
)
{
    // Split the line is the blank spaces
    const char delimiter[] = " ";
    char *token = strtok(line, delimiter);
    
    // Get the amount
    assert(strcmp(token, "move") == 0);
    token = strtok(NULL, delimiter);
    *amount = atol(token);

    // Get the origin
    token = strtok(NULL, delimiter);
    assert(strcmp(token, "from") == 0);
    token = strtok(NULL, delimiter);
    *origin_id = atol(token) - 1;
    
    // Get the destination
    token = strtok(NULL, delimiter);
    assert(strcmp(token, "to") == 0);
    token = strtok(NULL, delimiter);
    *destination_id = atol(token) - 1;
}

int main (int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[64];

    // Top box of each stack (for parts 1 and 2)
    BoxStack *stack_top_p1[9];
    BoxStack *stack_top_p2[9];

    // Bottom box of each stack (for parts 1 and 2)
    BoxStack *stack_bottom_p1[9];
    BoxStack *stack_bottom_p2[9];
    
    // Initialize all values to zero
    memset(stack_top_p1, 0, sizeof(stack_top_p1));
    memset(stack_bottom_p1, 0, sizeof(stack_bottom_p1));
    memset(stack_top_p2, 0, sizeof(stack_top_p2));
    memset(stack_bottom_p2, 0, sizeof(stack_bottom_p2));

    // Read the first few lines until we find a digit as the line's second character
    while (fgets(line, sizeof(line), input) != NULL)
    {
        if (isdigit(line[1])) break;
        size_t line_length = strlen(line);

        // The index of the stack
        size_t stack_id = 0;

        // Loop through all letters in the line
        // (they start at index 1, and there is another letter each 4 characters)
        for (size_t i = 1; i < line_length; i += 4)
        {
            assert(stack_id < 9);
            char box = line[i];
            
            if (box != ' ')
            {
                BoxStack *current_box_p1 = box_new(box);
                BoxStack *current_box_p2 = box_new(box);

                // If the stack is empty, add the box to the top of the stack
                if (!stack_top_p1[stack_id])
                {
                    stack_top_p1[stack_id] = current_box_p1;
                    stack_top_p2[stack_id] = current_box_p2;
                }

                // Link the box previously at the bottom to the new one
                if (stack_bottom_p1[stack_id]) stack_bottom_p1[stack_id]->next = current_box_p1;
                if (stack_bottom_p2[stack_id]) stack_bottom_p2[stack_id]->next = current_box_p2;
                
                // Add the box to the bottom of the stack
                stack_bottom_p1[stack_id] = current_box_p1;
                stack_bottom_p2[stack_id] = current_box_p2;
            }
            
            // Move to the next stack
            stack_id++;
        }
    }
    
    // Skip the blank line
    fgets(line, sizeof(line), input);
    assert(line[0] == '\n');
    
    // Read the remaining of the file, line by line until the end
    while (fgets(line, sizeof(line), input) != NULL)
    {
        size_t amount;          // Amount of boxes to move
        size_t origin_id;       // Where to move from
        size_t destination_id;  // Where to move to

        // Parse the data fromt he line
        parse_instructions(line, &amount, &origin_id, &destination_id);

        // Part 1: Move individual boxes from one stack to the other
        for (size_t i = 0; i < amount; i++)
        {
            BoxStack *moved_box = stack_top_p1[origin_id];
            
            // Remove the box from the top of the origin stack
            stack_top_p1[origin_id] = stack_top_p1[origin_id]->next;

            // Link the moved box to the destination stack
            moved_box->next = stack_top_p1[destination_id];
            
            // Put the box at the top of the destination stack
            stack_top_p1[destination_id] = moved_box;
        }

        // Part 2: Move multiple boxes from one stack to the other
        BoxStack *top_box = stack_top_p2[origin_id];    // Top box of the moved pile
        BoxStack *bottom_box = stack_top_p2[origin_id]; // Bottom box of the moved pile

        // Get to pile's bottom
        for (size_t j = 0; j < amount - 1; j++)
        {
            bottom_box = bottom_box->next;
        }
        
        // Remove the pile from the top of the origin stack
        stack_top_p2[origin_id] = bottom_box->next;

        // Link the pile to the destination stack
        bottom_box->next = stack_top_p2[destination_id];
        
        // Put the pile at the top of the destination stack
        stack_top_p2[destination_id] = top_box;
    }

    fclose(input);

    // Get the top boxes of each stack
    
    char answer_p1[10] = "";    // Part 1
    char answer_p2[10] = "";    // Part 2
    for (size_t i = 0; i < 9; i++)
    {
        answer_p1[i] = stack_top_p1[i]->box;
        answer_p2[i] = stack_top_p2[i]->box;
        stack_destroy(stack_top_p1[i]);
        stack_destroy(stack_top_p2[i]);
    }
    answer_p1[9] = '\0';
    answer_p1[9] = '\0';

    printf("Part 1: %s\n", answer_p1);
    printf("Part 2: %s\n", answer_p2);
}