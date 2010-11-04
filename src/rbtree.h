#pragma once

#include <stdbool.h>

#define T void

//typedef bool (*equals)(T *, T *);
typedef int (*compareTo)(T *, T *);

typedef struct __rbtree rbtree;

rbtree *rbtreeCreate(compareTo);

bool rbtreeInsert(rbtree *, T *);

bool rbtreeRemove(rbtree *, T *);

bool rbtreeContains(rbtree *, T *);

unsigned int rbtreeSize(rbtree *);

void rbtreeDestroy(rbtree *);
