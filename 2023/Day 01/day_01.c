#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

static const char* names[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
static const int values[]  = {1, 2, 3, 4, 5, 6, 7, 8, 9};

// A trie for storing the names of the digits
typedef struct PrefixTree {
    struct PrefixTree *branch[26];  // A branch for each of the 'a' to 'z' characters
    const int *data;                // Contains a pointer to the digit's value if on a terminator node, NULL otherwise
} PrefixTree;

// Allocate memory and set zero all its bytes, while also checking if the returned point is not NULL
static void* aoc_calloc(size_t count, size_t size)
{
    void *const my_ptr = calloc(count, size);
    if (!my_ptr)
    {
        fprintf(stderr, "Not enough memory.\n");
        exit(EXIT_FAILURE);
    }
    return my_ptr;
}

// Initialize the trie data structure with all the digit's names
static PrefixTree* trie_init()
{
    // Root node of the trie
    PrefixTree *my_trie = aoc_calloc(1, sizeof(PrefixTree));

    // Store the nine digits in the trie
    for (size_t i = 0; i < 9; i++)
    {
        // Current node on the trie
        PrefixTree *node = my_trie;

        const char *digit = names[i];   // Digit's name as a null terminated string
        size_t pos = 0;                 // Index on the string
        
        // Parse all characters on the string
        while (digit[pos])
        {
            // Character's index on the alphabet (0-indexed)
            const size_t idx = digit[pos++] - 'a';

            // Pointer to the character's branch on the trie
            PrefixTree **const next_node = &node->branch[idx];

            // Create the branch if it does not exist
            if ( !(*next_node) )
            {
                *next_node = aoc_calloc(1, sizeof(PrefixTree));
            }

            // Move to the branch
            node = *next_node;
        }
        
        // Add a pointer to the digit's value at its terminator node on the trie
        node->data = &values[i];
    }
    
    // Return the trie with all digits' names stored on it
    return my_trie;
}

// Free all the memory used by the trie
static void trie_destroy(PrefixTree *root)
{
    if (!root) return;
    for (size_t i = 0; i < 26; i++)
    {
        // Recursively free the children of the current node
        trie_destroy(root->branch[i]);
    }
    free(root);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Please run the program with the input file's path as the only argument.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "rt");
    if (!file)
    {
        fprintf(stderr, "Could not open '%s'.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    bool is_first_digit = true;
    int first_digit = 0;
    int last_digit = 0;
    uint64_t solution = 0;
    int my_char = 0;
    
    /********** Part 1 **********/
    
    while ( (my_char = fgetc(file)) != EOF )
    {
        // Update the solution's digits when a numeric character is found
        if (isdigit(my_char))
        {
            const int my_digit = my_char - '0';
            if (is_first_digit)
            {
                first_digit = my_digit;
                is_first_digit = false;
            }
            last_digit = my_digit;
        }

        // On a new line, update the solution and switch to the next value
        if (my_char == '\n')
        {
            solution += (10 * first_digit) + last_digit;
            is_first_digit = true;
            first_digit = 0;
            last_digit = 0;
        }
    }

    printf("Part 1: %lu\n", solution);

    /********** Part 2 **********/

    rewind(file);
    solution = 0;
    is_first_digit = true;
    first_digit = 0;
    last_digit = 0;
    
    // Root node of the trie with the names of the digits
    PrefixTree *trie_root = trie_init();
    
    // Current node on the trie (it represents the substring so far)
    PrefixTree *node = trie_root;

    char line[256] = {0};

    while ( fgets(line, sizeof(line), file) )
    {
        // Indexes of the first and last character of the substring
        size_t start = 0;
        size_t end = 0;
        
        // Keep reading the string until the substring's start reach the end of the string 
        while (line[start] != '\n' && line[start] != '\0')
        {
            // Current character of the substring
            const int my_char = line[end];
            
            if (isdigit(my_char))
            {
                // If the current character is a digit, output its value
                const int my_digit = my_char - '0';
                if (is_first_digit)
                {
                    first_digit = my_digit;
                    is_first_digit = false;
                }
                last_digit = my_digit;
                
                // Reset the substring
                node = trie_root;
                start = ++end;  // The substring is moved to the next position after the digit
            }
            else if (islower(my_char))
            {
                // If the current character is a lowercase letter, check if the substring is in the trie
                size_t idx = my_char - 'a';
                PrefixTree *next_node = node->branch[idx];
                
                if (next_node)
                {
                    if (next_node->data)
                    {
                        // If we are in a terminator node of the trie, output the substring's value
                        const int my_digit = *(next_node->data);
                        if (is_first_digit)
                        {
                            first_digit = my_digit;
                            is_first_digit = false;
                        }
                        last_digit = my_digit;
                        
                        // Reset the substring
                        node = trie_root;
                        end = ++start;  // The substring is moved to the next position after where it started
                    }
                    else
                    {
                        // If we are not in a terminator node of the trie, just move to the next node
                        node = next_node;
                        end++;  // The substring is expanded to the right by one position
                    }
                }
                else
                {
                    // If the substring is not in the trie, reset the substring
                    node = trie_root;
                    end = ++start;  // The substring is moved to the next position after where it started
                }
            }
            else if (my_char == '\n' || my_char == '\0')
            {
                // If the substring was expanded until the end of the string, reset the substring
                node = trie_root;
                end = ++start;  // The substring is moved to the next position after where it started
            }
        }

        // Update the solution when at the end of the string
        solution += (10 * first_digit) + last_digit;
        is_first_digit = true;
        first_digit = 0;
        last_digit = 0;
    }

    printf("Part 2: %lu\n", solution);
    
    // Clean-up
    trie_destroy(trie_root);
    fclose(file);

    return 0;
}