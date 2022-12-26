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

// An entry of the hash set
typedef struct ElfNode {
    struct ElfCoord coord;  // (x, y) coordinates
    struct ElfNode *next;   // Next entry on the linked list
    bool used;              // Whether this entry has a coordinate
} ElfNode;

// A hash set for storing the coordinates of the elves
// Collisions are resolved by separate chaining (a linked list on each bucket).
typedef struct ElfSet {
    size_t capacity;         // How many buckets the set has
    struct ElfNode data[];   // Array of buckets
} ElfSet;

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

// Given a coordinate, retrieve the bucket on the hash set where the coordinate should go
// Note: This function does not resolve collisions.
static inline ElfNode* hs_node(ElfSet *set, const ElfCoord coorddinate)
{
    const uint8_t *key = (uint8_t*)(&coorddinate);
    const uint64_t index = hash_fnv1a(key, sizeof(ElfCoord)) % set->capacity;
    return &set->data[index];
}

// Create a new hash set
// Note: Should be freed with 'hs_free()'
static ElfSet* hs_new(size_t capacity)
{
    const size_t set_size = sizeof(ElfSet) + (capacity * sizeof(ElfNode));
    ElfSet *set = (ElfSet*)calloc(1, set_size);
    set->capacity = capacity;
    return set;
}

// Insert a coordinate to the hash set
static void hs_insert(ElfSet *set, const ElfCoord coordinate)
{
    // The bucket where the coordinate should be
    ElfNode *node = hs_node(set, coordinate);

    // Stop if the coordinate is at the first entry of the list
    if (node->used && coord_equal(node->coord, coordinate)) return;
    
    // Navigate until the end of the list
    while (node->next)
    {
        // Stop if the coordinate is at the list's next position
        if (coord_equal(node->next->coord, coordinate)) return;

        // Move to the next position
        node = node->next;
    }
    
    // If the current node already has a coordinate, create a new node
    if (node->used)
    {
        node->next = (ElfNode*)calloc(1, sizeof(ElfNode));
        node = node->next;
    }

    // Store the coordinate
    node->coord = coordinate;
    node->used = true;
}

// Check if a coordinate is in the hash set
static bool hs_contains(ElfSet *set, const ElfCoord coordinate)
{
    // The bucket where the coordinate should be
    ElfNode *node = hs_node(set, coordinate);
    
    // Check if the coordinate is at the beginning of the list
    if (node->used && coord_equal(node->coord, coordinate))
    {
        return true;
    }
    
    // Navigate through the list until the coordinate is found
    while (node->next)
    {
        // Check if the coordinate is at the list's next position
        if (coord_equal(node->next->coord, coordinate)) return true;

        // Move to the next position
        node = node->next;
    }

    // If the coordinate was not found
    return false;
}

// Remove a coordinate from the hash set
static void hs_remove(ElfSet *set, const ElfCoord coordinate)
{
    ElfNode *node = hs_node(set, coordinate);
    ElfNode *previous_node = NULL;

    // Navigate through the list until the coordinate is found
    while (node->next)
    {
        // If the node is occupied and its coordinate matches the searched value
        if (node->used && coord_equal(node->coord, coordinate))
        {
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
                    // because it is stored directly on the set
                    node->used = false;
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
                    // because it is stored directly on the set.
                    // Instead, we just flag the space as 'unused'.
                    node->used = false;
                    return;
                }
            }
        }

        // Move to the next node
        previous_node = node;
        node = node->next;
    }
}

// Free the memory used by a hash set
static void hs_free(ElfSet *set)
{
    for (size_t i = 0; i < set->capacity; i++)
    {
        ElfNode *node = set->data[i].next;
        while (node)
        {
            ElfNode *next_node = node->next;
            free(node);
            node = next_node;
        }        
    }
    free(set);
}

int main(int argc, char **argv)
{

    FILE *input = fopen("input.txt", "rt");
    char cur_char;                  // Current character on the input file
    ElfCoord cur_coord = {0, 0};    // Current coordinate on the map

    ElfSet *elves = hs_new(4096);   // Hash set to store the elves' coordinates on the map

    while ( (cur_char = fgetc(input)) != EOF )
    {
        assert(cur_char == '.' || cur_char == '#' || cur_char == '\n');
        
        if (cur_char == '#')
        {
            hs_insert(elves, cur_coord);
            assert(hs_contains(elves, cur_coord));
        }
        else
        {
            assert(!hs_contains(elves, cur_coord));
        }

        if (cur_char == '\n')
        {
            cur_coord.x = 0;
            cur_coord.y++;
        }
        else
        {
            cur_coord.x++;
        }
    }

    fclose(input);

    hs_free(elves);

    return 0;
}