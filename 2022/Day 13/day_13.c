#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>


// Main struct to store the values of a packer
typedef struct Packet
{
    struct PacketValue *array;  // Array of the packet's values
    size_t size;                // Amount of elements on the array
} Packet;

// Wrapper for a value in a packet
// Can store either an integer directly, or a pointer to another packet (in case of nested packets)
typedef union PacketWrapper
{
    int64_t integer;        // Integer value
    struct Packet *list;    // Nested packet
} PacketWrapper;

// A value inside a packet
typedef struct PacketValue
{
    union PacketWrapper value;  // An integer value or a nested packet
    bool is_list;               // If true, the value is a nested packet. Otherwise, an integer.
} PacketValue;

// Calculate the length
static size_t packet_length(char *line)
{
    // Return 0 if this is an empty line
    if (line[0] == '\n' || line[0] == '\0') return 0;
    assert(line[0] == '[');
    
    int64_t depth = -1;     // Current depth on the nested list (0 = top level list, -1 = outside the list)
    size_t index = 0;       // Index on the string
    size_t count = 0;       // Count of elements at the top level of the list
    char cur_char = '\0';   // Current character on the list

    // Read the string until the end of the top level list
    do
    {
        // Move to the next character
        cur_char = line[index++];

        switch (cur_char)
        {
            case '[':
                if (depth == 0) count += 1; // Count the element if at the top level
                depth += 1;                 // Move down a depth
                break;
            
            case ']':
                depth -= 1; // Move up a depth
                break;
            
            case ',':
                /* Do nothing */
                break;
            
            default:
                if (isdigit(cur_char))
                {
                    if (depth == 0) count += 1;     // Count the element if at the top level
                    while (isdigit(line[index+1]))
                    {
                        cur_char = line[index++];   // Move until the end of the integer
                    }
                }
                else
                {
                    // On a valid input file, this error should never be reached
                    fprintf(stderr, "Error: Malformatted input\n");
                    abort();
                }
                break;
        }

    } while (depth > -1 && cur_char != '\0');

    // Return the count of elements
    if (depth == -1)
    {
        return count;
    }
    else
    {
        // The string ended before the list was closed
        fprintf(stderr, "Error: Unbalanced brackets\n");
        abort();
    }
}

// Parse a packet from a line of the input file
static Packet* packet_parse(char *line)
{
    // Get the number of elements on top level of the list
    size_t length = packet_length(line);
    
    // Allocate memory for the top level list
    Packet *packet = calloc(1, sizeof(Packet));
    PacketValue *value_array = length > 0 ? (PacketValue*)calloc(length, sizeof(PacketValue)) : NULL;
    if (!packet || (!value_array && length > 0))
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    // Initialize the list
    *packet = (Packet){
        .array = value_array,
        .size = length
    };

    // Return an packet with length zero, if it contains no elements
    if (length == 0) return packet;
    
    int64_t depth = -1;     // Current depth on the nested list
    size_t char_index = 0;  // Index of the current character
    size_t list_index = 0;  // Index of the current value on the list

    char cur_char = line[char_index++];

    // Loop through all characters on the string
    while (cur_char != '\0' && cur_char != '\n')
    {
        switch (cur_char)
        {
            case '[':
                // Move down a depth on the list
                depth += 1;
                
                // If we were at the top level, this means we found a nested list there
                if (depth == 1)
                {
                    // Parse the nested list, then add it to its parent
                    Packet *nested_packet = packet_parse(&line[char_index-1]);
                    value_array[list_index++] = (PacketValue){
                        .value.list = nested_packet,
                        .is_list = true
                    };
                }

                break;
            
            case ']':
                // Move up a depth on the list
                depth -= 1;

                // // If we moved to outside the list, this means the parsing has finished
                if (depth == -1) return packet;
                
                break;
            
            case ',':
                /* Do nothing */
                break;
            
            default:
                // A digit should be found here, if it is not any of the other checked characters
                if (isdigit(cur_char))
                {
                    // If we are at the top level, then we have found an integer value there
                    if (depth == 0)
                    {
                        // Convert the string to integer, then add it to the list
                        int64_t integer = atol(&line[char_index-1]);
                        value_array[list_index++] = (PacketValue){
                            .value.integer = integer,
                            .is_list = false
                        };
                    }

                    // Skip the remaining digit characters
                    while (isdigit(line[char_index]))
                    {
                        cur_char = line[char_index++];
                    }
                }

                break;
        }
        
        // Move to the next character
        cur_char = line[char_index++];
    }

    // On a valid input file, this error should never be reached
    fprintf(stderr, "Error: Malformatted input\n");
    abort();
}

