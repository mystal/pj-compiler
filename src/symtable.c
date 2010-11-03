#include "symtable.h"

#include <stdbool.h>
#include <stdlib.h>

#include "bst.h"
//#include "pjbuiltins.h"
#include "directive.h"
#include "list.h"
#include "str.h"

struct __symtable
{
    list *blockList;
};

typedef struct __block
{
    string *name;
    bst *symbols;
} block;

/* cmp_func for the bsts in blocks */
int symbolCompareTo(void *, void *);

symtable *symCreate()
{
    symtable *st = (symtable *) malloc(sizeof(symtable));
    st->blockList = listCreate();
    //TODO Initialize list with pj builtin procedures
    return st;
}

void symEnterBlock(symtable *st, string *name)
{
    block *b = (block *) malloc(sizeof(block));
    b->name = name;
    b->symbols = bstCreate(symbolCompareTo);
    listAddBack(st->blockList, b);
}

void symExitBlock(symtable *st)
{
    block *b = listRemoveBack(st->blockList);
    stringFree(b->name);
    free(b->name);
    //TODO cleanup memory for symbols in bst
    bstDestroy(b->symbols);
    free(b);
}

bool symAddSymbol(symtable *st, symbol *s)
{
    if (symLookup(st, s->name) != NULL)
        return false;
    return true;
}

symbol *symLookup(symtable *st, string *name)
{
    //TODO create iterator for lists?
    for (int i = 0; i < listSize(st->blockList); i++)
    {
    }
    return NULL;
}

void symDestroy(symtable *st)
{
    //TODO check that the size is 1
    //TODO free memory for block 0
    listDestroy(st->blockList);
    st = NULL;
}

int symbolCompareTo(void *v1, void *v2)
{
    symbol *s1 = (symbol *) v1;
    symbol *s2 = (symbol *) v2;
    return symbolCompareTo(s1->name, s2->name);
}
