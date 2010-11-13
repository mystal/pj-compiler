#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "buffer.h"
#include "directive.h"
#include "lexfsm.h"
#include "token.h"
#include "state.h"

/* The last token read */
token curToken;

/* Private helper functions */
void classifyLiteral(token *);
void lookupKeyword(token *);

void lexerInit()
{
    tokenInit(&curToken);
}

void lexerCleanup()
{
    tokenClean(&curToken);
}

token *lexerGetToken()
{
    tokenClean(&curToken);
    tokenInit(&curToken);
    state curState = state_begin;
    char curChar;
    // Main finite state machine loop
    while (curState != state_end)
    {
        curChar = bufferGetChar();
        if (curChar == '\0')
            return &curToken;
        curState = performAction(curState, curChar, &curToken);
    }
    // Reclassify token where needed
    if (curToken.kind == tok_literal)
        classifyLiteral(&curToken);
    else if (curToken.kind == tok_id)
        lookupKeyword(&curToken);
    if (dirGet(dir_token_echo))
    {
        unsigned int len = stringGetLength(curToken.lexeme);
        char *buff = stringGetBuffer(curToken.lexeme);
        fprintf(stdout, "Lexeme: %.*s\tLength: %d\tKind: %s\n",
                len, buff, len, tokenKindString(curToken.kind));
    }
    return &curToken;
}

/**
 * Reclassifies literals depending on their length.
 **/
void classifyLiteral(token *t)
{
    unsigned int len = stringGetLength(t->lexeme);
    if (len == 1)
        t->kind = tok_char_const;
    else if (len == 10)
        t->kind = tok_alfa_const;
    else
        t->kind = tok_string_const;
}

/**
 * Looks up if the given token is a keyword.
 **/
void lookupKeyword(token *t)
{
    string *s = t->lexeme;
    unsigned int n = stringGetLength(s);
    if (n == 2)
    {
        if (stringCompareCharArray(s, "do", n) == 0)
            t->kind = tok_kw_do;
        else if (stringCompareCharArray(s, "if", n) == 0)
            t->kind = tok_kw_if;
        else if (stringCompareCharArray(s, "of", n) == 0)
            t->kind = tok_kw_of;
        else if (stringCompareCharArray(s, "or", n) == 0)
            t->kind = tok_kw_or;
        else if (stringCompareCharArray(s, "to", n) == 0)
            t->kind = tok_kw_to;
    }
    else if (n == 3)
    {
        if (stringCompareCharArray(s, "and", n) == 0)
            t->kind = tok_kw_and;
        else if (stringCompareCharArray(s, "div", n) == 0)
            t->kind = tok_kw_div;
        else if (stringCompareCharArray(s, "end", n) == 0)
            t->kind = tok_kw_end;
        else if (stringCompareCharArray(s, "for", n) == 0)
            t->kind = tok_kw_for;
        else if (stringCompareCharArray(s, "mod", n) == 0)
            t->kind = tok_kw_mod;
        else if (stringCompareCharArray(s, "not", n) == 0)
            t->kind = tok_kw_not;
        else if (stringCompareCharArray(s, "var", n) == 0)
            t->kind = tok_kw_var;
    }
    else if (n == 4)
    {
        if (stringCompareCharArray(s, "alfa", n) == 0)
            t->kind = tok_kw_alfa;
        else if (stringCompareCharArray(s, "char", n) == 0)
            t->kind = tok_kw_char;
        else if (stringCompareCharArray(s, "else", n) == 0)
            t->kind = tok_kw_else;
        else if (stringCompareCharArray(s, "real", n) == 0)
            t->kind = tok_kw_real;
        else if (stringCompareCharArray(s, "text", n) == 0)
            t->kind = tok_kw_text;
        else if (stringCompareCharArray(s, "then", n) == 0)
            t->kind = tok_kw_then;
        else if (stringCompareCharArray(s, "true", n) == 0)
            t->kind = tok_kw_true;
    }
    else if (n == 5)
    {
        if (stringCompareCharArray(s, "array", n) == 0)
            t->kind = tok_kw_array;
        else if (stringCompareCharArray(s, "begin", n) == 0)
            t->kind = tok_kw_begin;
        else if (stringCompareCharArray(s, "const", n) == 0)
            t->kind = tok_kw_const;
        else if (stringCompareCharArray(s, "false", n) == 0)
            t->kind = tok_kw_false;
        else if (stringCompareCharArray(s, "while", n) == 0)
            t->kind = tok_kw_while;
    }
    else if (n == 6)
    {
        if (stringCompareCharArray(s, "downto", n) == 0)
            t->kind = tok_kw_downto;
    }
    else if (n == 7)
    {
        if (stringCompareCharArray(s, "boolean", n) == 0)
            t->kind = tok_kw_boolean;
        else if (stringCompareCharArray(s, "integer", n) == 0)
            t->kind = tok_kw_integer;
        else if (stringCompareCharArray(s, "program", n) == 0)
            t->kind = tok_kw_program;
    }
    else if (n == 9)
    {
        if (stringCompareCharArray(s, "procedure", n) == 0)
            t->kind = tok_kw_procedure;
    }
}
