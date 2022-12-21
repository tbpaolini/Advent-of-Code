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
        size_t next_id = (i == value_count - 1) ? 0 : i+1;
        size_t prev_id = (i == 0) ? value_count - 1 : i-1;
        
        values[i].next = &values[next_id];
        values[i].previous = &values[prev_id];

        if (values[i].number == 0) decrypted = &values[i];
    }

    return 0;
}