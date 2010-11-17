#include "symtable.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bst.h"
#include "directive.h"
#include "list.h"
#include "pjlang.h"
#include "str.h"
#include "symbol.h"

struct __symtable
{
    list *blockList;
};

typedef struct __block
{
    string *name;
    bst *symbols;
    unsigned int nextLoc;
} block;

/* cmp_func for symbols */
int bstCompareSymbol(void *, void *);

/* print_func for symbols */
void bstPrintSymbol(void *);

/* del_func for symbols */
void bstDelSymbol(void *);

symtable *stCreate()
{
    symtable *st = (symtable *) malloc(sizeof(symtable));
    st->blockList = listCreate();
    //Add block0
    block *b = (block *) malloc(sizeof(block));
    b->name = stringCreate();
    stringAppendCharArray(b->name, "block0", 6*sizeof(char));
    b->symbols = bstCreate(bstCompareSymbol);
    b->nextLoc = 1;
    listAddBack(st->blockList, b);
    //Initialize block0 with PJ's builtin procedures and input/output files
    for (unsigned int i = 0; i < builtin_num; i++)
    {
        char *builtinName = pjbuiltinString(i);
        string *name = stringCreate();
        stringAppendCharArray(name, builtinName, strlen(builtinName));
        symbol *sym = symbolCreate(name);
        stringDestroy(name);
        symbolSetBuiltin(sym, i);
        stAddSymbol(st, sym);
    }
    return st;
}

void stEnterBlock(symtable *st, string *name)
{
    block *b = (block *) malloc(sizeof(block));
    b->name = name;
    b->symbols = bstCreate(bstCompareSymbol);
    b->nextLoc = 1;
    listAddBack(st->blockList, b);
}

void stExitBlock(symtable *st)
{
    block *b = (block *) listRemoveBack(st->blockList);
    bstDestroy(b->symbols, bstDelSymbol); //Cleanup memory for symbols in bst
    free(b);
}

unsigned int stGetLevel(symtable *st)
{
    return listSize(st->blockList) - 1;
}

bool stAddSymbol(symtable *st, symbol *sym)
{
    block *b;
    if (listSize(st->blockList) > 1)
    {
        b = (block *) listGetFront(st->blockList);
        if (bstContains(b->symbols, sym))
            return false;
    }
    b = (block *) listGetBack(st->blockList);
    //Fail if already in block
    if (bstContains(b->symbols, sym))
        return false;
    sym_type type = symbolGetType(sym);
    switch (type)
    {
        case symt_var:
            symVarSetLocation(sym, b->nextLoc++);
            break;
        case symt_array:
            symArraySetLocation(sym, b->nextLoc);
            b->nextLoc += symArrayGetRange(sym);
            break;
        case symt_proc:
            symProcSetLocation(sym, b->nextLoc++);
            break;
        default:
            break;
    }
    bstInsert(b->symbols, sym);
    return true;
}

symbol *stLookup(symtable *st, string *name, unsigned int *level)
{
    symbol *ret;
    //TODO: create iterator for lists?
    for (unsigned int i = listSize(st->blockList)-1; i >= 0; i--)
    {
        symbol *test = symbolCreate(name);
        ret = (symbol *) bstGet(((block *) listGet(st->blockList, i))->symbols, test);
        symbolDestroy(test);
        if (ret != NULL)
        {
            if (level != NULL)
                *level = i;
            return ret;
        }
    }
    return NULL;
}

void stPrintBlocks(symtable *st, unsigned int n)
{
    int i = listSize(st->blockList) - 1;
    while (i >= 0 && n > 0)
    {
        block *b = (block *) listGet(st->blockList, i);
        fprintf(stdout, "Block %d: %.*s\n", i, stringGetLength(b->name),
                stringGetBuffer(b->name));
        bstPrint(b->symbols, bstPrintSymbol);
        i--;
        n--;
    }
}

void stDestroy(symtable *st)
{
    if (listSize(st->blockList) != 1)
        fprintf(stderr, "Symbol table block leak!\n");
    block *b = (block *) listRemoveBack(st->blockList);
    stringDestroy(b->name);
    bstDestroy(b->symbols, bstDelSymbol); //Cleanup memory for symbols in bst
    free(b);
    listDestroy(st->blockList);
    free(st);
    st = NULL;
}

int bstCompareSymbol(void *v1, void *v2)
{
    symbol *s1 = (symbol *) v1;
    symbol *s2 = (symbol *) v2;
    return stringCompareString(symbolGetName(s1), symbolGetName(s2));
}

void bstPrintSymbol(void *v)
{
    symbol *sym = (symbol *) v;
    symbolPrint(sym);
}

void bstDelSymbol(void *v)
{
    symbol *sym = (symbol *) v;
    symbolDestroy(sym);
}
