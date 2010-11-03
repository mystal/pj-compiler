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

void error(error_kind e, token *t, token_kind tok)
{
    unsigned int line = bufferLineNumber();
    unsigned int pos = bufferPos();
    unsigned int lexLen = t->lexeme.len;
    if (t->kind != tok_char_const && t->kind != tok_alfa_const &&
        t->kind != tok_string_const)
        fprintf(stdout, "(%d, %d): error: %s, found \"%.*s\"", line,
            pos - lexLen + 1, errorStrings[e], t->lexeme.len,
            t->lexeme.buffer);
    else
    {
        lexLen += 2;
        fprintf(stdout, "(%d, %d): error: %s, found \"'%.*s'\"", line,
            pos - lexLen + 1, errorStrings[e], t->lexeme.len,
            t->lexeme.buffer);
    }
    if (e == err_unex)
        fprintf(stdout, ", expected %s", tokenKindString(tok));
    fprintf(stdout, "\n");
    bufferPrint(stdout);
    for (int i = 0; i < pos - lexLen; i++)
        fprintf(stdout, " ");
    for (int i = 0; i < lexLen; i++)
        fprintf(stdout, "^");
    fprintf(stdout, "\n");
}
