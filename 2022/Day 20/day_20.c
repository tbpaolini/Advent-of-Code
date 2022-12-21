#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

typedef struct MixValue {
    int64_t number;             // The value itself
    struct MixValue *next;      // Next value on the list
    struct MixValue *previous;  // Previous value on the list
} MixValue;

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("test.txt", "rt");
    char line[16];
    
    int64_t value_count = 0;
    while (fgets(line, sizeof(line), input)) value_count++;
    rewind(input);

    MixValue values[value_count];   // Array of input values in the original order
    MixValue *decrypted = NULL;     // Values on a linked list in the correct order

    int64_t num_index = 0;
    while (fgets(line, sizeof(line), input))
    {
        assert(isdigit(line[0]) || line[0] == '-');
        values[num_index++] = (MixValue){atol(line), NULL, NULL};
    }

    fclose(input);

    // Link the values to each other
    for (size_t i = 0; i < value_count; i++)
    {
        // Wrap the indexes around
        size_t next_id = (i == value_count - 1) ? 0 : i+1;
        size_t prev_id = (i == 0) ? value_count - 1 : i-1;
        
        // Link the current value to the next and the pevious
        values[i].next = &values[next_id];
        values[i].previous = &values[prev_id];

        // Starting point to read the decrypted values
        if (values[i].number == 0) decrypted = &values[i];
    }

    for (size_t i = 0; i < value_count; i++)
    {
        MixValue *value = &values[i];
        MixValue *new_position = value;
        int64_t steps = value->number % value_count;    // How many steps to take in the circular list

        // Navigate fowards or backwards on the list
        if (steps > 0)
        {
            // Positive value: fowards
            for (int64_t j = 0; j < steps; j++)
            {
                new_position = new_position->next;
            }

            // On the "gap" left by the value, link the values there to each other
            value->previous->next = value->next;
            value->next->previous = value->previous;

            // Link the value to the ones that now come before and after
            value->previous = new_position;
            value->next = new_position->next;

            // Link the neighbors to the value
            new_position->next->previous = value;
            new_position->next = value;
        }
        else if (steps < 0)
        {
            // Negative value: backwards
            for (int64_t j = 0; j < -steps; j++)
            {
                new_position = new_position->previous;
            }

            // Positive value: fowards
            for (int64_t j = 0; j < value->number; j++)
            {
                new_position = new_position->next;
            }

            // On the "gap" left by the value, link the values there to each other
            value->previous->next = value->next;
            value->next->previous = value->previous;

            // Link the value to the ones that now come before and after
            value->next = new_position;
            value->previous = new_position->previous;

            // Link the neighbors to the value
            new_position->previous->next = value;
            new_position->previous = value;
        }
    }

    MixValue *test = decrypted;
    for (size_t i = 0; i < 3000 % value_count; i++)
    {
        test = test->next;
    }
    

    return 0;
}