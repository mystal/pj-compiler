#include "bst.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct __bst_node bst_node;

struct __bst_node
{
    token_kind elem;
    bst_node *left;
    bst_node *right;
};

struct __bst
{
    bst_node *root;
    unsigned int size;
};

/* Private helper functions */
bool insertHelper(bst_node *, token_kind);
bool removeHelper(bst *, bst_node *, bst_node *, token_kind);
void clearHelper(bst_node *);
bst_node *findMin(bst_node *);
void replaceChild(bst_node *, bst_node *, bst_node *);

bst *bstCreate()
{
    bst *t = (bst *) malloc(sizeof(bst));
    t->root = NULL;
    t->size = 0;
    return t;
}

bool bstInsert(bst *t, token_kind e)
{
    if(t->size == 0)
    {
        bst_node *newN = (bst_node *) malloc(sizeof(bst_node));
        newN->elem = e;
        newN->left = newN->right = NULL;
        t->root = newN;
        t->size++;
        return true;
    }
    bool success = insertHelper(t->root, e);
    if (success)
        t->size++;
    return success;
}

bool bstRemove(bst *t, token_kind e)
{
    if (t->size == 0)
        return false;
    bool success = removeHelper(t, t->root, NULL, e);
    if (success)
        t->size--;
    return success;
}

bool bstContains(bst *t, token_kind e)
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

bool insertHelper(bst_node *n, token_kind e)
{
    if (e == n->elem)
        return false;
    if (e < n->elem)
    {
        if(n->left == NULL)
        {
            bst_node *newN = (bst_node *) malloc(sizeof(bst_node));
            newN->elem = e;
            newN->left = newN->right = NULL;
            n->left = newN;
            return true;
        }
        return insertHelper(n->left, e);
    }
    else
    {
        if(n->right == NULL)
        {
            bst_node *newN = (bst_node *) malloc(sizeof(bst_node));
            newN->elem = e;
            newN->left = newN->right = NULL;
            n->right = newN;
            return true;
        }
        return insertHelper(n->right, e);
    }
}

bool removeHelper(bst *t, bst_node *n, bst_node *p, token_kind e)
{
    if (n == NULL)
        return false;
    if (e < n->elem)
        return removeHelper(t, n->left, n, e);
    if (e > n->elem)
        return removeHelper(t, n->right, n, e);
    if (n->left != NULL && n->right != NULL)
    {
        bst_node *successor = findMin(n->right);
        n->elem = successor->elem;
        replaceChild(successor);
    }
    else if (n->left != NULL)
    {
        if (p != NULL)
            replaceChild(n, p, r->left);
        else
            t->root = r->left;
        free(n);
    }
    else if (n->right != NULL)
    {
        if (p != NULL)
            replaceChild(n, p, r->right);
        else
            t->root = r->right;
        free(n);
    }
    else
    {
        if (p != NULL)
            replaceChild(n, p, NULL);
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
    while (n->left != NULL)
        min = n->left;
    return min;
}

void replaceChild(bst_node *n, bst_node *p, bst_node *newC)
{
    if (p->left == n)
        p->left = newC;
    else
        p->right = newC;
}
