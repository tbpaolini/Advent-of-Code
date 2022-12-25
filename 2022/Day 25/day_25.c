#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// Check if a character represents a valid snafu digit
static inline bool is_snafu(char digit)
{
    return (digit == '=' || digit == '-' || digit == '0' || digit == '1' || digit == '2');
}

// Convert a SNAFU string to decimal
static int64_t snafu_to_decimal(char *snafu)
{
    // Amount of digits of the SNAFU string
    const int64_t snafu_len = strlen(snafu);

    int64_t decimal = 0;            // Accumulator for the result
    int64_t snafu_magnitude = 1;    // Magnitude of the current SNAFU digit

    // Loop through the digits from right to left
    for (int64_t i = snafu_len - 1; i >= 0; i--)
    {
        // SNAFU digit == snigit :-)
        char snigit = snafu[i];
        
        // Value of the snigit in decimal
        int64_t value;
        switch (snigit)
        {
            case '=':
                value = -2;
                break;
            
            case '-':
                value = -1;
                break;
            
            case '0':
                value = 0;
                break;
            
            case '1':
                value = 1;
                break;
            
            case '2':
                value = 2;
                break;
            
            case '\n':  // Ignore the newline at the end
                if (i == snafu_len-1) continue;
            
            default:
                fprintf(stderr, "Error: Invalid SNAFU number '%s'\n", snafu);
                abort();
                break;
        }

        // Add the value to the result
        decimal += value * snafu_magnitude;

        // Increase the order of magnitude of the next value
        snafu_magnitude *= 5;
    }
    
    // Return the decimal representation
    return decimal;
}

// Convert a decimal number to a SNAFU string
// It is necessary to provide an output buffer for the string, and the buffer's size.
// The function returns the amount of digits in the SNAFU.
// Pass NULL as the buffer in order to just get the maximum amount of digits that the output can has.
static size_t decimal_to_snafu(
    int64_t decimal,    // Decimal number to be converted
    char *snafu,        // Output buffer
    size_t buffer_size  // Size of the output buffer
)
{
    // Lookup table for converting a decimal value to a SNAFU digit
    static const char SNAFU_BASE[] = {'=', '-', '0', '1', '2'};
    static const char *SNAFU_TABLE = &SNAFU_BASE[0] + 2;
    
    if (decimal < 0)
    {
        fprintf(stderr, "Error: The decimal number cannot be negative.");
        abort();
    }
    
    // Check if the buffer is big enough
    int64_t temp_val = decimal;
    size_t count = 0;   // Estimation of the amount of digits (will never be smaller than the actual amount)
    
    do
    {
        temp_val /= 5;
        count++;
    }
    while (temp_val > 1);

    if (snafu == NULL) return count;
    
    if (buffer_size < count + 1)
    {
        fprintf(stderr, "Error: Insufficient buffer size of '%lu' for decimal '%ld'\n", count, decimal);
        abort();
    }

    // Writing position on the buffer
    // (we are going to write the string backwards)
    int64_t pos = count+1;
    snafu[pos--] = '\0';    // Null terminator of the string

    // Convert to SNAFU digits
    while (decimal > 0)
    {
        // Get the value of the current digit
        // The order of values is: 2; 1; 0; -1; -2
        int64_t value = decimal % 5;
        if (value >= 3) value -= 5;
        /* Note:
            If the calculated value would be 4, then it becomes -1.
            If it would be 3, it becomes -2.
        */
        
        // Look up for the corresponding SNAFU digit, and append it to the left of the string.
        char snigit = SNAFU_TABLE[value];
        assert(pos >= 0);
        snafu[pos--] = snigit;

        // Discount the value from the decimal,
        // then divide it by 5 to get to the next SNAFU digit
        decimal -= value;
        decimal /= 5;
    }
    
    // The estimation of the amount of digits can be either the actual amount,
    // or one more than that. If it is the later, we just move the string buffer
    // by one character to the left.
    if (pos == 0)
    {
        memmove(&snafu[0], &snafu[1], count);
        count--;
    }
    
    return count;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[32];

    int64_t total = 0;

    // Parse the input line by line
    while (fgets(line, sizeof(line), input))
    {
        // Convert the SNAFU value to decimal, and add it to the total
        total += snafu_to_decimal(line);
    }

    fclose(input);

    // Convert the total back to SNAFU
    char solution[128];
    decimal_to_snafu(total, solution, sizeof(solution));

    printf("Part 1: %s\n", solution);
    printf("Part 2: Good luck on getting all stars from previous days :-)\n");

    printf("\nMerry Christmas!\n");
    
    return 0;
}