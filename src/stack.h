#pragma once

#define T void

typedef struct __stack stack;

/**
 * Create and return a new stack.
 **/
stack *stackCreate(void);

/**
 * Push the given item onto the top of the stack.
 **/
void stackPush(stack *, T *);

/**
 * Pop and return the top item from the stack. Returns NULL if stack is empty.
 **/
T *stackPop(stack *);

/**
 * Peek at the top item of the stack. Returns NULL if stack is empty.
 **/
T *stackPeek(stack *);

/**
 * Returns the size of the stack.
 **/
unsigned int stackSize(stack *);

/**
 * Deallocates memory for the stack and sets its pointer to NULL.
 **/
void stackDestroy(stack *);
