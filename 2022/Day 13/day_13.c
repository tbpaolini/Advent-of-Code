#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>


typedef struct Packet
{
    struct PacketValue *array;
    size_t size;
} Packet;

typedef union PacketWrapper
{
    int64_t integer;
    struct Packet *list;
} PacketWrapper;

typedef struct PacketValue
{
    union PacketWrapper value;
    bool is_list;
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

static Packet* parse_line(char *line)
{
    // Get the number of elements on top level of the list
    size_t length = packet_length(line);
    if (length == 0) return NULL;
    
    // Allocate memory for the top level list
    Packet *packet = calloc(1, sizeof(Packet));
    PacketValue *value_array = (PacketValue*)calloc(length, sizeof(PacketValue));
    if (!value_array || !packet)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }

    // Initialize the list
    *packet = (Packet){
        .array = value_array,
        .size = length
    };
    
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
                    Packet *nested_packet = parse_line(&line[char_index-1]);
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
        packet_array[packet_index++] = (Packet*)parse_line(line);
    }
    
    free(packet_array);
}