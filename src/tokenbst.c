#include "tokenbst.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct __tokenbst_node tokenbst_node;

struct __tokenbst_node
{
    TOKBST_T elem;
    tokenbst_node *parent;
    tokenbst_node *left;
    tokenbst_node *right;
};

struct __tokenbst
{
    tokenbst_node *root;
    unsigned int size;
};

/* tokenbst_node methods */
tokenbst_node *tokenbst_nodeCreate(TOKBST_T, tokenbst_node *, tokenbst_node *,
                                   tokenbst_node *);

/* Private helper functions */
bool tokenbstInsertHelper(tokenbst_node *, TOKBST_T);
bool tokenbstRemoveHelper(tokenbst_node *, tokenbst *, TOKBST_T);
void tokenbstClearHelper(tokenbst_node *);
tokenbst_node *tokenbstFindMin(tokenbst_node *);
void tokenbstReplaceSelf(tokenbst_node *, tokenbst_node *);

tokenbst *tokenbstCreate()
{
    tokenbst *t = (tokenbst *) malloc(sizeof(tokenbst));
    t->root = NULL;
    t->size = 0;
    return t;
}

bool tokenbstInsert(tokenbst *t, TOKBST_T e)
{
    bool success;
    if(t->size == 0)
    {
        t->root = tokenbst_nodeCreate(e, NULL, NULL, NULL);
        success = true;
    }
    else
        success = tokenbstInsertHelper(t->root, e);
    if (success)
        t->size++;
    return success;
}

bool tokenbstRemove(tokenbst *t, TOKBST_T e)
{
    if (t->size == 0)
        return false;
    bool success = tokenbstRemoveHelper(t->root, t, e);
    if (success)
        t->size--;
    return success;
}

bool tokenbstContains(tokenbst *t, TOKBST_T e)
{
    tokenbst_node *n = t->root;
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

unsigned int tokenbstSize(tokenbst *t)
{
    return t->size;
}

void tokenbstDestroy(tokenbst *t)
{
    tokenbstClearHelper(t->root);
    free(t);
    t = NULL;
}

tokenbst_node *tokenbst_nodeCreate(TOKBST_T e, tokenbst_node *p,
                                   tokenbst_node *l, tokenbst_node *r)
{
    tokenbst_node *n = (tokenbst_node *) malloc(sizeof(tokenbst_node));
    n->elem = e;
    n->parent = p;
    n->left = l;
    n->right = r;
    return n;
}

bool tokenbstInsertHelper(tokenbst_node *n, TOKBST_T e)
{
    if (e == n->elem)
        return false;
    if (e < n->elem)
    {
        if (n->left == NULL)
        {
            n->left = tokenbst_nodeCreate(e, n, NULL, NULL);
            return true;
        }
        return tokenbstInsertHelper(n->left, e);
    }
    else
    {
        if (n->right == NULL)
        {
            n->right = tokenbst_nodeCreate(e, n, NULL, NULL);
            return true;
        }
        return tokenbstInsertHelper(n->right, e);
    }
}

bool tokenbstRemoveHelper(tokenbst_node *n, tokenbst *t, TOKBST_T e)
{
    if (n == NULL)
        return false;
    if (e < n->elem)
        return tokenbstRemoveHelper(n->left, t, e);
    if (e > n->elem)
        return tokenbstRemoveHelper(n->right, t, e);
    if (n->left != NULL && n->right != NULL)
    {
        tokenbst_node *successor = tokenbstFindMin(n->right);
        n->elem = successor->elem;
        tokenbstReplaceSelf(successor, successor->right);
        free(successor);
    }
    else if (n->left != NULL)
    {
        if (n->parent != NULL)
            tokenbstReplaceSelf(n, n->left);
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
            tokenbstReplaceSelf(n, n->right);
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
            tokenbstReplaceSelf(n, NULL);
        else
            t->root = NULL;
        free(n);
    }
    return true;
}

void tokenbstClearHelper(tokenbst_node *n)
{
    if (n != NULL)
    {
        tokenbstClearHelper(n->left);
        tokenbstClearHelper(n->right);
        free(n);
    }
}

tokenbst_node *tokenbstFindMin(tokenbst_node *n)
{
    tokenbst_node *min = n;
    while (min->left != NULL)
        min = min->left;
    return min;
}

void tokenbstReplaceSelf(tokenbst_node *n, tokenbst_node *newN)
{
    tokenbst_node *p = n->parent;
    if (p->left == n)
        p->left = newN;
    else
        p->right = newN;
    if (newN != NULL)
        newN->parent = p;
}
