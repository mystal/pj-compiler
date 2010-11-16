#include "stack.h"

#include <stdlib.h>

typedef struct __stack_node stack_node;

struct __stack_node
{
    T *elem;
    stack_node *next;
};

struct __stack
{
    stack_node *top;
    unsigned int size;
};

stack *stackCreate()
{
    stack *s = (stack *) malloc(sizeof(stack));
    stack_node *sentinel = (stack_node *) malloc(sizeof(stack_node));
    sentinel->elem = NULL;
    sentinel->next = NULL;
    s->top = sentinel;
    s->size = 0;
    return s;
}

void stackDestroy(stack *s)
{
    stack_node *n = s->top;
    stack_node *temp;
    while (n != NULL)
    {
        temp = n;
        n = temp->next;
        free(temp);
    }
    free(s);
    s = NULL;
}

void stackPush(stack *s, T *e)
{
    stack_node *temp = (stack_node *) malloc(sizeof(stack_node));
    temp->elem = e;
    temp->next = s->top;
    s->top = temp;
    s->size++;
}

T *stackPop(stack *s)
{
    if (s->size == 0)
        return NULL;
    T *e = s->top->elem;
    stack_node *temp = s->top;
    s->top = temp->next;
    free(temp);
    s->size--;
    return e;
}

T *stackPeek(stack *s)
{
    return s->top->elem;
}

unsigned int stackSize(stack *s)
{
    return s->size;
}
