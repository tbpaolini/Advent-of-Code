#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

// Struct to represent a monkey
typedef struct MathMonkey
{
    struct MathMonkey* others[2];   // Pointers to the other monkeys this one is waiting for
    char others_name[2][5];         // The names of the other monkeys this one is waiting for
    double number;                  // The number that this monkey should yell
    char name[5];                   // Name of this monkey
    char operation;                 // Operation this monkey should perform with the numbers of the other monkeys
} MathMonkey;

// prefix tree (trie) for searching the monkeys
typedef struct prefixTree {
    struct prefixTree* branches[26];    // One branch for each 'a' to 'z' letter
    struct MathMonkey* data;            // Pointer to the monkey
} prefixTree;

// Create a new node for the trie
static prefixTree* trie_new()
{
    prefixTree *trie = (prefixTree*)calloc(1, sizeof(prefixTree));
    if (!trie)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return trie;
}

// Insert a monkey pointer to the trie
static void trie_insert(prefixTree *trie, MathMonkey *monkey)
{
    if (!monkey || !monkey->name) return;
    char *key = monkey->name;
    
    // Start from the root node of the trie
    prefixTree *node = trie;
    char next_char;
    size_t pos = 0;
    
    // Loop over the characters
    while ( next_char = key[pos++] )
    {
        assert(islower(next_char));
        
        // Get the alphabet's position of the letter
        const size_t id = next_char - 'a';
        
        if (!node->branches[id])
        {
            // Create a new branch, if the character does not have one
            node->branches[id] = trie_new();
        }
        
        // Move to the character's branch
        node = node->branches[id];
        
        assert(pos <= 4);
    }
    
    // Insert the monkey pointer at the destination node
    node->data = monkey;
}

// Given a name string, retrieve the corresponding monkey pointer from the trie
static MathMonkey* trie_get(prefixTree *trie, char *name)
{
    // Start from the root node
    prefixTree *node = trie;
    char next_char;
    size_t pos = 0;
    
    // Loop over all characters of the name
    while ( next_char = name[pos++] )
    {
        assert(islower(next_char));
        
        // Move to the character's branch
        const size_t id = next_char - 'a';
        if (!node->branches[id]) return NULL;
        node = node->branches[id];
    }

    // Return the pointer at the destination node
    return node->data;
}

// Free the memory used by the trie
static void trie_destroy(prefixTree *trie)
{
    // Start from the root node
    prefixTree *node = trie;

    if (node)
    {
        // Recursively free the child nodes
        for (size_t i = 0; i < 26; i++)
        {
            trie_destroy(node->branches[i]);
        }
    }
    
    // Free the node after its children have been freed
    free(node);
}

// Recursively calculate which number the monkey should yell
static double get_monkey_number(MathMonkey *monkey)
{
    if (!monkey->operation)
    {
        // If this is a "number monkey", just return the number
        return monkey->number;
    }
    else
    {
        // Get the numbers this monkey should do an operation
        double other_numbers[2] = {
            get_monkey_number(monkey->others[0]),
            get_monkey_number(monkey->others[1])
        };

        // Perform the operation
        switch (monkey->operation)
        {
            case '+':
                return other_numbers[0] + other_numbers[1];
                break;
            
            case '-':
                return other_numbers[0] - other_numbers[1];
                break;
            
            case '/':
                return other_numbers[0] / other_numbers[1];
                break;
            
            case '*':
                return other_numbers[0] * other_numbers[1];
                break;

            default:
                fprintf(stderr, "Error: Invalid operation '%c'\n", monkey->operation);
                abort();
                break;
        }
    }
}

int main(int argc, char * argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[32];

    // Count how many monkeys there are
    size_t monkeys_count = 0;
    while (fgets(line, sizeof(line), input)) monkeys_count++;
    rewind(input);

    // Array to store the monkeys
    MathMonkey monkeys[monkeys_count];
    memset(monkeys, 0, sizeof(monkeys));

    // prefix Tree for searching the monkeys
    prefixTree *monkey_tree = trie_new();

    // Parse the input file line by line
    for (size_t i = 0; i < monkeys_count; i++)
    {
        char *status = fgets(line, sizeof(line), input);
        assert(status);
        
        // Split the line on colons and spaces
        const char delimiters[] = ": ";
        char *token = strtok(line, delimiters);
        
        // Store the monkey's name
        assert(strlen(token) == 4);
        strcpy(monkeys[i].name, token);
        trie_insert(monkey_tree, &monkeys[i]);

        token = strtok(NULL, delimiters);
        if (isdigit(token[0]))
        {
            // Store the monkey's number
            monkeys[i].number = atof(token);
        }
        else
        {
            // Store the name of the first monkey this one is waiting for
            assert(strlen(token) == 4);
            strcpy(monkeys[i].others_name[0], token);
            
            // Store the operation this monkey should perform with the numbers
            token = strtok(NULL, delimiters);
            assert(strlen(token) == 1);
            monkeys[i].operation = token[0];

            // Store the name of the second monkey this one is waiting for
            token = strtok(NULL, delimiters);
            assert(strlen(token) == 5 && token[4] == '\n');
            token[4] = '\0';
            strcpy(monkeys[i].others_name[1], token);
        }
    }

    fclose(input);

    // Link each monkey to the others they are waiting for
    for (size_t i = 0; i < monkeys_count; i++)
    {
        MathMonkey *monkey = &monkeys[i];
        
        for (size_t j = 0; j < 2; j++)
        {
            monkey->others[j] = trie_get(monkey_tree, monkey->others_name[j]);
        }
    }

    // The monkeys we are keeping track for the puzzle
    MathMonkey *root = trie_get(monkey_tree, "root");
    MathMonkey *humn = trie_get(monkey_tree, "humn");

    // The prefix Tree is no longer necessary because we have linked the monkeys
    trie_destroy(monkey_tree);

    /******************** Part 1 ********************/
    
    // Calculate the number that 'root' should yell
    double number_p1 = get_monkey_number(root);
    printf("Part 1: %.0f\n", number_p1);

    /******************** Part 2 ********************/

    // The previous guess for the number that 'humn' should yell
    double previous_number = humn->number;

    // Now 'root' is subtracting values
    // (if the result is zero, then the values are equal)
    root->operation = '-';

    // Error function: how far we currently are from zero
    double error = abs(get_monkey_number(root));
    double previous_error = error;

    // First guess for the number of 'humn'
    humn->number = 0;
    error = fabs(get_monkey_number(root));

    // Learning rate of the minimization algorithm (gradient descent)
    const double rate = 0.1;

    // Keep searching for the number until the error is smaller than 0.1
    while (error > 0.1)
    {
        // The direction and speed that the error function has changed
        // between the current and previous guesses
        double gradient;
        
        if (error != previous_error)
        {
            // Calculate the local derivative of the error function
            gradient = (humn->number - previous_number) / (error - previous_error);
        }
        else
        {
            // If we were to perform a division by zero, force the gradient to be 1 or -1
            gradient = (error < previous_error) ? 1.0 : -1.0;
        }

        // Remember the results of the previous guess
        previous_number = humn->number;
        previous_error = error;

        // Move the guessed value in the direction that the error function decreases
        humn->number -= rate * error * gradient;

        // Recalculate the error
        error = fabs(get_monkey_number(root));
    }

    printf("Part 2: %.0f\n", humn->number);

    return 0;
}