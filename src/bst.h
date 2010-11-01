#pragma once

#include <stdbool.h>

#include "token.h"

#define T token_kind

typedef struct __bst bst;

/**
 * Creates and returns a new bst.
 **/
bst *bstCreate(void);

/**
 * Inserts the given item into the tree. Returns true if successful,
 * false otherwise.
 **/
bool bstInsert(bst *, T);

/**
 * Removes the given item from the tree. Returns true if successful,
 * false otherwise.
 **/
bool bstRemove(bst *, T);

/**
 * Returns true if the given item is in the tree, false otherwise.
 **/
bool bstContains(bst *, T);

/**
 * Returns the size of the bst.
 **/
unsigned int bstSize(bst *);

/**
 * Deallocates memory for the bst and sets its pointer to NULL.
 **/
void bstDestroy(bst *);
