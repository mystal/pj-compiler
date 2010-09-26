#include "stack.h"

#include <stdlib.h>

struct __stack_node;
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

void stackInit(stack *s)
{
    s->top = NULL;
    s->size = 0;
}

void stackPush(stack *s, T *e)
{
    if (s->size == 0)
    {
        s->top = (stack_node *) malloc(sizeof(stack_node));
        s->top->elem = e;
        s->top->next = NULL;
    }
    else
    {
        stack_node *temp = (stack_node *) malloc(sizeof(stack_node));
        temp->elem = e;
        temp->next = s->top;
        s->top = temp;
    }
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
    if (s->size == 0)
        return NULL;
    return s->top->elem;
}

unsigned int stackGetSize(stack *s)
{
    return s->size;
}

void stackClean(stack *s)
{
    stack_node *n = s->top;
    stack_node *temp;
    while (n != NULL)
    {
        temp = n;
        n = temp->next;
        free(temp);
    }
    s->top = NULL;
    s->size = 0;
}