// Compare two packets and return:
// +1: if packet_1 >  packet_2
//  0: if packet_1 == packet_2
// -1: if packet_1 <  packet_2
static int packet_compare(Packet *packet_1, Packet *packet_2)
{
    int result = 0;

    // Check the minimum length among the two packets
    size_t min_size = packet_1->size < packet_2->size ? packet_1->size : packet_2->size;

    // Allocate memory on the stack for the temporary list
    // (used to convert an integer into a list, when the compared types are different)
    static Packet temp_packet;
    static PacketValue temp_value;
    
    // Loop through the elements pairwise until at least one list run out of elements
    for (size_t i = 0; i < min_size; i++)
    {
        
        // The pair of values (one from each packet)
        PacketValue *value_1 = &packet_1->array[i];
        PacketValue *value_2 = &packet_2->array[i];

        // How many of the elements are lists
        int comparison_type = (int)(value_1->is_list) + (int)(value_2->is_list);

        switch (comparison_type)
        {
            case 0: // Both elements are integers
                const int64_t int_1 = value_1->value.integer;
                const int64_t int_2 = value_2->value.integer;
                if (int_1 == int_2) continue;   // Directly compare the values
                return int_1 < int_2 ? -1 : 1;
                break;
            
            case 1: // One element is integer and the other a list
                PacketValue *list_value;
                PacketValue *int_value;
                
                // Check the type of each one
                if (value_1->is_list)
                {
                    list_value = value_1;
                    int_value  = value_2;
                }
                else
                {
                    list_value = value_2;
                    int_value  = value_1;
                }
                
                // Turn the integer into a list of one element
                temp_value = (PacketValue){
                    .value.integer = int_value->value.integer,
                    .is_list = false
                };

                temp_packet = (Packet){
                    .array = &temp_value,
                    .size = 1
                };

                // Retry the comparison with the integer as a list
                if (list_value == value_1)
                {
                    result = packet_compare(list_value->value.list, &temp_packet);
                }
                else
                {
                    result = packet_compare(&temp_packet, list_value->value.list);
                }
                
                if (result != 0) return result;
                break;
            
            case 2: // Both elements are lists
                Packet *list_1 = value_1->value.list;
                Packet *list_2 = value_2->value.list;
                result = packet_compare(list_1, list_2);    // Recursively compare the lists
                if (result != 0) return result;
                break;
        }
    }

    // If we ran out of elements on a list before a inequality was found,
    // then the list size is the "tiebreaker"
    if (packet_1->size == packet_2->size )
    {
        return 0;
    }
    else if (packet_1->size < packet_2->size)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[256];
    size_t packet_count = 0;

    while (fgets(line, sizeof(line), input))
    {
        if (line[0] == '[') packet_count++;
    }

    rewind(input);

    Packet **packet_array = (Packet**)calloc(packet_count, sizeof(Packet*));
    if (!packet_array)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    size_t packet_index = 0;
    while (fgets(line, sizeof(line), input))
    {
        if (line[0] == '\n') continue;
        packet_array[packet_index++] = (Packet*)packet_parse(line);
    }

    fclose(input);

    size_t count = 0;
    for (size_t i = 0; i < packet_count; i += 2)
    {
        int result = packet_compare(packet_array[i], packet_array[i+1]);
        if (result == -1) count += (i / 2) + 1;
    }
    
    printf("%lu\n", count);
    free(packet_array);
}