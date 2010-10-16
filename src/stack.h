#pragma once

#include <stdbool.h>

typedef struct __stack stack;

stack *stackCreate();

void stackPush(stack *, void *);

void *stackPop(stack *);

void *stackPeek(stack *);

unsigned int stackSize(stack *);

void stackDestroy(stack *);
