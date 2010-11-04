#include "bst.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct __bst_node bst_node;

struct __bst_node
{
    BST_T *elem;
    bst_node *parent;
    bst_node *left;
    bst_node *right;
};

struct __bst
{
    bst_node *root;
    unsigned int size;
    cmp_func compareTo;
};

/* bst_node methods */
bst_node *bst_nodeCreate(BST_T *, bst_node *, bst_node *, bst_node *);

/* Private helper functions */
bool bstInsertHelper(bst *, bst_node *, BST_T *);
bool bstRemoveHelper(bst *, bst_node *, BST_T *);
void bstClearHelper(bst_node *, del_func);
bst_node *bstFindMin(bst_node *);
void bstReplaceSelf(bst_node *, bst_node *);
void bstPrintHelper(bst_node *, print_func);

bst *bstCreate(cmp_func cmp)
{
    bst *t = (bst *) malloc(sizeof(bst));
    t->root = NULL;
    t->size = 0;
    t->compareTo = cmp;
    return t;
}

bool bstInsert(bst *t, BST_T *e)
{
    bool success;
    if(t->size == 0)
    {
        t->root = bst_nodeCreate(e, NULL, NULL, NULL);
        success = true;
    }
    else
        success = bstInsertHelper(t, t->root, e);
    if (success)
        t->size++;
    return success;
}

bool bstRemove(bst *t, BST_T *e)
{
    if (t->size == 0)
        return false;
    bool success = bstRemoveHelper(t, t->root, e);
    if (success)
        t->size--;
    return success;
}

bool bstContains(bst *t, BST_T *e)
{
    bst_node *n = t->root;
    while (n != NULL)
    {
        if (t->compareTo(e, n->elem) == 0)
            return true;
        else if (t->compareTo(e, n->elem) < 0)
            n = n->left;
        else
            n = n->right;
    }
    return false;
}

BST_T *bstGet(bst *t, BST_T *e)
{
    bst_node *n = t->root;
    while (n != NULL)
    {
        if (t->compareTo(e, n->elem) == 0)
            return n->elem;
        else if (t->compareTo(e, n->elem) < 0)
            n = n->left;
        else
            n = n->right;
    }
    return NULL;
}

unsigned int bstSize(bst *t)
{
    return t->size;
}

void bstPrint(bst *t, print_func pf)
{
    bstPrintHelper(t->root, pf);
}

void bstDestroy(bst *t, del_func del)
{
    bstClearHelper(t->root, del);
    free(t);
    t = NULL;
}

bst_node *bst_nodeCreate(BST_T *e, bst_node *p, bst_node *l, bst_node *r)
{
    bst_node *n = (bst_node *) malloc(sizeof(bst_node));
    n->elem = e;
    n->parent = p;
    n->left = l;
    n->right = r;
    return n;
}

bool bstInsertHelper(bst *t, bst_node *n, BST_T *e)
{
    if (t->compareTo(e, n->elem) == 0)
        return false;
    if (t->compareTo(e, n->elem) < 0)
    {
        if (n->left == NULL)
        {
            n->left = bst_nodeCreate(e, n, NULL, NULL);
            return true;
        }
        return bstInsertHelper(t, n->left, e);
    }
    else
    {
        if (n->right == NULL)
        {
            n->right = bst_nodeCreate(e, n, NULL, NULL);
            return true;
        }
        return bstInsertHelper(t, n->right, e);
    }
}

bool bstRemoveHelper(bst *t, bst_node *n, BST_T *e)
{
    if (n == NULL)
        return false;
    if (t->compareTo(e, n->elem) < 0)
        return bstRemoveHelper(t, n->left, e);
    if (t->compareTo(e, n->elem) > 0)
        return bstRemoveHelper(t, n->right, e);
    if (n->left != NULL && n->right != NULL)
    {
        bst_node *successor = bstFindMin(n->right);
        n->elem = successor->elem;
        bstReplaceSelf(successor, successor->right);
        free(successor);
    }
    else if (n->left != NULL)
    {
        if (n->parent != NULL)
            bstReplaceSelf(n, n->left);
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
            bstReplaceSelf(n, n->right);
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
            bstReplaceSelf(n, NULL);
        else
            t->root = NULL;
        free(n);
    }
    return true;
}

void bstClearHelper(bst_node *n, del_func del)
{
    if (n != NULL)
    {
        bstClearHelper(n->left, del);
        bstClearHelper(n->right, del);
        if (del != NULL)
            del(n->elem);
        free(n);
    }
}

bst_node *bstFindMin(bst_node *n)
{
    bst_node *min = n;
    while (min->left != NULL)
        min = min->left;
    return min;
}

void bstReplaceSelf(bst_node *n, bst_node *newN)
{
    bst_node *p = n->parent;
    if (p->left == n)
        p->left = newN;
    else
        p->right = newN;
    if (newN != NULL)
        newN->parent = p;
}

void bstPrintHelper(bst_node *n, print_func pf)
{
    if (n == NULL)
        return;
    bstPrintHelper(n->left, pf);
    pf(n->elem);
    bstPrintHelper(n->right, pf);
}
