#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// The elf's coordinates
typedef struct ElfCoord {
    int64_t x;
    int64_t y;
} ElfCoord;

// An entry of the hash table
typedef struct ElfNode {
    struct ElfCoord coord;  // (x, y) coordinates
    struct ElfNode *next;   // Next entry on the linked list
    size_t count;           // How many elves in this coordinate
} ElfNode;

// A hash table for storing the coordinates of the elves
// Collisions are resolved by separate chaining (a linked list on each bucket).
typedef struct ElfTable {
    size_t capacity;         // How many buckets the table has
    size_t direction;        // Which direction the elves are considering
    struct ElfNode data[];   // Array of buckets
} ElfTable;

// Hashing function: FNV-1a
// Details: http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1a
//          https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
static uint64_t hash_fnv1a(const uint8_t *key, const size_t length)
{
    // Parameters of the FNV-1a hash
    static const uint64_t fnv_offset_64 = 0xcbf29ce484222325;   // Starting value of the hash
    static const uint64_t fnv_prime_64  = 0x00000100000001b3;   // Multiplies the hash on each step of the loop

    uint64_t hash = fnv_offset_64;      // Initialize the hash
    
    // Loop through the bytes on the key
    for (size_t i = 0; i < length; i++)
    {
        const uint8_t current_byte = key[i];    // Move to the next byte
        hash ^= current_byte;   // Bitwise XOR with the byte value
        hash *= fnv_prime_64;   // Multiply by the FNV prime
    }
    
    return hash;
}

// Check if two coordinates are equal
static inline bool coord_equal(const ElfCoord coord1, const ElfCoord coord2)
{
    return memcmp(&coord1, &coord2, sizeof(ElfCoord)) == 0;
}

// Given a coordinate, retrieve the bucket on the hash table where the coordinate should go
// Note: This function does not resolve collisions.
static inline ElfNode* ht_node(ElfTable *table, const ElfCoord coorddinate)
{
    const uint8_t *key = (uint8_t*)(&coorddinate);
    const uint64_t index = hash_fnv1a(key, sizeof(ElfCoord)) % table->capacity;
    return &table->data[index];
}

// Create a new hash table
// Note: Should be freed with 'ht_free()'
static ElfTable* ht_new(size_t capacity)
{
    const size_t set_size = sizeof(ElfTable) + (capacity * sizeof(ElfNode));
    ElfTable *table = (ElfTable*)calloc(1, set_size);
    table->capacity = capacity;
    return table;
}

// Insert a coordinate to the hash table
static void ht_insert(ElfTable *table, const ElfCoord coordinate)
{
    // The bucket where the coordinate should be
    ElfNode *node = ht_node(table, coordinate);

    // Stop if the coordinate is at the first entry of the list
    if (node->count && coord_equal(node->coord, coordinate))
    {
        node->count++;
        return;
    }
    
    // Navigate until the end of the list
    while (node->next)
    {
        // Stop if the coordinate is at the list's next position
        if (coord_equal(node->next->coord, coordinate))
        {
            node->count++;
            return;
        }

        // Move to the next position
        node = node->next;
    }
    
    // If the current node already has a coordinate, create a new node
    if (node->count)
    {
        node->next = (ElfNode*)calloc(1, sizeof(ElfNode));
        node = node->next;
    }

    // Store the coordinate
    node->coord = coordinate;
    node->count = 1;
}

// Check how many elves in coordinate of the hash table
static size_t ht_contains(ElfTable *table, const ElfCoord coordinate)
{
    // The bucket where the coordinate should be
    ElfNode *node = ht_node(table, coordinate);
    
    // Check if the coordinate is at the beginning of the list
    if (node->count && coord_equal(node->coord, coordinate))
    {
        return node->count;
    }
    
    // Navigate through the list until the coordinate is found
    while (node->next)
    {
        // Check if the coordinate is at the list's next position
        if (coord_equal(node->next->coord, coordinate)) return node->count;

        // Move to the next position
        node = node->next;
    }

    // If the coordinate was not found
    return 0;
}

