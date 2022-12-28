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
    struct ElfCoord coord;      // (x, y) coordinates
    size_t count;               // How many elves in this coordinate
    struct ElfNode *next;       // Next entry on the linked list
    struct ElfNode *previous;   // Previous entry on the linked list
} ElfNode;

// A hash table for storing the coordinates of the elves
// Collisions are resolved by separate chaining (a linked list on each bucket).
// (a bucket is a region on the table that can store data)
typedef struct ElfTable {
    size_t size;             // Size in bytes of the whole struct (including the data section)
    size_t capacity;         // How many buckets the table has
    size_t count;            // Amount of coordinates currently stored on the table
    size_t free_space;       // Amount of free spaces in the smallest rectangle that contains all elves
    size_t direction;        // Which direction the elves are considering
    struct ElfNode data[];   // Array of buckets
} ElfTable;
/* Notes:
 *  There is a linked list on each bucket of the data section.
 *  The first node of that list is stored directly on the table,
 *  in order to avoid memory fragmentation.
 */

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

// Create a new hash table
// Note: Should be freed with 'ht_free()'
static ElfTable* ht_new(size_t capacity)
{
    // Allocate a memory block for the hash table, initialized to zero
    const size_t table_size = sizeof(ElfTable) + (capacity * sizeof(ElfNode));
    ElfTable *table = (ElfTable*)calloc(1, table_size);

    // Initial parameters of the table
    table->size = table_size;
    table->capacity = capacity;
    return table;
}

// Given a coordinate, retrieve the bucket on the hash table where the coordinate should go
// This function can optionally insert the coordinate on the table.
static ElfNode* ht_find_node(ElfTable *table, const ElfCoord coorddinate, bool insert)
{
    // Calculate the bucket where the coordinate should go
    const uint8_t *key = (uint8_t*)(&coorddinate);
    const uint64_t index = hash_fnv1a(key, sizeof(ElfCoord)) % table->capacity;
    ElfNode *node = &table->data[index];

    // Base case: the bucket has no other coordinates stored (no collision)
    if (node->count == 0 || coord_equal(node->coord, coorddinate))
    {
        // When on insert mode: store the coordinate
        if (insert)
        {
            if (node->count == 0)
            {
                // Create the entry if the bucket is empty
                *node = (ElfNode){
                    .coord = coorddinate,
                    .count = 1,
                    .next = NULL,
                    .previous = NULL
                };
            }
            else
            {
                // If the same coordinate is already there, increment its counter
                node->count++;
            }
            
            // Global counter for the table
            table->count++;
        }
        
        // Return a pointer to where the coordinate is on the table
        // If the coordinate is not found and not being inserted, then NULL is returned.
        if (node->count == 0) return NULL;
        else return node;
    }

    // General case: collision resolution
    ElfNode *previous_node = node;
    node = node->next;
    while (node)
    {
        // Check if the same coordinate is already stored on the node
        if (coord_equal(node->coord, coorddinate))
        {
            // When on insert mode: increment the counters
            if (insert)
            {
                node->count++;
                table->count++;
            }
            
            break;
        }

        // Move to the next node on the linked list
        previous_node = node;
        node = node->next;
    }

    // When on insert mode: if the coordinate was not found, append it to the linked list
    if (insert && node == NULL)
    {
        // Allocate and initialize the new node
        node = (ElfNode*)malloc(sizeof(ElfNode));
        *node = (ElfNode){
            .coord = coorddinate,
            .count = 1,
            .next = NULL,
            .previous = previous_node
        };

        // Link the prevous node to the new node
        previous_node->next = node;

        // Increment the global counter
        table->count++;
    }
    
    // Return a pointer to where the coordinate is on the table
    // If the coordinate is not found and not being inserted, then NULL is returned.
    return node;
}

// Insert a coordinate to the hash table
static void ht_insert(ElfTable *table, const ElfCoord coordinate)
{
    ht_find_node(table, coordinate, true);
    // Note: when the third argument of that function is 'true',
    //       the coordinate is inserted on the table.
}

// Check how many elves in coordinate of the hash table
static size_t ht_contains(ElfTable *table, const ElfCoord coordinate)
{
    // Retrieve the coordinate from the table, and check its counter of elves
    ElfNode *node = ht_find_node(table, coordinate, false);
    if (node == NULL) return 0;
    else return node->count;
}

