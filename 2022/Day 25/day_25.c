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
// Pass NULL as the buffer in order to just get the amount of digits, without actually converting.
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
    size_t count = 0;
    
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
    int64_t pos = count;
    snafu[pos--] = '\0';

    while (decimal > 0)
    {
        int64_t value = decimal % 5;
        if (value >= 3) value -= 5;
        
        char snigit = SNAFU_TABLE[value];
        assert(pos >= 0);
        snafu[pos--] = snigit;

        decimal -= value;
        decimal /= 5;
    }
    
    return count;
}

int main(int argc, char **argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[32];

    int64_t total = 0;

    while (fgets(line, sizeof(line), input))
    {
        total += snafu_to_decimal(line);
    }

    fclose(input);

    char out[100];
    decimal_to_snafu(total, out, 100);
    
    return 0;
}