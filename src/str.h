#pragma once

#include <stdlib.h>

typedef struct __string
{
    char *buffer;
    unsigned int len;
    size_t bytes_alloc;
} string;

void stringInit(string *);

void stringFree(string *);

void stringAppend(string *, char);

void stringRemove(string *, unsigned int);

int stringToCString(string *, char *, unsigned int);
