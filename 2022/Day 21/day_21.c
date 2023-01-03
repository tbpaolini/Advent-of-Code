#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct MathMonkey
{
    struct MathMonkey *others[2];
    double number;
    char name[5];
    char operation;
} MathMonkey;

int main(int argc, char * argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[32];

    while (fgets(line, sizeof(line), input))
    {
        /* code */
    }

    fclose(input);
    
    return 0;
}