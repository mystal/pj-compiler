#include "str.h"

#include <stdio.h>
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

void stringAppend(string *str, char c)
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

int stringToCString(string *str, char *cstr, unsigned int n)
{
    if (n > str->len)
    {
        strncpy(cstr, str->buffer, str->len);
        cstr[str->len] = '\0';
        return str->len+1;
    }
    return -1;
}
