#pragma once

#include <stdbool.h>

#include "token.h"

//#define T void

typedef struct __bst bst;

/**
 * Creates and returns a new bst.
 **/
bst *bstCreate();

/**
 * Inserts the given item into the tree. Returns true if successful,
 * false otherwise.
 **/
bool bstInsert(bst *, token_kind);

/**
 * Removes the given item from the tree. Returns true if successful,
 * false otherwise.
 **/
bool bstRemove(bst *, token_kind);

/**
 * Returns true if the given item is in the tree, false otherwise.
 **/
bool bstContains(bst *, token_kind);

/**
 * Returns the size of the bst.
 **/
unsigned int bstSize(bst *);

/**
 * Deallocates memory for the bst and sets its pointer to NULL.
 **/
void bstDestroy(bst *);
