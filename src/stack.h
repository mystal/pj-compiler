#pragma once

#include <stdbool.h>

#define T void

typedef struct __stack stack;

stack *stackCreate();

void stackPush(stack *, T *);

T *stackPop(stack *);

T *stackPeek(stack *);

unsigned int stackSize(stack *);

void stackDestroy(stack *);
