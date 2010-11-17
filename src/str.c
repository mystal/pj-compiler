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
    stringAppendCharArray(str, other->buffer, other->len);
}

void stringAppendCharArray(string *str, char *other, unsigned int n)
{
    unsigned int len = str->len;
    bool resized = (len + n) >= str->bytes_alloc;
    while ((len + n) >= str->bytes_alloc)
        str->bytes_alloc *= 2;
    if (resized)
    {
        char *newBuff = (char *) malloc(sizeof(char)*str->bytes_alloc);
        strncpy(newBuff, str->buffer, len);
        free(str->buffer);
        str->buffer = newBuff;
    }
    strncpy(str->buffer+len, other, n);
    str->len += n;
}

void stringDrop(string *str, unsigned int n)
{
    str->len -= (str->len < n) ? str->len : n;
}

int stringCompareString(string *s1, string *s2)
{
    return stringCompareCharArray(s1, s2->buffer, s2->len);
}

int stringCompareCharArray(string *s1, char *s2, unsigned int n)
{
    for (unsigned int i = 0; i < s1->len && i < n; i++)
    {
        int diff = tolower(s1->buffer[i]) - tolower(s2[i]);
        if (diff < 0)
            return -1;
        if (diff > 0)
            return 1;
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

int stringToInt(string *str)
{
    int ret;
    stringAppendChar(str, '\0');
    ret = atoi(stringGetBuffer(str));
    stringDrop(str, 1);
    return ret;
}

float stringToFloat(string *str)
{
    float ret;
    stringAppendChar(str, '\0');
    ret = atof(stringGetBuffer(str));
    stringDrop(str, 1);
    return ret;
}

bool stringToBool(string *str)
{
    if (stringCompareCharArray(str, "true", 4*sizeof(char)) == 0)
        return true;
    return false;
}

void stringDestroy(string *str)
{
    free(str->buffer);
    free(str);
    str = NULL;
}
