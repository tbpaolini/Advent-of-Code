#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// Struct to represent the monkeys and the items they are holding
typedef struct Monkey
{
    struct ItemQueue    // A queue of items (first in, last out)
    {
        struct ItemNode *head;  // First item of the queue
        struct ItemNode *tail;  // Last item of the queue
    } items;
    char operator;      // The operator that the monkey will apply to the worry level ('+' or '*')
    uint64_t operand;   // The value that the monkey will add or multiply to the worry level (if this is OLD_VALUE, then the operand is the current worry level itself)
    uint64_t div_test;  // Test if the worry level's result is divisible by this value
    size_t if_true;     // Index of the next monkey if the test passes
    size_t if_false;    // Index of the next monkey if the test fails
    uint64_t activity;  // How many times this monkey has inspected an item
} Monkey;

static const uint64_t OLD_VALUE = UINT64_MAX;   // Use the old value of the worry level during the '+' or '*' operation

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

    // Since the item is outside a queue now, it has no next or previous items
    item->next = NULL;
    item->previous = NULL;

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

// Perform a round of Keep Away on an array of monkeys
// Note: The array is modified in-place.
static void do_round(
    Monkey monkey_array[],  // Array of monkeys
    size_t monkey_amount    // Number of monkeys in the array
)
{
    // Loop through all monkeys
    for (size_t i = 0; i < monkey_amount; i++)
    {
        Monkey *current_monkey = &monkey_array[i];
        ItemNode *current_item = queue_pop(current_monkey);
        
        // Loop through all items with the monkey
        while (current_item)
        {
            // Increment the monkey's activity counter
            current_monkey->activity++;
            
            // Which value to add or multiply to the current worry level
            // (might be either the current worry level or a fixed value)
            uint64_t value;
            if (current_monkey->operand == OLD_VALUE)
            {
                value = current_item->worry_level;
            }
            else
            {
                value = current_monkey->operand;
            }

            // Which operation to perform with the value
            if (current_monkey->operator == '+')
            {
                // Addition
                current_item->worry_level += value;
            }
            else if (current_monkey->operator == '*')
            {
                // Multiplication
                current_item->worry_level *= value;
            }
            else
            {
                fprintf(stderr, "Error: Illegal operation '%c'", current_monkey->operator);
                abort();
            }

            // Monkey plays with the item:
            // The worry level is divided by 3 and then rounded down to the nearest integer
            current_item->worry_level /= 3;

            // Determine which monkey to give the item to
            Monkey *next_monkey;
            if (current_item->worry_level % current_monkey->div_test == 0)
            {
                // Is divisible by the test value
                next_monkey = &monkey_array[current_monkey->if_true];
            }
            else
            {
                // Is not divisible by the test value
                next_monkey = &monkey_array[current_monkey->if_false];
            }

            // Give the item to the next monkey
            queue_add(next_monkey, current_item);

            // Move to the next item
            current_item = queue_pop(current_monkey);
        }
    }
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
    size_t monkey_amount = (line_count + 1) / 7;

    // Array to store the state of each monkey
    Monkey monkeys[monkey_amount];
    memset(monkeys, 0, sizeof(monkeys));

    // Parse the initial state of the monkeys
    for (size_t i = 0; i < monkey_amount; i++)
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

    for (size_t i = 0; i < 20; i++)
    {
        do_round(monkeys, monkey_amount);
    }

    uint64_t top_1, top_2;
    for (size_t i = 0; i < monkey_amount; i++)
    {
        uint64_t activity = monkeys[i].activity;
        if (activity > top_1)
        {
            top_2 = top_1;
            top_1 = activity;
        }
        else if (activity > top_2)
        {
            top_2 = activity;
        }
    }

    uint64_t monkey_business = top_1 * top_2;

    return 0;
}