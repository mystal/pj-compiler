#include "rbtree.h"

#include <stdbool.h>
#include <stdlib.h>

typedef enum __rbcolor
{
    black,
    red
} rbcolor;

typedef struct __rb_node rb_node;

struct __rb_node
{
    T *elem;
    rbcolor color;
    rb_node *left;
    rb_node *right;
};

struct __rbtree
{
    rb_node *root;
    unsigned int size;
    //equals eq;
    compareTo cmp;
};

/* Private helper functions */
bool rbtreeInsertHelper(rb_node *, T *);
bool rbtreeRemoveHelper(rb_node *, T *);
bool isRed(rb_node *);

rbtree *rbtreeCreate(compareTo cmp)
{
    rbtree *t = (rbtree *) malloc(sizeof(rbtree));
    t->root = NULL;
    t->size = 0;
    t->cmp = cmp;
    return t;
}

bool rbtreeInsert(rbtree *t, T *e)
{
    if (t->size == 0)
    {
        rb_node *n = (rb_node *) malloc(sizeof(rb_node));
        n->elem = e;
        n->color = black;
        n->left = n->right = NULL;
        t->root = n;
        t->size++;
        return true;
    }
    rb_node *prev = NULL;
    rb_node *cur = t->root;
    while (cur != NULL)
    {
        int c = t->cmp(e, cur->elem);
        if (c == 0)
            return false;
        else
        {
            prev = cur;
            if (c < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
    }
    rb_node *n = (rb_node *) malloc(sizeof(rb_node));
    n->elem = e;
    n->color = red;
    n->left = n->right = NULL;
    if (t->cmp(e, cur->elem))
        cur = cur->left;
    else
        cur = cur->right;
    t->size++;
    return true;
}

bool rbtreeRemove(rbtree *t, T *e)
{
    if (t->size == 0)
        return false;
    if (t->size == 1)
    {
        rb_node *n = t->root;
        if (t->cmp(e, n->elem) == 0)
        {
            free(n);
            t->root = NULL;
            t->size--;
            return true;
        }
        else
            return false;
    }
    rb_node *prev = NULL;
    rb_node *cur = t->root;
    while (cur != NULL)
    {
        int c = t->cmp(e, cur->elem);
        if (c == 0)
            return false;
        else
        {
            prev = cur;
            if (c < 0)
                cur = cur->left;
            else
                cur = cur->right;
        }
    }
    rb_node *n = (rb_node *) malloc(sizeof(rb_node));
    n->elem = e;
    n->color = red;
    n->left = n->right = NULL;
    if (t->cmp(e, cur->elem))
        cur = cur->left;
    else
        cur = cur->right;
    t->size++;
    return false;
}

bool rbtreeContains(rbtree *t, T *e)
{
    rb_node *n = t->root;
    while (n != NULL)
    {
        int c = t->cmp(e, n->elem);
        if (c == 0)
            return true;
        else if (c < 0)
            n = n->left;
        else
            n = n->right;
    }
    return false;
}

unsigned int rbtreeSize(rbtree *t)
{
    return t->size;
}

void rbtreeDestroy(rbtree *t)
{
    free(t);
    t = NULL;
}

bool rbtreeRemoveHelper(rb_node *n, T *e)
{
}

bool isRed(rb_node *n)
{
    return n != NULL && n->color == red;
}
