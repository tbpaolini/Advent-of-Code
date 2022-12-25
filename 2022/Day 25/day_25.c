#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

static const char SNAFU_BASE[] = {'=', '-', '0', '1', '2'};
static const char *SNAFU = &SNAFU_BASE[0] + 2;

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

    int64_t decimal = 0;
    int64_t snafu_magnitude = 1;

    // Loop through the digits from right to left
    for (int64_t i = snafu_len - 1; i >= 0; i--)
    {
        // SNAFU digit == snigit :-)
        char snigit = snafu[i];
        assert(is_snafu(snigit));
        
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

int main(int argc, char **argv)
{
    snafu_to_decimal("1=11-2");
    return 0;
}