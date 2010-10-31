#include "error.h"

#include <stdio.h>

#include "buffer.h"
#include "token.h"

char *errorStrings[err_num] =
{
    "unexpected token",
    "unexpected token in expression",
    "expected an integer or id",
    "expected keyword downto or keyword to",
    "expected a constant",
    "expected a type",
    "expected keyword end",
    "expected a period",
    "illegal start of statement"
};

void error(error_kind e, token *t)
{
    unsigned int line = bufferLineNumber();
    unsigned int pos = bufferPos();
    fprintf(stdout, "(%d, %d): error: %s, found '%.*s'\n", line,
            pos - t->lexeme.len + 1, errorStrings[e], t->lexeme.len,
            t->lexeme.buffer);
    bufferPrint(stdout);
    for (int i = 0; i < pos - t->lexeme.len; i++)
        fprintf(stdout, " ");
    for (int i = 0; i < t->lexeme.len; i++)
        fprintf(stdout, "^");
    fprintf(stdout, "\n");
}

char *errorString(error_kind e)
{
    return errorStrings[e];
}
