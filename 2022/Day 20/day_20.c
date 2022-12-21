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

static MixValue* cycle(MixValue *val, size_t count)
{
    MixValue *res = val->next;
    for (size_t i = 0; i < count-1; i++)
    {
        assert(res != val);
        res = res->next;
    }
    return res;
}

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
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
    for (int64_t i = 0; i < value_count; i++)
    {
        // Wrap the indexes around
        int64_t next_id = (i == value_count - 1) ? 0 : i+1;
        int64_t prev_id = (i == 0) ? value_count - 1 : i-1;
        
        // Link the current value to the next and the pevious
        values[i].next = &values[next_id];
        values[i].previous = &values[prev_id];

        // Starting point to read the decrypted values
        if (values[i].number == 0) decrypted = &values[i];
    }

    assert(decrypted == cycle(decrypted, value_count));

    for (int64_t i = 0; i < value_count; i++)
    {
        // Set the head of the list to the current value
        MixValue *value = &values[i];
        MixValue *head = value;                             
        
        // How many spaces to move the value in the circular list
        // Important: Since the other elements are being rotated around the current value,
        // we are taking the modulo by the list lenght minus 1 (not the list lenght itself).
        int64_t steps = value->number % (value_count - 1);

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

        assert(decrypted == cycle(decrypted, value_count));
    }

    int64_t solution_p1 = 0;
    
    for (size_t i = 1; i <= 3; i++)
    {
        MixValue *value_p1 = decrypted;
        
        for (size_t j = 0; j < (1000 * i) % value_count; j++)
        {
            value_p1 = value_p1->next;
        }
        
        solution_p1 += value_p1->number;
    }

    printf("Part 1: %ld\n", solution_p1);

    return 0;
}