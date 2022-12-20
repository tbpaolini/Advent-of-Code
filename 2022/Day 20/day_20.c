#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct MixValue {
    int64_t number; // The value itself
    size_t index;   // Current index of the value on the array
} MixValue;

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("input.txt", "rt");
    char line[16];
    
    int64_t value_count = 0;
    while (fgets(line, sizeof(line), input)) value_count++;
    rewind(input);

    MixValue values[value_count];       // The input values in the original order
    MixValue *encrypted[value_count];   // Values on the original order, as pointers
    MixValue *decrypted[value_count];   // Values on the correct order, as pointers

    size_t num_index = 0;
    while (fgets(line, sizeof(line), input))
    {
        assert(isdigit(line[0]) || line[0] == '-');
        values[num_index] = (MixValue){atol(line), num_index};
        encrypted[num_index] = &values[num_index];
        decrypted[num_index] = &values[num_index];
        num_index++;
    }

    fclose(input);
}