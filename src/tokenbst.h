#pragma once

#include <stdbool.h>

#include "token.h"

#define TOKBST_T token_kind

typedef struct __tokenbst tokenbst;

/**
 * Creates and returns a new tokenbst.
 **/
tokenbst *tokenbstCreate(void);

/**
 * Inserts the given item into the tree. Returns true if successful,
 * false otherwise.
 **/
bool tokenbstInsert(tokenbst *, TOKBST_T);

/**
 * Removes the given item from the tree. Returns true if successful,
 * false otherwise.
 **/
bool tokenbstRemove(tokenbst *, TOKBST_T);

/**
 * Returns true if the given item is in the tree, false otherwise.
 **/
bool tokenbstContains(tokenbst *, TOKBST_T);

/**
 * Returns the size of the tokenbst.
 **/
unsigned int tokenbstSize(tokenbst *);

/**
 * Deallocates memory for the tokenbst and sets its pointer to NULL.
 **/
void tokenbstDestroy(tokenbst *);
