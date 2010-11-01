#include "bst.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct __bst_node bst_node;

struct __bst_node
{
    T elem;
    bst_node *parent;
    bst_node *left;
    bst_node *right;
};

struct __bst
{
    bst_node *root;
    unsigned int size;
};

/* bst_node methods */
bst_node *bst_nodeCreate(T, bst_node *, bst_node *, bst_node *);

/* Private helper functions */
bool insertHelper(bst_node *, T);
bool removeHelper(bst_node *, bst *, T);
void clearHelper(bst_node *);
bst_node *findMin(bst_node *);
void replaceSelf(bst_node *, bst_node *);

bst *bstCreate()
{
    bst *t = (bst *) malloc(sizeof(bst));
    t->root = NULL;
    t->size = 0;
    return t;
}

bool bstInsert(bst *t, T e)
{
    bool success;
    if(t->size == 0)
    {
        t->root = bst_nodeCreate(e, NULL, NULL, NULL);
        success = true;
    }
    else
        success = insertHelper(t->root, e);
    if (success)
        t->size++;
    return success;
}

bool bstRemove(bst *t, T e)
{
    if (t->size == 0)
        return false;
    bool success = removeHelper(t->root, t, e);
    if (success)
        t->size--;
    return success;
}

bool bstContains(bst *t, T e)
{
    bst_node *n = t->root;
    while (n != NULL)
    {
        if (e == n->elem)
            return true;
        else if (e < n->elem)
            n = n->left;
        else
            n = n->right;
    }
    return false;
}

unsigned int bstSize(bst *t)
{
    return t->size;
}

void bstDestroy(bst *t)
{
    clearHelper(t->root);
    free(t);
    t = NULL;
}

bst_node *bst_nodeCreate(T e, bst_node *p, bst_node *l, bst_node *r)
{
    bst_node *n = (bst_node *) malloc(sizeof(bst_node));
    n->elem = e;
    n->parent = p;
    n->left = l;
    n->right = r;
    return n;
}

bool insertHelper(bst_node *n, T e)
{
    if (e == n->elem)
        return false;
    if (e < n->elem)
    {
        if (n->left == NULL)
        {
            n->left = bst_nodeCreate(e, n, NULL, NULL);
            return true;
        }
        return insertHelper(n->left, e);
    }
    else
    {
        if (n->right == NULL)
        {
            n->right = bst_nodeCreate(e, n, NULL, NULL);
            return true;
        }
        return insertHelper(n->right, e);
    }
}

bool removeHelper(bst_node *n, bst *t, T e)
{
    if (n == NULL)
        return false;
    if (e < n->elem)
        return removeHelper(n->left, t, e);
    if (e > n->elem)
        return removeHelper(n->right, t, e);
    if (n->left != NULL && n->right != NULL)
    {
        bst_node *successor = findMin(n->right);
        n->elem = successor->elem;
        replaceSelf(successor, successor->right);
        free(successor);
    }
    else if (n->left != NULL)
    {
        if (n->parent != NULL)
            replaceSelf(n, n->left);
        else
        {
            t->root = n->left;
            t->root->parent = NULL;
        }
        free(n);
    }
    else if (n->right != NULL)
    {
        if (n->parent != NULL)
            replaceSelf(n, n->right);
        else
        {
            t->root = n->right;
            t->root->parent = NULL;
        }
        free(n);
    }
    else
    {
        if (n->parent != NULL)
            replaceSelf(n, NULL);
        else
            t->root = NULL;
        free(n);
    }
    return true;
}

void clearHelper(bst_node *n)
{
    if (n != NULL)
    {
        clearHelper(n->left);
        clearHelper(n->right);
        free(n);
    }
}

bst_node *findMin(bst_node *n)
{
    bst_node *min = n;
    while (min->left != NULL)
        min = min->left;
    return min;
}

void replaceSelf(bst_node *n, bst_node *newN)
{
    bst_node *p = n->parent;
    if (p->left == n)
        p->left = newN;
    else
        p->right = newN;
    if (newN != NULL)
        newN->parent = p;
}
