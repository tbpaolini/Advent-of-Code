#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// Calculate the priority of an item
static uint64_t get_priority(char item)
{
    if isupper(item)
    {
        return item - 'A' + 27;
    }
    else if (islower(item))
    {
        return item - 'a' + 1;
    }
    else
    {
        return 0;
    }
}

// Helper function to get the priority of the common item in the rucksack
static uint64_t item_priority(uint64_t rucksack)
{
    // Assert that there are any items
    assert(rucksack > 0);
    
    uint64_t common_priority = 0;       // The priority of the item in common
    uint64_t last_bit = rucksack & 1UL; // Least significant bit (LSB) of the bitmask
    
    // Keep shifting the mask to the right until the LSB is 1
    while (last_bit != 1)
    {
        rucksack >>= 1;
        common_priority++;
        last_bit = rucksack & 1UL;
    }
    
    // Assert that there is only one item in common
    assert(rucksack == 1);

    return common_priority;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[128];
    
    uint64_t compartment[2] = {0, 0};
    uint64_t elf[3] = {0, 0, 0};
    size_t elf_count = 0;
    
    uint64_t total_priority_p1 = 0; // Part 1's answer
    uint64_t total_priority_p2 = 0; // Part 2's answer
    
    // Read the file line by line until the end
    while (fgets(line, sizeof(line), input) != NULL)
    {
        size_t count = strlen(line);    // Amount of characters on the string
        line[count-1] = '\0';           // Remove the newline at the end
        count -= 1;
        
        size_t middle = count / 2;          // Index of the character on the middle of the string
        size_t elf_index = elf_count % 3;   // Index of the elf inside the group
        elf_count++;

        // Loop through all characters on the string
        for (size_t i = 0; i < count; i++)
        {
            char item = line[i];
            uint64_t priority = get_priority(item); // Get the priority value of the item
            uint64_t item_mask = 1UL << priority;   // Bitmask that represents this specific item
            
            // Add the item to a compartment on the rucksack
            if (i < middle)
            {
                // First half of the string
                compartment[0] |= item_mask;
            }
            else
            {
                // Second half of the string
                compartment[1] |= item_mask;
            }

            // Add the item to an elf in the group
            elf[elf_index] |= item_mask;
        }
        
        // Check the items in common of the compartments
        uint64_t rucksack = compartment[0] & compartment[1];
        total_priority_p1 += item_priority(rucksack);   // Add the priority of the common item
        
        // Reset the compartments
        compartment[0] = 0;
        compartment[1] = 0;

        // Check the 3 elves in the group
        // Note: once we get to index 2, that means the group is full
        if (elf_index == 2)
        {
            // Check the items in common between elves
            uint64_t group = elf[0] & elf[1] & elf[2];
            total_priority_p2 += item_priority(group);  // Add the priority of the common item

            // Reset the group
            elf[0] = 0;
            elf[1] = 0;
            elf[2] = 0;
        }
    }

    fclose(input);

    printf("Part 1: %lu total priority\n", total_priority_p1);
    printf("Part 2: %lu total priority\n", total_priority_p2);
}