#include "str.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void stringInit(string *str)
{
    str->bytes_alloc = 2;
    str->len = 0;
    str->buffer = (char *) malloc(sizeof(char)*str->bytes_alloc);
}

void stringFree(string *str)
{
    free(str->buffer);
    str->len = 0;
    str->bytes_alloc = 0;
}

void stringAppendChar(string *str, char c)
{
    if (str->len == str->bytes_alloc)
    {
        str->bytes_alloc *= 2;
        char *newBuff = (char *) malloc(sizeof(char)*str->bytes_alloc);
        strncpy(newBuff, str->buffer, str->len);
        free(str->buffer);
        str->buffer = newBuff;
    }
    str->buffer[str->len++] = c;
}

void stringAppendString(string *str, char *other, unsigned int n)
{
    bool resized = (str->len + n) >= str->bytes_alloc;
    while ((str->len + n) >= str->bytes_alloc)
        str->bytes_alloc *= 2;
    if (resized)
    {
        char *newBuff = (char *) malloc(sizeof(char)*str->bytes_alloc);
        strncpy(newBuff, str->buffer, str->len);
        free(str->buffer);
        str->buffer = newBuff;
    }
    strncpy(str->buffer+str->len, other, n);
    str->len += n;
}

void stringRemove(string *str, unsigned int n)
{
    str->len -= n;
}
