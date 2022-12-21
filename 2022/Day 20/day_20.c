#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>


// Circular doubly linked list
// The elements are linked to previous and next elements,
// while the "first" and "last" elements are linked to each other
typedef struct CircularList {
    int64_t number;                 // The value itself
    struct CircularList *next;      // Next value on the list
    struct CircularList *previous;  // Previous value on the list
} CircularList;

// Perform mixing on the values
// Note: The function updates the pointers between the values, the array itself is not changed.
//       The values are in a circular doubly linked list.
static void mix(
    int64_t value_count,                // Amount of values
    CircularList values[value_count],   // Array of list's nodes (in the original order of the values)
    size_t mix_times                    // How many times to mix
)
{
    // Perform one or more rounds of mixing
    for (size_t n = 0; n < mix_times; n++)
    {
        // Loop through all values in the original order
        for (int64_t i = 0; i < value_count; i++)
        {
            // Set the head of the list to the current value
            CircularList *value = &values[i];
            CircularList *head = value;                             
            
            // How many spaces to move the value in the circular list
            // IMPORTANT: Since the other elements are being rotated around the current value,
            // we are taking the modulo by the list lenght minus 1 (not the list lenght itself).
            int64_t steps = value->number % (value_count - 1);

            // Check which direction is the shortest to rotate the list,
            // while still reaching the same destination
            int64_t other_dir_steps = (value_count - 1) + steps;
            if ( abs(other_dir_steps) <  abs(steps) )
            {
                steps = other_dir_steps;
            }

            // Do nothing if the value ends up in the same position
            if (steps == 0) continue;

            // Remove the head from the list
            // (the previous element becomes the new head)
            head->previous->next = head->next;
            head->next->previous = head->previous;
            head = head->previous;

            // Rotate the list's head forwards or backwards
            if (steps > 0)
            {
                // Positive value: forwards
                for (int64_t j = 0; j < steps; j++)
                {
                    head = head->next;
                }
            }
            else if (steps < 0)
            {
                // Negative value: backwards
                for (int64_t j = 0; j < -steps; j++)
                {
                    head = head->previous;
                }
            }

            // Insert the value after the current head
            value->previous = head;
            value->next = head->next;
            head->next->previous = value;
            head->next = value;
        }
    }
}

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char line[16];
    
    int64_t value_count = 0;
    while (fgets(line, sizeof(line), input)) value_count++;
    rewind(input);

    CircularList values_p1[value_count];    // Array of input values in the original order (for Part 1)
    CircularList *decrypted_p1 = NULL;      // Values on a circular linked list in the correct order (starting from 0)

    // Insert the input values on the array
    int64_t num_index = 0;
    while (fgets(line, sizeof(line), input))
    {
        assert(isdigit(line[0]) || line[0] == '-');
        values_p1[num_index++] = (CircularList){atol(line), NULL, NULL};
    }

    fclose(input);

    // Create a copy of the array (for Part 2)
    CircularList values_p2[value_count];
    memcpy(values_p2, values_p1, sizeof(values_p2));
    CircularList *decrypted_p2 = NULL;  // Values on a circular linked list in the correct order (starting from 0)

    // Link the values to each other
    for (int64_t i = 0; i < value_count; i++)
    {
        // Wrap the indexes around
        int64_t next_id = (i == value_count - 1) ? 0 : i+1;
        int64_t prev_id = (i == 0) ? value_count - 1 : i-1;
        
        // Link the current value to the next and the pevious
        values_p1[i].next = &values_p1[next_id];
        values_p1[i].previous = &values_p1[prev_id];
        values_p2[i].next = &values_p2[next_id];
        values_p2[i].previous = &values_p2[prev_id];

        // Starting point to read the decrypted values
        if (values_p1[i].number == 0) decrypted_p1 = &values_p1[i];
        if (values_p2[i].number == 0) decrypted_p2 = &values_p2[i];
    }

    // Perform one round of mixing for Part 1
    mix(value_count, values_p1, 1);

    // Apply the decryption key for Part 2
    for (int64_t i = 0; i < value_count; i++)
    {
        values_p2[i].number *= 811589153;
    }

    // Perform 10 rounds of mixing for Part 2
    mix(value_count, values_p2, 10);
    
    // Answers for both parts
    int64_t solution_p1 = 0;
    int64_t solution_p2 = 0;
    
    // Check the 1000th, 2000th, and 3000th values on the lists
    for (size_t i = 1; i <= 3; i++)
    {
        // Start from the value of zero
        CircularList *value_p1 = decrypted_p1;
        CircularList *value_p2 = decrypted_p2;
        
        // Check the corresponding value after that
        for (size_t j = 0; j < (1000 * i) % value_count; j++)
        {
            value_p1 = value_p1->next;
            value_p2 = value_p2->next;
        }
        
        // Add the value to the solution
        solution_p1 += value_p1->number;
        solution_p2 += value_p2->number;
    }

    // Print the answers
    printf("Part 1: %ld\n", solution_p1);
    printf("Part 2: %ld\n", solution_p2);

    return 0;
}