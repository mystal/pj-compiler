#include "str.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct __string
{
    char *buffer;
    unsigned int len;
    size_t bytes_alloc;
};

string *stringCreate()
{
    string *str = (string *) malloc(sizeof(string));
    str->bytes_alloc = 2;
    str->len = 0;
    str->buffer = (char *) malloc(sizeof(char)*str->bytes_alloc);
    return str;
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

void stringAppendString(string *str, string *other)
{
    unsigned int len1 = str->len;
    unsigned int len2 = other->len;
    bool resized = (len1 + len2) >= str->bytes_alloc;
    while ((len1 + len2) >= str->bytes_alloc)
        str->bytes_alloc *= 2;
    if (resized)
    {
        char *newBuff = (char *) malloc(sizeof(char)*str->bytes_alloc);
        strncpy(newBuff, str->buffer, len1);
        free(str->buffer);
        str->buffer = newBuff;
    }
    strncpy(str->buffer+len1, other->buffer, len2);
    str->len += len2;
}

void stringAppendCharArray(string *str, char *other, unsigned int n)
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

void stringDrop(string *str, unsigned int n)
{
    str->len -= (str->len < n) ? str->len : n;
}

int stringCompareString(string *s1, string *s2)
{
    return stringCompareCharArray(s1, stringGetBuffer(s2),
                                  stringGetLength(s2));
}

int stringCompareCharArray(string *s1, char *s2, unsigned int n)
{
    unsigned int i = 0;
    while (i < s1->len && i < n)
    {
        int diff = tolower(s1->buffer[i]) - tolower(s2[i]);
        if (diff < 0)
            return -1;
        if (diff > 0)
            return 1;
        i++;
    }
    if (s1->len < n)
        return -1;
    if (s1->len > n)
        return 1;
    return 0;
}

unsigned int stringGetLength(string *str)
{
    return str->len;
}

char *stringGetBuffer(string *str)
{
    return str->buffer;
}

void stringDestroy(string *str)
{
    free(str->buffer);
    free(str);
    str = NULL;
}
