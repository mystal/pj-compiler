#pragma once

#include <stdbool.h>

#include "pjlang.h"
#include "str.h"
#include "symbol.h"

typedef struct __symtable symtable;

/**
 * Create and return a new symtable.
 **/
symtable *stCreate(void);

/**
 * Create and enter a new block with the given name.
 **/
void stEnterBlock(symtable *, string *);

/**
 * Exit and destroy the top-most block.
 **/
void stExitBlock(symtable *);

/**
 * Returns the current block level, with the top-most being 1.
 **/
unsigned int stGetLevel(symtable *);

/**
 * Add the given symbol to the current block. Returns true if successful,
 * otherwise returns false, indicating that a symbol with the same name
 * already exists in some block.
 **/
bool stAddSymbol(symtable *, symbol *);

/**
 * Checks the symbol table for a symbol with the given name. Returns a pointer
 * to the symbol found if it is in the table, otherwise returns NULL.
 **/
symbol *stLookup(symtable *, string *, unsigned int *);

/**
 * Prints up to the given number of blocks in the symbol table, starting from
 * the top-most block.
 **/
void stPrintBlocks(symtable *, unsigned int);

/**
 * Destroys the given symbol table and sets the pointer to NULL.
 **/
void stDestroy(symtable *);