// Remove a coordinate from the hash table
static void ht_remove(ElfTable *table, const ElfCoord coordinate)
{
    ElfNode *node = ht_node(table, coordinate);
    ElfNode *previous_node = NULL;

    if (node->count && coord_equal(node->coord, coordinate))
    {
        // If we are at the first node of the list, the node is not freed
        // because it is stored directly on the table.
        // Instead, we just flag the space as 'unused'.
        node->count--;
        return;
    }
    else
    {
        node = node->next;
    }

    // Navigate through the list until the coordinate is found
    do /* while (node->next); */
    {
        // If the node is occupied and its coordinate matches the searched value
        if (coord_equal(node->coord, coordinate))
        {
            node->count--;
            if (node->count) return;    // Do not delete the node if there are still elves here
            
            if (node->next)
            {
                // If there is a node after the current, copy its contents to the current node
                ElfNode *old_node = node->next;
                node->coord = old_node->coord;
                node->next = old_node->next;

                if (previous_node)
                {
                    // Link the previous node to the next
                    previous_node->next = old_node->next;
                }
                else
                {
                    // If we are at the first node of the list, the node is not freed
                    // because it is stored directly on the table
                    node->count = false;
                }

                // Free the unused node
                free(old_node);
                return;
            }
            else
            {
                if (previous_node)
                {
                    // If this is the last (but not the only) node on the list,
                    // just remove the node from the list
                    previous_node->next = NULL;
                    free(node);
                    return;
                }
                else
                {
                    // If we are at the first node of the list, the node is not freed
                    // because it is stored directly on the table.
                    // Instead, we just flag the space as 'unused'.
                    node->count = false;
                    return;
                }
            }
        }

        // Move to the next node
        previous_node = node;
        node = node->next;
    
    } while (node->next);
}

// Free the memory used by a hash table
static void ht_free(ElfTable *table)
{
    for (size_t i = 0; i < table->capacity; i++)
    {
        ElfNode *node = table->data[i].next;
        while (node)
        {
            ElfNode *next_node = node->next;
            free(node);
            node = next_node;
        }        
    }
    free(table);
}

// Perform a given amount of movement rounds
static void do_round(ElfTable *elves, size_t amount)
{
    static ElfCoord directions[8] = {
        { 0, -1},   // North
        {+1, -1},   // Northeast
        {+1,  0},   // East
        {+1, +1},   // Southeast
        { 0, +1},   // South
        {-1, +1},   // Southwest
        {-1,  0},   // West
        {-1, -1},   // Northwest
    };

    static size_t decision[4][3] = {
        {7, 0, 1},  // Move north
        {3, 4, 5},  // Move south
        {5, 6, 7},  // Move west
        {1, 2, 3},  // Move east
    };

    for (size_t round = 0; round < amount; round++)
    {
        for (size_t i = 0; i < elves->capacity; i++)
        {
            const ElfNode *elf = &elves->data[i];
            if (!elf->count) continue;

            bool has_elf[8];
            for (size_t dir = 0; dir < 8; dir++)
            {
                const ElfCoord coord = {
                    elf->coord.x + directions[dir].x,
                    elf->coord.y + directions[dir].y
                };
                
                has_elf[dir] = ht_contains(elves, coord);
            }
        }
    }
    
}

int main(int argc, char **argv)
{

    FILE *input = fopen("input.txt", "rt");
    char cur_char;                  // Current character on the input file
    ElfCoord cur_coord = {0, 0};    // Current coordinate on the map

    ElfTable *elves = ht_new(4096);   // Hash table to store the elves' coordinates on the map

    // Parse the elves' positions from the imput
    while ( (cur_char = fgetc(input)) != EOF )
    {
        assert(cur_char == '.' || cur_char == '#' || cur_char == '\n');
        
        if (cur_char == '#')
        {
            // If there is an elf here, add the coordinate to the table
            ht_insert(elves, cur_coord);
            assert(ht_contains(elves, cur_coord));
        }
        else
        {
            assert(!ht_contains(elves, cur_coord));
        }

        if (cur_char == '\n')
        {
            // If at a line break, move to the beginning of the next row
            cur_coord.x = 0;
            cur_coord.y++;
        }
        else
        {
            // Move to the next column, if not at a line break.
            cur_coord.x++;
        }
    }

    fclose(input);

    ht_free(elves);

    return 0;
}