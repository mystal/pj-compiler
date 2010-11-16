#pragma once

#define T void

typedef struct __stack stack;

/**
 * Create and return a new stack.
 **/
stack *stackCreate(void);

/**
 * Deallocates memory for the stack and sets its pointer to NULL.
 **/
void stackDestroy(stack *);

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

//TODO create print function
