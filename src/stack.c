#include "stack.h"

#include <stdlib.h>

typedef struct __stack_node stack_node;

struct __stack_node
{
    void *elem;
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
    s->top = NULL;
    s->size = 0;
    return s;
}

void stackPush(stack *s, void *e)
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

void *stackPop(stack *s)
{
    if (s->size == 0)
        return NULL;
    void *e = s->top->elem;
    stack_node *temp = s->top;
    s->top = temp->next;
    free(temp);
    s->size--;
    return e;
}

void *stackPeek(stack *s)
{
    if (s->size == 0)
        return NULL;
    return s->top->elem;
}

unsigned int stackSize(stack *s)
{
    return s->size;
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
