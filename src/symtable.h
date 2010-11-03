#pragma once

#include <stdbool.h>

#include "pjtype.h"
#include "str.h"
#include "symbol.h"

typedef struct __symtable symtable;

/**
 * Create and return a new symtable.
 **/
symtable *symCreate(void);

/**
 * Create and enter a new block with the given name.
 **/
void symEnterBlock(symtable *, string *);

/**
 * Exit and destroy the top-most block.
 **/
void symExitBlock(symtable *);

/**
 * Add the given symbol to the current block. Returns true if successful,
 * otherwise returns false, indicating that a symbol with the same name
 * already exists in some block.
 **/
bool symAddSymbol(symtable *, symbol *);

/**
 * Checks the symbol table for a symbol with the given name. Returns a pointer
 * to the symbol found if it is in the table, otherwise returns NULL.
 **/
symbol *symLookup(symtable *, string *);

/**
 * Destroys the given symbol table and sets the pointer to NULL.
 **/
void symDestroy(symtable *);
