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
    double number;
    char name[5];
    char operation;
} MathMonkey;

typedef struct PreffixTree {
    char branches[26];
    struct MathMonkey* data;
    bool is_terminator;
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

static void trie_insert(PreffixTree *trie, char *key)
{
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
        
        assert(pos < 4);
    }
    
    node->is_terminator = true;
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
        if (!node->branches[id]) break;
        node = node->branches[id];
        
        assert(pos < 4);
    }

    if (node->is_terminator) return node->data;

    return NULL;
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

int main(int argc, char * argv)
{
    FILE *input = fopen("input.txt", "rt");
    char line[32];

    size_t monkeys_count = 0;

    while (fgets(line, sizeof(line), input)) monkeys_count++;

    MathMonkey monkeys[monkeys_count];
    memset(monkeys, 0, sizeof(monkeys));

    MathMonkey *root = NULL;
    MathMonkey *humn = NULL;

    while (fgets(line, sizeof(line), input))
    {
        /* code */
    }

    fclose(input);
    
    return 0;
}