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
typedef struct ElfTable {
    size_t size;             // Size in bytes of the data section of the table
    size_t capacity;         // How many buckets the table has
    size_t count;            // Amount of coordinates currently stored on the table
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

// Create a new hash table
// Note: Should be freed with 'ht_free()'
static ElfTable* ht_new(size_t capacity)
{
    const size_t table_size = sizeof(ElfTable) + (capacity * sizeof(ElfNode));
    ElfTable *table = (ElfTable*)calloc(1, table_size);
    table->size = table_size - sizeof(ElfTable);
    table->capacity = capacity;
    return table;
}

// Given a coordinate, retrieve the bucket on the hash table where the coordinate should go
// This function can optionally insert the coordinate on the table.
static ElfNode* ht_find_node(ElfTable *table, const ElfCoord coorddinate, bool insert)
{
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
        return node;
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
    if (node == NULL)
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
}

// Check how many elves in coordinate of the hash table
static size_t ht_contains(ElfTable *table, const ElfCoord coordinate)
{
    ElfNode *node = ht_find_node(table, coordinate, false);
    if (node == NULL) return 0;
    else return node->count;
}

// Remove a coordinate from the hash table
static void ht_remove(ElfTable *table, const ElfCoord coordinate)
{
    ElfNode *node = ht_find_node(table, coordinate, false);
    if (node == NULL || node->count == 0) return;
    
    node->count--;
    table->count--;
    if (node->count > 0) return;

    if (node->previous == NULL)
    {
        if (node->next)
        {
            node->next->previous = NULL;
            if (node->next->next) node->next->next->previous = node;
            ElfNode *old_node = node->next;
            *node = *node->next;
            free(old_node);
        }
        else
        {
            memset(node, 0, sizeof(node));
        }
    }
    else
    {
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

static void print_map(ElfTable *elves)
{
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
    
    const int64_t width  = max.x - min.x + 1;
    const int64_t height = max.y - min.y + 1;

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
static int64_t do_round(ElfTable *elves, size_t amount)
{
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

    static size_t decision[4][3] = {
        {7, 0, 1},  // Check north
        {3, 4, 5},  // Check south
        {5, 6, 7},  // Check west
        {1, 2, 3},  // Check east
    };

    static ElfCoord movement[4] = {
        { 0, -1},   // Move north
        { 0, +1},   // Move south
        {-1,  0},   // Move west
        {+1,  0},   // Move east
    };

    #ifdef _DEBUG
    print_map(elves);
    #endif
    ElfTable *tentative_movements = ht_new(elves->capacity);

    // Temporary array to store the movements that the elves are going to make
    struct ChangeQueue {ElfCoord old; ElfCoord new;} changes[elves->count];
    size_t change_id = 0;

    for (size_t round = 0; round < amount; round++)
    {
        // First half of the round: consider the 4 cardinal directions
        for (size_t i = 0; i < elves->capacity; i++)
        {
            ElfNode *elf = &elves->data[i];
            if (!elf->count) continue;
            ElfCoord target_coord;

            bool has_elf[8];
            bool any_elf = false;
            for (size_t dir = 0; dir < 8; dir++)
            {
                const ElfCoord coord = {
                    elf->coord.x + directions[dir].x,
                    elf->coord.y + directions[dir].y
                };
                
                has_elf[dir] = ht_contains(elves, coord);
                if (has_elf[dir]) any_elf = true;
            }

            if (!any_elf) continue;

            for (size_t face = 0; face < 4; face++)
            {
                size_t my_dir = (elves->direction + face) % 4;
                bool can_move = false;

                for (size_t pos = 0; pos < 3; pos++)
                {
                    const size_t other_dir = decision[my_dir][pos];
                    if (has_elf[other_dir]) break;
                    if (pos == 2) can_move = true;
                }
                
                if (can_move)
                {
                    const ElfCoord target_offset = movement[my_dir];
                    const ElfCoord target_coord = (ElfCoord){
                        elf->coord.x + target_offset.x,
                        elf->coord.y + target_offset.y
                    };

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
            const size_t target_count = ht_contains(tentative_movements, changes[i].new);
            
            if (target_count == 1)
            {
                ht_remove(elves, changes[i].old);
                ht_insert(elves, changes[i].new);
            }
        }

        #ifdef _DEBUG
        print_map(elves);
        #endif
        memset(tentative_movements->data, 0, tentative_movements->size);
        elves->direction = (elves->direction + 1) % 4;
        change_id = 0;
    }
    
    ht_free(tentative_movements);

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
    
    const int64_t width  = max.x - min.x + 1;
    const int64_t height = max.y - min.y + 1;
    return (width * height) - elves->count;
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

    ElfNode *test_dump[elves->count];
    ht_dump(elves, test_dump, elves->count);

    ElfCoord coords[elves->count];
    for (size_t i = 0; i < elves->count; i++)
    {
        size_t j = (i + 1) % elves->count;
        coords[i] = test_dump[j]->coord;
    }
    
    size_t count = elves->count;
    for (size_t i = 0; i < count; i++)
    {
        assert(ht_contains(elves, coords[i]) == 1);
        ht_remove(elves, coords[i]);
    }
    free(elves);

    // int64_t test = do_round(elves, 10);

    // ht_free(elves);

    return 0;
}