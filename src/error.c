#include "error.h"

#include <stdio.h>

#include "buffer.h"
#include "codegen.h"
#include "str.h"
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
    "illegal start of statement",
    "duplicate symbol",
    "undefined symbol",
    "file must be of type text",
    "undeclared file(s)",
    "array range must be an integer constant",
    "builtin cannot be redefined",
    "unsupported operand type",
    "invalid file buffer"
};

void errorParse(error_kind e, token *t, token_kind tok)
{
    codegenReportError();
    unsigned int line = bufferLineNumber();
    unsigned int pos = bufferPos();
    unsigned int lexLen = stringGetLength(t->lexeme);
    char *lexBuffer = stringGetBuffer(t->lexeme);
    if (t->kind != tok_char_const && t->kind != tok_alfa_const &&
        t->kind != tok_string_const)
        fprintf(stdout, "(%d, %d): error: %s, found \"%.*s\"", line,
            pos - lexLen + 1, errorStrings[e], lexLen, lexBuffer);
    else
    {
        fprintf(stdout, "(%d, %d): error: %s, found \"'%.*s'\"", line,
            pos - lexLen - 1, errorStrings[e], lexLen, lexBuffer);
        lexLen += 2;
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

void errorST(error_kind e, string *lexeme)
{
    codegenReportError();
    if (e == err_dup_sym || e == err_undef_sym || e == err_file_not_text ||
        e == err_range_not_const)
    {
        unsigned int lexLen = stringGetLength(lexeme);
        char *lexBuffer = stringGetBuffer(lexeme);
        unsigned int line = bufferLineNumber();
        unsigned int pos = bufferPos();
        fprintf(stdout, "(%d, %d): error: %s, \"%.*s\"\n", line,
            pos - lexLen + 1, errorStrings[e], lexLen, lexBuffer);
        bufferPrint(stdout);
    }
    else if (e == err_undecl_file)
        fprintf(stdout, "error: %s,", errorStrings[e]);
}

void errorType(error_kind e)
{
    unsigned int line = bufferLineNumber();
    unsigned int pos = bufferPos();
    codegenReportError();
    fprintf(stdout, "(%d, %d): error: %s\n", line, pos, errorStrings[e]);
    bufferPrint(stdout);
}
