#pragma once

#define T void

typedef struct __list list;

/**
 * Create and return a new list.
 **/
list *listCreate(void);

/**
 * Add the given item to the front of the list.
 **/
void listAddFront(list *, T *);

/**
 * Add the given item to the back of the list.
 **/
void listAddBack(list *, T *);

/**
 * Remove and return the first item of the list. Returns NULL if list is empty.
 **/
T *listRemoveFront(list *);

/**
 * Remove and return the last item of the list. Returns NULL if list is empty.
 **/
T *listRemoveBack(list *);

/**
 * Get the item at the front of the list. Returns NULL if the list is empty.
 **/
T *listGetFront(list *);

/**
 * Get the item at the back of the list. Returns NULL if the list is empty.
 **/
T *listGetBack(list *);

/**
 * Get the item at the given index. Returns NULL if the index is invalid.
 **/
T *listGet(list *, unsigned int);

/**
 * Returns the size of the list.
 **/
unsigned int listSize(list *);

/**
 * Deallocates memory for the list and sets its pointer to NULL.
 **/
void listDestroy(list *);