// Remove a coordinate from the hash table
static void ht_remove(ElfTable *table, const ElfCoord coordinate)
{
    // Retrieve the coordinate to be removed from the table
    ElfNode *node = ht_find_node(table, coordinate, false);
    if (node == NULL || node->count == 0) return;
    
    // Decrement the counters, if the coordinate exists on the table
    node->count--;
    table->count--;
    
    // If there are still elves there, do not remove the coordinate
    if (node->count > 0) return;

    // Check if the coordinate is stored directly on the table
    if (node->previous == NULL)
    {
        // If the node is directly on the table, the next node of the linked
        // list will be copied to the table, then that node will be freed
        if (node->next)
        {
            // Link the next node to the one after it and remove its 'previous' link
            // (there are no 'previous' nodes when the node is the first on the list)
            node->next->previous = NULL;
            if (node->next->next) node->next->next->previous = node;

            // Copy the contents of the next node to the table, then free that node
            ElfNode *old_node = node->next;
            *node = *node->next;
            free(old_node);
        }
        else
        {
            // If there are no further nodes, just clear the node's memory region
            memset(node, 0, sizeof(node));
        }
    }
    else
    {
        // If the node is not at the beginning of the list, link the previous
        // and next nodes to each other, then free the node
        ElfNode *old_node = node;
        if (node->next) node->next->previous = node->previous;
        node->previous->next = node->next;
        free(old_node);
    }
}

