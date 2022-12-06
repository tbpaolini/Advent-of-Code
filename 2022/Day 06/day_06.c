#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

// Check if all characters in a string are unique lowercase letters
static bool is_unique(char *string, size_t sequence_size)
{
    // An array of 26 booleans to check if a character has been seen previously
    static bool seen_chars[26];
    memset(seen_chars, 0, sizeof(seen_chars));  // Initialize all values to 'false'
    
    // Loop through all characters in the sequence
    for (size_t i = 0; i < sequence_size; i++)
    {
        // The character must be a lowercase letter
        if (!islower(string[i])) return false;
        
        // The index of the character on the alphabet
        uint8_t char_index = string[i] - 'a';
        
        if (seen_chars[char_index])
        {
            // The character has already been seen
            return false;
        }
        else
        {
            // Flag the character as seen
            seen_chars[char_index] = true;
        }
    }
    
    // All characters are unique
    return true;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");

    // Find the file size
    fseek(input, 0, SEEK_END);
    size_t file_size = ftell(input);
    if (file_size > 10000000) abort();
    fseek(input, 0, SEEK_SET);
    
    // Read the entire file into the buffer
    char buffer[file_size];
    size_t status = fread(buffer, sizeof(buffer), 1, input);
    fclose(input);

    // Loop through all positions in the buffer
    
    size_t count_p1 = 0;    // Character counter for part 1
    size_t count_p2 = 0;    // Character counter for part 2
    
    // Part 1: sequence size of 4
    for (size_t i = 0; i < sizeof(buffer) - 4; i++)
    {
        // Check if the 4 characters from here are unique
        if (is_unique(buffer + i, 4))
        {
            count_p1 = i + 4;
            break;
        }
    }
    
    // Part 2: sequence size of 14
    for (size_t i = 0; i < sizeof(buffer) - 14; i++)
    {
        // Check if the 14 characters from here are unique
        if (is_unique(buffer + i, 14))
        {
            count_p2 = i + 14;
            break;
        }
    }

    printf("Part 1: %lu characters\n", count_p1);
    printf("Part 2: %lu characters\n", count_p2);
}