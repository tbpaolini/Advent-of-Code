#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// Whether an item of the file system is a file or a directory
typedef enum ItemType
{
    T_NULL,
    T_FILE,
    T_DIRECTORY,
} ItemType;

// Node on the file system (they are linked in a tree structure)
typedef struct ItemNode
{
    char name[64];  // Name of the file or directory
    uint64_t size;  // Total size of this item (recursive)
    ItemType type;  // Type of item (T_FILE or T_DIRECTORY)
    size_t dir_count;           // Amount subdirectories directly on this directory
    struct ItemNode *parent;    // Parent directory of this item
    struct ItemNode *children;  // List of child items (only if this is a directory)
    struct ItemNode *next;      // Next item on the same directory as this
} ItemNode;

// The mode when parsing the puzzle input
typedef enum CurrentMode {
    M_COMMAND,  // Sending a command
    M_LISTING,  // Receiving a directory listing
} CurrentMode;

// Create a new node for the file system
static ItemNode *new_node()
{
    ItemNode *node = (ItemNode*)calloc(1, sizeof(ItemNode));
    if (!node)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return node;
}

// Recursively calculate the combined size of all children of a directory, then set it
static uint64_t set_directory_size(ItemNode *directory)
{
    assert(directory->type == T_DIRECTORY);
    
    // If the size has beed calculated before, just return it
    if (directory->size > 0) return directory->size;
    
    // Get the list of children nodes of the directory
    ItemNode *child = directory->children;
    
    // Loop through all children nodes
    while (child != NULL)
    {
        if (child->type == T_FILE)
        {
            // If it is a file, add the size to the current directory
            directory->size += child->size;
        }
        else
        {
            // If it is another directory, calculate its size then set it to the current directory
            directory->size += set_directory_size(child);
        }

        // Move to the next child
        child = child->next;
    }
    
    return directory->size;
}

// Set the values of an array of directory sizes
// The function returns the inxed after the last added value
static size_t set_dir_size_array(
    ItemNode *directory,    // INPUT: The directory where to start from
    uint64_t *array,        // OUTPUT: The array where to store the sizes
    size_t start_index      // INPUT: The index on the array where to start setting the values
)
{
    ItemNode *node = directory->children;       // Current file or subdirectory
    ItemNode *children[directory->dir_count];   // Array of pointers to the immediate subdirectories
    memset(children, 0, sizeof(children));      // Initialize all values to NULL
    
    size_t node_index = start_index;    // Index of the current file or subdirectory
    size_t child_index = 0;             // Index of the subdirectory inside the directory

    // Loop through all items in the current directory
    while (node != NULL)
    {
        if (node->type == T_DIRECTORY)
        {
            array[node_index++] = node->size;   // Store the subdirectory's size
            children[child_index++] = node;     // Store the subdirectory's pointer
        }
        
        // Move to the next item
        node = node->next;
    }
    
    // Get the sizes of the "grandchildren" of the current directory
    for (size_t i = 0; i < directory->dir_count; i++)
    {
        // We need to continue populating the array from the index we left of
        node_index = set_dir_size_array(children[i], array, node_index);
    }

    // Keep track of the array position during the recursion
    return node_index;
}

// Recursively free the memory of a node and  all of its children
static void destroy_tree(ItemNode *root_node)
{
    ItemNode *current_node = root_node->children;
    while (current_node != NULL)
    {
        ItemNode *next_node = current_node->next;
        if (current_node->type == T_DIRECTORY)
        {
            destroy_tree(current_node);
        }
        else
        {
            free(current_node);
        }
        current_node = next_node;
    }
    free(root_node);
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[64];
    ItemNode *root = new_node();
    ItemNode *current_directory = NULL;
    CurrentMode mode = M_COMMAND;
    root->type = T_DIRECTORY;
    root->name[0] = '/';
    uint64_t file_count = 0;
    uint64_t dir_count = 0;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        size_t line_length = strlen(line);  // Amount of characters in the line
        line[--line_length] = '\0';         // Remove the newline character at the end
        
        if (line[0] == '$') // We are issuing a command
        {
            assert(line_length >= 4);
            mode = M_COMMAND;

            if (strncmp(line + 2, "cd", 2) == 0)
            {
                // Changing directory
                if (strncmp(line + 5, "..", 2) == 0) 
                {
                    // To the parent directory
                    current_directory = current_directory->parent;
                }
                else if (line[5] == '/')
                {
                    // To the root directory
                    current_directory = root;
                }
                else
                {
                    // To a subdirectory
                    current_directory = current_directory->children;
                    char *dir = line + 5;
                    
                    // Check the child directories until the specified name is found
                    while (!(current_directory->type == T_DIRECTORY && strcmp(current_directory->name, dir) == 0))
                    {
                        current_directory = current_directory->next;
                        assert(current_directory != NULL);
                    }
                }
            }
            else if (strncmp(line + 2, "ls", 2) == 0)   // List the files and directories
            {
                mode = M_LISTING;
            }
            else
            {
                // We should never reach this branch
                fprintf(stderr, "Error: Malformatted input\n");
                abort();
            }
        }
        else    // This is a listing of items on the directory
        {
            assert(mode == M_LISTING);
            char *token = strtok(line, " ");
            ItemNode *next_node = new_node();

            if (strcmp(token, "dir") == 0)
            {
                // The new item is a directory
                next_node->type = T_DIRECTORY;
                dir_count++;
                current_directory->dir_count++;
            }
            else
            {
                // The new item is a file
                next_node->type = T_FILE;
                file_count++;
                
                // Get the file size
                assert(isdigit(token[0]));
                next_node->size = atol(token);
            }
            
            // Get the name of the new item
            token = strtok(NULL, " ");
            assert(token != NULL);

            // Store the name
            strncpy(next_node->name, token, sizeof(next_node->name) - 1);

            // Link the new item to its parent directory
            next_node->parent = current_directory;

            // Add the new item to its parent directory
            ItemNode *child = current_directory->children;
            if (child == NULL)
            {
                // The new item is the first child
                current_directory->children = next_node;
            }
            else
            {
                // Add the new item to the end of the list of children
                while (child->next != NULL) {child = child->next;}
                child->next = next_node;
            }
        }
    }

    fclose(input);
    set_directory_size(root);
    
    // Part 1: Get the sum of the directories with size up to 100000
    uint64_t total_size = 0;
    uint64_t dir_sizes[dir_count];              // Array with the sizes of the directories
    memset(dir_sizes, 0, sizeof(dir_sizes));    // Initialize all values to zero
    set_dir_size_array(root, dir_sizes, 0);     // Get the sizes of every directory
    
    // Part 2: Get the smallest folder that should be deleted in order to free enough space
    uint64_t min_folder_size = UINT64_MAX;
    uint64_t free_space = 70000000 - root->size;    // Current free space
    
    // Loop through all the folders
    for (size_t i = 0; i < dir_count; i++)
    {
        // Size of the currend folder
        const uint64_t size = dir_sizes[i];
        if (size <= 100000) total_size += size; // Add the size if below or at the threshold

        // Check if deleting this folder would free enough space
        if (free_space + size >= 30000000)
        {
            // Check if the current folder has the smallest size so far
            min_folder_size = size < min_folder_size ? size : min_folder_size;
        }
        
    }
    
    printf("Part 1: %lu\n", total_size);
    printf("Part 2: %lu\n", min_folder_size);

    destroy_tree(root);
}

