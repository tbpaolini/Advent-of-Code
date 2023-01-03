#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct MathMonkey
{
    struct MathMonkey* others[2];
    char others_name[2][5];
    double number;
    char name[5];
    char operation;
} MathMonkey;

typedef struct PreffixTree {
    struct PreffixTree* branches[26];
    struct MathMonkey* data;
} PreffixTree;

static PreffixTree* trie_new()
{
    PreffixTree *trie = (PreffixTree*)calloc(1, sizeof(PreffixTree));
    if (!trie)
    {
        fprintf(stderr, "Error: No enough memory\n");
        abort();
    }
    return trie;
}

static void trie_insert(PreffixTree *trie, char *key, MathMonkey *monkey)
{
    if (!key) return;
    
    PreffixTree *node = trie;
    char next_char;
    size_t pos = 0;
    
    while ( next_char = key[pos++] )
    {
        assert(islower(next_char));
        
        const size_t id = next_char - 'a';
        if (!node->branches[id])
        {
            node->branches[id] = trie_new();
        }
        node = node->branches[id];
        
        assert(pos <= 4);
    }
    
    node->data = monkey;
}

static MathMonkey* trie_get(PreffixTree *trie, char *key)
{
    PreffixTree *node = trie;
    char next_char;
    size_t pos = 0;
    
    while ( next_char = key[pos++] )
    {
        assert(islower(next_char));
        
        const size_t id = next_char - 'a';
        if (!node->branches[id]) return NULL;
        node = node->branches[id];
    }

    return node->data;
}

static void trie_destroy(PreffixTree *trie)
{
    PreffixTree *node = trie;

    if (node)
    {
        for (size_t i = 0; i < 26; i++)
        {
            trie_destroy(node->branches[i]);
        }
    }
    
    free(node);
}

static double get_monkey_number(MathMonkey *monkey)
{
    if (!monkey->operation)
    {
        return monkey->number;
    }
    else
    {
        double other_numbers[2] = {
            get_monkey_number(monkey->others[0]),
            get_monkey_number(monkey->others[1])
        };

        switch (monkey->operation)
        {
            case '+':
                return other_numbers[0] + other_numbers[1];
                break;
            
            case '-':
                return other_numbers[0] - other_numbers[1];
                break;
            
            case '/':
                return other_numbers[0] / other_numbers[1];
                break;
            
            case '*':
                return other_numbers[0] * other_numbers[1];
                break;

            default:
                fprintf(stderr, "Error: Invalid operation '%c'\n", monkey->operation);
                abort();
                break;
        }
    }
}

int main(int argc, char * argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[32];

    size_t monkeys_count = 0;

    while (fgets(line, sizeof(line), input)) monkeys_count++;
    rewind(input);

    MathMonkey monkeys[monkeys_count];
    memset(monkeys, 0, sizeof(monkeys));

    PreffixTree *monkey_tree = trie_new();

    for (size_t i = 0; i < monkeys_count; i++)
    {
        fgets(line, sizeof(line), input);
        
        const char delimiters[] = ": ";
        char *token = strtok(line, delimiters);
        
        assert(strlen(token) == 4);
        strcpy(monkeys[i].name, token);
        trie_insert(monkey_tree, token, &monkeys[i]);

        token = strtok(NULL, delimiters);
        if (isdigit(token[0]))
        {
            monkeys[i].number = atof(token);
        }
        else
        {
            assert(strlen(token) == 4);
            strcpy(monkeys[i].others_name[0], token);
            
            token = strtok(NULL, delimiters);
            assert(strlen(token) == 1);
            monkeys[i].operation = token[0];

            token = strtok(NULL, delimiters);
            assert(strlen(token) == 5 && token[4] == '\n');
            token[4] = '\0';
            strcpy(monkeys[i].others_name[1], token);
        }
    }

    fclose(input);

    for (size_t i = 0; i < monkeys_count; i++)
    {
        MathMonkey *monkey = &monkeys[i];
        
        for (size_t j = 0; j < 2; j++)
        {
            monkey->others[j] = trie_get(monkey_tree, monkey->others_name[j]);
        }
    }

    trie_destroy(monkey_tree);
    
    return 0;
}