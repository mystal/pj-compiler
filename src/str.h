#pragma once

#include <stddef.h>

typedef struct __string
{
    char *buffer;
    unsigned int len;
    size_t bytes_alloc;
} string;

/**
 *
 **/
void stringInit(string *);

/**
 *
 **/
void stringFree(string *);

/**
 *
 **/
void stringAppendChar(string *, char);

/**
 *
 **/
void stringAppendString(string *, char *, unsigned int);

/**
 *
 **/
void stringRemove(string *, unsigned int);

/**
 *
 **/
int stringCompare(string *, string *);