// Store in a buffer the pointers to hash table's elements
// Note: The size is the amount of pointers the buffer can hold.
static void ht_dump(ElfTable *table, ElfNode **output_buffer, size_t buffer_size)
{
    // Position on the buffer
    size_t pos = 0;
    static bool printed_error = false;
    
    // Iterate over the buckets on the table
    for (size_t i = 0; i < table->capacity; i++)
    {
        // Get the current bucket on the table
        // If the count is zero, there are no elements on the bucket
        ElfNode *node = &table->data[i];
        if (node->count == 0) continue;

        // Iterate over the linked list on the bucket
        while (node)
        {
            // Stop if we arrived to the end of the buffer
            if (pos == buffer_size)
            {
                if (!printed_error)
                {
                    // This error message is shown only once during the program's lifetime,
                    // in order to avoid spamming the terminal.
                    fprintf(stderr, "Error: Insufficient buffer size for dumping the hash table\n");
                    printed_error = true;
                }
                return;
            }

            // Add the current node to the buffer and move to the next on the list
            output_buffer[pos++] = node;
            node = node->next;
        }
    }
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

// Print the current elves positions to the terminal
// (only if the _DEBUG macro is defined)
static void print_map(ElfTable *elves)
{
    // Determine the maximum and minimum coordinates
    ElfCoord max = {INT64_MIN, INT64_MIN};
    ElfCoord min = {INT64_MAX, INT64_MAX};
    for (size_t i = 0; i < elves->capacity; i++)
    {
        const ElfNode *elf = &elves->data[i];
        if (!elf->count) continue;

        const int64_t x = elf->coord.x;
        const int64_t y = elf->coord.y;

        if (x > max.x) max.x = x;
        if (y > max.y) max.y = y;
        if (x < min.x) min.x = x;
        if (y < min.y) min.y = y;
    }
    
    // Calculate the width and height of the map, based on the max and min coordinates
    const int64_t width  = max.x - min.x + 1;
    const int64_t height = max.y - min.y + 1;

    // Add the elves to the map
    bool map[height][width];
    memset(map, 0, sizeof(map));

    for (size_t i = 0; i < elves->capacity; i++)
    {
        const ElfNode *elf = &elves->data[i];
        if (!elf->count) continue;

        const int64_t x = elf->coord.x;
        const int64_t y = elf->coord.y;

        map[y-min.y][x-min.x] = true;
    }

    // Print the map's positions to the terminal
    for (int64_t y = 0; y < height; y++)
    {
        for (int64_t x = 0; x < width; x++)
        {
            putchar(map[y][x] ? '#' : '.');
        }
        putchar('\n');
    }
    putchar('\n');
}

// Perform a given amount of movement rounds
// Function returns true if any elf has moved in the round, false otherwise.
static bool do_round(ElfTable *elves, size_t amount)
{
    // The 8 spots adjascent to an elf
    static ElfCoord directions[8] = {
        { 0, -1},   // 0: North
        {+1, -1},   // 1: Northeast
        {+1,  0},   // 2: East
        {+1, +1},   // 3: Southeast
        { 0, +1},   // 4: South
        {-1, +1},   // 5: Southwest
        {-1,  0},   // 6: West
        {-1, -1},   // 7: Northwest
    };

    // The spots on the directions where an elf is checking for other elves
    // (3 spots on each of the 4 cardinal directions)
    static size_t decision[4][3] = {
        {7, 0, 1},  // Check north
        {3, 4, 5},  // Check south
        {5, 6, 7},  // Check west
        {1, 2, 3},  // Check east
    };

    // Offsets of the coordinate where an elf is trying to move to
    static ElfCoord movement[4] = {
        { 0, -1},   // Move north
        { 0, +1},   // Move south
        {-1,  0},   // Move west
        {+1,  0},   // Move east
    };

    #ifdef _DEBUG
    print_map(elves);
    #endif

    // Whether any elf has moved
    bool elf_has_moved = false;

    // Temporary array to store the movements that the elves are going to make
    struct ChangeQueue {ElfCoord old; ElfCoord new;} changes[elves->count];
    size_t change_id = 0;

    // Total amount of elves
    // THIS VALUE SHOULD NEVER CHANGE IF THEIR POSITIONS ARE BEING CALCULATED PROPERLY
    const size_t elves_count = elves->count;

    for (size_t round = 0; round < amount; round++)
    {
        assert(elves->count == elves_count);
        
        // Hash table to store the positions where the elves are considering to move to
        ElfTable *tentative_movements = ht_new(elves->capacity);

        // Get all the elements of the hash table that stores the current coordinates
        ElfNode *elves_array[elves_count];
        ht_dump(elves, elves_array, elves_count);
        
        // First half of the round: consider the 4 cardinal directions
        for (size_t i = 0; i < elves_count; i++)
        {
            // Get the current elf
            ElfNode *elf = elves_array[i];
            assert(elf->count == 1);
            
            // The coordinate the elf is considering to move to
            ElfCoord target_coord;

            bool has_elf[8];        // Which of the 8 directions has an elf there
            bool any_elf = false;   // If there is at least one elf in those directions
            
            // Iterate over the 8 directions
            for (size_t dir = 0; dir < 8; dir++)
            {
                // Coordinate of the spot on the direction
                const ElfCoord coord = {
                    elf->coord.x + directions[dir].x,
                    elf->coord.y + directions[dir].y
                };
                
                // Check if there is an elf on that spot
                has_elf[dir] = ht_contains(elves, coord);
                if (has_elf[dir]) any_elf = true;
            }

            // If there isn't any adjascent elf, then the current elf skips the round
            if (!any_elf) continue;

            // Check if there is any elf adjascent to the 4 cardinal directions (north, south, west, east)
            for (size_t face = 0; face < 4; face++)
            {
                // Direction that the elf is considering to move
                size_t my_dir = (elves->direction + face) % 4;
                bool can_move = false;

                // Check the 3 adjascent spots in that direction
                for (size_t pos = 0; pos < 3; pos++)
                {
                    const size_t other_dir = decision[my_dir][pos];
                    if (has_elf[other_dir]) break;
                    if (pos == 2) can_move = true;
                }
                
                // If there are no other elves on those spots, movement will be attempted
                if (can_move)
                {
                    // Get the coordinate the elf is trying to move to
                    const ElfCoord target_offset = movement[my_dir];
                    const ElfCoord target_coord = (ElfCoord){
                        elf->coord.x + target_offset.x,
                        elf->coord.y + target_offset.y
                    };

                    // Enqueue the movement attempt
                    changes[change_id++] = (struct ChangeQueue){
                        .old = elf->coord,
                        .new = target_coord
                    };
                    ht_insert(tentative_movements, target_coord);
                    
                    break;
                }
            }
        }

        // Second half of the round: try moving to the chosen destination
        for (size_t i = 0; i < change_id; i++)
        {
            // How many elves are trying to move to the considered coordinate
            const size_t target_count = ht_contains(tentative_movements, changes[i].new);
            
            // Only perform the movement if exactly one elf is considering to move there 
            if (target_count == 1)
            {
                ht_remove(elves, changes[i].old);
                ht_insert(elves, changes[i].new);
            }

            // Flag that a movement has happened
            elf_has_moved = true;
        }

        #ifdef _DEBUG
        print_map(elves);
        #endif
        
        // The next direction that the elves are going to consider
        elves->direction = (elves->direction + 1) % 4;

        // Reset the data structures that keep track of the movements that the elves considered
        change_id = 0;
        ht_free(tentative_movements);
    }

    // Get all elements from the hash table of coordinates
    ElfNode *elves_array[elves_count];
    ht_dump(elves, elves_array, elves_count);
    
    // Maximum and minimum coordinates
    ElfCoord max = {INT64_MIN, INT64_MIN};
    ElfCoord min = {INT64_MAX, INT64_MAX};
    
    // Iterate over all coordinates
    for (size_t i = 0; i < elves_count; i++)
    {
        // Get the elf
        const ElfNode *elf = elves_array[i];
        assert(elf->count == 1);

        // Elf's coordinates
        const int64_t x = elf->coord.x;
        const int64_t y = elf->coord.y;

        // Update the maximum and minimum values
        if (x > max.x) max.x = x;
        if (y > max.y) max.y = y;
        if (x < min.x) min.x = x;
        if (y < min.y) min.y = y;
    }
    
    // Calculate and return the amount of free space
    const int64_t width  = max.x - min.x + 1;
    const int64_t height = max.y - min.y + 1;
    return (width * height) - elves_count;
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
            assert(ht_contains(elves, cur_coord) == 1);
        }
        else
        {
            assert(ht_contains(elves, cur_coord) == 0);
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

    int64_t test = do_round(elves, 10);

    ht_free(elves);

    return 0;
}