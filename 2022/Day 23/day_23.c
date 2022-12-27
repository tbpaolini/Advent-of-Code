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
    struct ElfCoord target; // Coordinate the elf is considering to move into
    struct ElfNode *next;   // Next entry on the linked list
    size_t count;           // How many elves in this coordinate
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
// Note: This function does not resolve collisions.
static inline ElfNode* ht_node(ElfTable *table, const ElfCoord coorddinate)
{
    const uint8_t *key = (uint8_t*)(&coorddinate);
    const uint64_t index = hash_fnv1a(key, sizeof(ElfCoord)) % table->capacity;
    return &table->data[index];
}

// Insert a coordinate to the hash table
static void ht_insert(ElfTable *table, const ElfCoord coordinate)
{
    // Increment the total coordinates counter
    table->count++;
    
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
            node->next->count++;
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
        if (coord_equal(node->next->coord, coordinate)) return node->next->count;

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
    if (!node->count) return;
    ElfNode *previous_node = NULL;

    if (coord_equal(node->coord, coordinate))
    {
        // If we are at the first node of the list, the node is not freed
        // because it is stored directly on the table.
        // Instead, we just flag the space as 'unused'.
        node->count--;
        table->count--;

        // If there are no more elves here and there is a linked list on this node,
        // move list's head to the next node
        if (node->count == 0 && node->next)
        {
            ElfNode *old_node = node->next;
            *node = *node->next;
            free(old_node);
        }
        
        return;
    }
    
    previous_node = node;
    node = node->next;

    // Navigate through the list until the coordinate is found
    while (node)
    {
        // If the node is occupied and its coordinate matches the searched value
        if (coord_equal(node->coord, coordinate))
        {
            node->count--;
            table->count--;
            if (node->count > 0) return;    // Do not delete the node if there are still elves here
            
            if (node->next)
            {
                // Link the previous node to the next
                previous_node->next = node->next;
            }
            else
            {
                // We are at the tail of the list
                previous_node->next = NULL;
            }

            // Free the removed node
            free(node);
            return;
        }

        // Move to the next node
        previous_node = node;
        node = node->next;
    }

    assert(false);
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

    int64_t test = do_round(elves, 10);
    // 5550 - too high
    // 4013 - too high

    ht_free(elves);

    return 0;
}