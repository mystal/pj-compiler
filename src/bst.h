#pragma once

#include <stdbool.h>

#include "token.h"

#define BST_T void

/*  */
typedef int (*cmp_func)(BST_T *, BST_T *);

typedef struct __bst bst;

/**
 * Creates and returns a new bst.
 **/
bst *bstCreate(cmp_func);

/**
 * Inserts the given item into the tree. Returns true if successful,
 * false otherwise.
 **/
bool bstInsert(bst *, BST_T *);

/**
 * Removes the given item from the tree. Returns true if successful,
 * false otherwise.
 **/
bool bstRemove(bst *, BST_T *);

/**
 * Returns true if the given item is in the tree, false otherwise.
 **/
bool bstContains(bst *, BST_T *);

/**
 * Returns the size of the bst.
 **/
unsigned int bstSize(bst *);

/**
 * Deallocates memory for the bst and sets its pointer to NULL.
 **/
void bstDestroy(bst *);
