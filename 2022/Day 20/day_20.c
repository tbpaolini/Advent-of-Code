#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>

typedef struct MixValue {
    int64_t number; // The value itself
    int64_t index;  // Current index of the value on the array
} MixValue;

static inline void swap_pointers(void **pointer_1, void **pointer_2)
{
    *pointer_1 = (void*)((ptrdiff_t)(*pointer_1) ^ (ptrdiff_t)(*pointer_2));
    *pointer_2 = (void*)((ptrdiff_t)(*pointer_2) ^ (ptrdiff_t)(*pointer_1));
    *pointer_1 = (void*)((ptrdiff_t)(*pointer_1) ^ (ptrdiff_t)(*pointer_2));
}

int main(int argc, char **argv)
{
    // Open the input file
    FILE *input = fopen("test.txt", "rt");
    char line[16];
    
    int64_t value_count = 0;
    while (fgets(line, sizeof(line), input)) value_count++;
    rewind(input);

    MixValue values[value_count];       // The input values in the original order
    MixValue *encrypted[value_count];   // Values on the original order, as pointers
    MixValue *decrypted[value_count];   // Values on the correct order, as pointers

    int64_t num_index = 0;
    while (fgets(line, sizeof(line), input))
    {
        assert(isdigit(line[0]) || line[0] == '-');
        values[num_index] = (MixValue){atol(line), num_index};
        encrypted[num_index] = &values[num_index];
        decrypted[num_index] = &values[num_index];
        num_index++;
    }

    fclose(input);

    for (int64_t i = 0; i < value_count; i++)
    {
        MixValue *value = &values[i];
        
        int64_t old_index = value->index;
        int64_t new_index = (value->number + old_index) % value_count;
        
        if (new_index <= 0) new_index = value_count + new_index - 1;
        if (new_index == old_index) continue;
        
        value->index = new_index;

        // Direction that the value is being moved: +1 = right; -1 = left
        int64_t dir = new_index > old_index ? +1 : -1;

        for (int64_t j = old_index + dir; j != new_index; j += dir)
        {
            // The values between the new and old indexes are moved to the opposite direction
            decrypted[j]->index -= dir;
        }
        
        decrypted[new_index]->index -= dir;

        if (dir == 1)
        {
            memmove(&decrypted[old_index], &decrypted[old_index+1], (new_index - old_index) * sizeof(MixValue*));
        }
        else
        {
            memmove(&decrypted[new_index], &decrypted[new_index+1], (old_index - new_index) * sizeof(MixValue*));
        }

        decrypted[new_index] = value;

        for (size_t i = 0; i < value_count; i++)
        {
            printf("%ld, ", decrypted[i]->number);
        }
        printf("\n");
    }

    return 0;
}