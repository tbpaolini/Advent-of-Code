#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct Monkey
{
    struct ItemQueue    // A queue of items (first in, last out)
    {
        struct ItemNode *head;  // First item of the queue
        struct ItemNode *tail;  // Last item of the queue
    } items;
    char operator;      // The operator that the monkey will apply to the worry level ('+' or '*')
    uint64_t operand;   // The value that the monkey will add or multiply to the worry level (if this is UINT64_MAX, then the operand is the current worry level itself)
    uint64_t div_test;  // Test if the worry level's result is divisible by this value
    size_t if_true;     // Index of the next monkey if the test passes
    size_t if_false;    // Index of the next monkey if the test fails
    uint64_t activity;  // How many times this monkey has inspected an item
} Monkey;

// A node on the queue of items
typedef struct ItemNode
{
    uint64_t worry_level;       // Worry level of the current item
    struct ItemNode *next;      // Next item on the queue
    struct ItemNode *previous;  // Previous item on the queue
} ItemNode;

// Create a new node for the item queue
static ItemNode* node_new()
{
    ItemNode *node = (ItemNode*)calloc(1, sizeof(ItemNode));
    if (node == NULL)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return node;
}

// Add an item to the end of the queue
static void queue_add(Monkey *monkey, ItemNode *item)
{
    // Link the last item to the new one (if there is a previous item)
    if (monkey->items.tail) monkey->items.tail->next = item;

    // Link the new item to the last one
    item->previous = monkey->items.tail;

    // Put the item at the end of the queue
    monkey->items.tail = item;

    // If this is the first item, add it to the beginning
    if (!monkey->items.head) monkey->items.head = item;
}

// Remove an item from the beginning of the queue
static ItemNode* queue_pop(Monkey *monkey)
{
    // Get the first item on the queue
    ItemNode *item = monkey->items.head;
    if (!item) return NULL;

    // Move the queue's head to the next item
    monkey->items.head = item->next;

    // If the removed item was the last, set the queue's tail to NULL
    if (!item->next) monkey->items.tail = NULL;

    return item;
}

// Free the memory used by a queue
static void queue_destroy(Monkey *monkey)
{
    ItemNode *current_item = monkey->items.head;
    while (current_item)
    {
        ItemNode *next_item = current_item->next;
        free(current_item);
        current_item = next_item;
    }
}

// Check if a string begins with a specified substring
static inline bool str_startswith(char *string, char *substring)
{
    const size_t sub_length = strlen(substring);
    return strncmp(string, substring, sub_length) == 0;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[128];
    
    // Count how many lines the file has
    size_t line_count = 0;
    while (fgets(line, sizeof(line), input)) {line_count++;}
    rewind(input);

    // Calculate the amount of monkeys
    // Notes:
    //  - One monkey each 7 lines.
    //  - Added 1 to the line count because there is not a blank line at the end of the file.
    size_t monkey_count = (line_count + 1) / 7;

    // Array to store the state of each monkey
    Monkey monkeys[monkey_count];
    memset(monkeys, 0, sizeof(monkeys));

    // Parse the initial state of the monkeys
    for (size_t i = 0; i < monkey_count; i++)
    {
        char *status = NULL;
        
        // Parse the monkey's index
        status = fgets(line, sizeof(line), input);
        assert(str_startswith(line, "Monkey ") && isdigit(line[7]));
        const size_t monkey_index = atol(&line[7]);
        assert(monkey_index == i);

        // Parse the queue of items on the monkey
        status = fgets(line, sizeof(line), input);
        assert(str_startswith(line, "  Starting items: "));
        const char item_delimiter[] = ",";
        char *item_token = strtok(&line[18], item_delimiter);
        
        while (item_token != NULL)
        {
            ItemNode *new_item = node_new();            // Create the new item
            new_item->worry_level = atol(item_token);   // Convert the item's worry level to numeric
            queue_add(&monkeys[i], new_item);           // Add the item to the monkey's queue
            item_token = strtok(NULL, item_delimiter);  // Get the next item
        }

        // Parse the operation to be performed with the item
        status = fgets(line, sizeof(line), input);
        assert(str_startswith(line, "  Operation: new = old "));
        
        // Whether to perform addition or multiplication
        monkeys[i].operator = line[23];
        assert(monkeys[i].operator == '+' || monkeys[i].operator == '*');

        // The value to add or multiply
        assert(isdigit(line[25]) || strcmp(&line[25], "old\n") == 0);
        monkeys[i].operand = isdigit(line[25]) ? atol(&line[25]) : UINT64_MAX;

        // Parse the division test
        status = fgets(line, sizeof(line), input);
        assert(str_startswith(line, "  Test: divisible by ") && isdigit(line[21]));
        monkeys[i].div_test = atol(&line[21]);

        // Next monkey if the division test passes
        status = fgets(line, sizeof(line), input);
        assert(str_startswith(line, "    If true: throw to monkey ") && isdigit(line[29]));
        monkeys[i].if_true = atol(&line[29]);

        // Next monkey if the division test fails
        status = fgets(line, sizeof(line), input);
        assert(str_startswith(line, "    If false: throw to monkey ") && isdigit(line[30]));
        monkeys[i].if_false = atol(&line[30]);

        // Check if the file did not end halfway through the parsing
        if (status == NULL)
        {
            fprintf(stderr, "Error: Unexpected end of the input file\n");
            abort();
        }

        // Check if we are at at a blank line or the end of the file
        status = fgets(line, sizeof(line), input);
        assert(line[0] == '\n' || status == NULL);

        // Clear the line buffer
        memset(line, 0, sizeof(line));
    }

    fclose(input);

    return 0;
}