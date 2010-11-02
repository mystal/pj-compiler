#include "list.h"

#include <stdlib.h>

typedef struct __list_node list_node;

struct __list_node
{
    LIST_T *elem;
    list_node *next;
    list_node *prev;
};

struct __list
{
    list_node *head;
    list_node *tail;
    unsigned int size;
};

list *listCreate()
{
    list *l = (list *) malloc(sizeof(list));
    list_node *sentinelHead = (list_node *) malloc(sizeof(list_node));
    list_node *sentinelTail = (list_node *) malloc(sizeof(list_node));
    sentinelHead->elem = NULL;
    sentinelHead->next = sentinelTail;
    sentinelHead->prev = NULL;
    sentinelTail->elem = NULL;
    sentinelTail->next = NULL;
    sentinelTail->prev = sentinelHead;
    l->head = sentinelHead;
    l->tail = sentinelTail;
    l->size = 0;
    return l;
}

void listAddFront(list *l, LIST_T *elem)
{
    list_node *n = (list_node *) malloc(sizeof(list_node));
    n->elem = elem;
    n->next = l->head->next;
    n->prev = l->head;
    l->head->next->prev = n;
    l->head->next = n;
    l->size++;
}

void listAddBack(list *l, LIST_T *elem)
{
    list_node *n = (list_node *) malloc(sizeof(list_node));
    n->elem = elem;
    n->next = l->tail;
    n->prev = l->tail->prev;
    l->tail->prev->next = n;
    l->tail->prev = n;
    l->size++;
}

LIST_T *listRemoveFront(list *l)
{
    if (l->size == 0)
        return NULL;
    list_node *n = l->head->next;
    LIST_T *ret = n->elem;
    l->head->next = n->next;
    n->next->prev = l->head;
    free(n);
    l->size--;
    return ret;
}

LIST_T *listRemoveBack(list *l)
{
    if (l->size == 0)
        return NULL;
    list_node *n = l->tail->prev;
    LIST_T *ret = n->elem;
    l->tail->prev = n->prev;
    n->prev->next = l->tail;
    free(n);
    l->size--;
    return ret;
}

LIST_T *listGetFront(list *l)
{
    return l->head->next->elem;
}

LIST_T *listGetBack(list *l)
{
    return l->tail->prev->elem;
}

LIST_T *listGet(list *l, unsigned int index)
{
    if (index >= l->size)
        return NULL;
    list_node *n;
    if (index < l->size/2) //Start from head
    {
        n = l->head->next;
        for (int i = 0; i < index; i++)
            n = n->next;
    }
    else //Start from tail
    {
        n = l->tail->prev;
        for (int i = l->size-1; i > index; i--)
            n = n->prev;
    }
    return n->elem;
}

unsigned int listSize(list *l)
{
    return l->size;
}

void listDestroy(list *l)
{
    list_node *n = l->head;
    list_node *temp;
    while (n != NULL)
    {
        temp = n;
        n = temp->next;
        free(temp);
    }
    free(l);
    l = NULL;
}
