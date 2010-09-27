#pragma once

#include <stdbool.h>

#include "exprsymbol.h"

typedef struct __slr_stack_pair
{
    unsigned int state : 8;
    symbol sym;
    bool isTerm;
} slr_stack_pair;

typedef slr_stack_pair T;

typedef struct __stack stack;

stack *stackCreate();

void stackPush(stack *, T *);

T *stackPop(stack *);

T *stackPeek(stack *);

unsigned int stackSize(stack *);

void stackDestroy(stack *);
