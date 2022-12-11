#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct Monkey
{
    struct ItemQueue    // A queue of items (first in, last out)
    {
        struct ItemNode *head;  // First item of the queue
        struct ItemNode *tail;  // Last item of the queue
    } items;
    uint64_t activity;  // How many times the monkey inspected an item
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

    return 0;
}