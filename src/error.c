#include "error.h"

#include <stdio.h>

#include "buffer.h"
#include "token.h"

char *errorStrings[err_num] =
{
    "unexpected token",
    "expected a constant",
    "expected a type",
    "expected keyword end",
    "expected a period"
};

void error(error_kind e, token *t)
{
    unsigned int line = bufferLineNumber();
    unsigned int pos = bufferPos();
    fprintf(stdout, "(%d, %d): error: %s\n", line, pos - t->lexeme.len,
            errorStrings[e]);
    bufferPrint(stdout);
    for (int i = 1; i < pos; i++)
        fprintf(stdout, " ");
    for (int i = 0; i < t->lexeme.len; i++)
        fprintf(stdout, "^");
    fprintf(stdout, "\n");
}

char *errorString(error_kind e)
{
    return errorStrings[e];
}
