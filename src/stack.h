#pragma once

typedef int T;

struct __stack;
typedef struct __stack stack;

void stackInit(stack *);

void stackPush(stack *, T *);

T *stackPop(stack *);

T *stackPeek(stack *);

unsigned int stackGetSize(stack *);

void stackClean(stack *);
