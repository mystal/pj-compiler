#pragma once

#include <stdbool.h>

#include "token.h"

#define BST_T void

/* Function pointer types used by bst */
typedef int (*cmp_func)(BST_T *, BST_T *);
typedef void (*print_func)(BST_T *);
typedef void (*del_func)(BST_T *);

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
 * Returns the given item if it is in the tree, NULL otherwise.
 **/
BST_T *bstGet(bst *, BST_T *);

/**
 * Returns the size of the bst.
 **/
unsigned int bstSize(bst *);

/**
 * Prints the tree using the provided function to print each element.
 **/
void bstPrint(bst *, print_func);

/**
 * Deallocates memory for the bst and sets its pointer to NULL. If the given
 * function is not NULL, the stored data's memory is also deallocated using it.
 **/
void bstDestroy(bst *, del_func);
