#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "buffer.h"
#include "directive.h"
#include "lexfsm.h"
#include "token.h"
#include "state.h"

/* Helper functions */
void classifyLiteral(token *);
void lookupKeyword(token *);
bool strneqi(char *, char *, unsigned int);

bool getToken(token *t)
{
    state curState = state_begin;
    char curChar;
    // Main finite state machine loop
    while (curState != state_end)
    {
        curChar = bufferGetChar();
        if (curChar == '\0')
            return false;
        curState = performAction(curState, curChar, t);
    }
    // Reclassify tokens where needed
    if (t->kind == tok_literal)
        classifyLiteral(t);
    else if (t->kind == tok_id)
        lookupKeyword(t);
    if (directives[dir_token_echo])
    {
        char cstr[t->lexeme.len+1];
        stringToCString(&t->lexeme, cstr, t->lexeme.len+1);
        fprintf(stdout, "Lexeme: %s\tLength: %d\tKind: %s\n",
                cstr, t->lexeme.len, tokenKindString(t->kind));
    }
    return true;
}

/**
 * Reclassifies literals depending on their length.
 **/
void classifyLiteral(token *t)
{
    if (t->lexeme.len == 1)
        t->kind = tok_char_const;
    else if (t->lexeme.len == 10)
        t->kind = tok_alfa_const;
    else
        t->kind = tok_string_const;
}

/**
 * Looks up if the given token is a keyword.
 **/
void lookupKeyword(token *t)
{
    unsigned int n = t->lexeme.len;
    char *s = t->lexeme.buffer;
    if (n == 2)
    {
        if (strneqi(s, "do", n))
            t->kind = tok_kw_do;
        else if (strneqi(s, "if", n))
            t->kind = tok_kw_if;
        else if (strneqi(s, "of", n))
            t->kind = tok_kw_of;
        else if (strneqi(s, "or", n))
            t->kind = tok_kw_or;
        else if (strneqi(s, "to", n))
            t->kind = tok_kw_to;
    }
    else if (n == 3)
    {
        if (strneqi(s, "and", n))
            t->kind = tok_kw_and;
        else if (strneqi(s, "div", n))
            t->kind = tok_kw_div;
        else if (strneqi(s, "end", n))
            t->kind = tok_kw_end;
        else if (strneqi(s, "for", n))
            t->kind = tok_kw_for;
        else if (strneqi(s, "mod", n))
            t->kind = tok_kw_mod;
        else if (strneqi(s, "not", n))
            t->kind = tok_kw_not;
        else if (strneqi(s, "var", n))
            t->kind = tok_kw_var;
    }
    else if (n == 4)
    {
        if (strneqi(s, "alfa", n))
            t->kind = tok_kw_alfa;
        else if (strneqi(s, "char", n))
            t->kind = tok_kw_char;
        else if (strneqi(s, "else", n))
            t->kind = tok_kw_else;
        else if (strneqi(s, "real", n))
            t->kind = tok_kw_real;
        else if (strneqi(s, "text", n))
            t->kind = tok_kw_text;
        else if (strneqi(s, "then", n))
            t->kind = tok_kw_then;
        else if (strneqi(s, "true", n))
            t->kind = tok_kw_true;
    }
    else if (n == 5)
    {
        if (strneqi(s, "array", n))
            t->kind = tok_kw_array;
        else if (strneqi(s, "begin", n))
            t->kind = tok_kw_begin;
        else if (strneqi(s, "const", n))
            t->kind = tok_kw_const;
        else if (strneqi(s, "false", n))
            t->kind = tok_kw_false;
        else if (strneqi(s, "while", n))
            t->kind = tok_kw_while;
    }
    else if (n == 6)
    {
        if (strneqi(s, "downto", n))
            t->kind = tok_kw_downto;
    }
    else if (n == 7)
    {
        if (strneqi(s, "boolean", n))
            t->kind = tok_kw_boolean;
        else if (strneqi(s, "integer", n))
            t->kind = tok_kw_integer;
        else if (strneqi(s, "program", n))
            t->kind = tok_kw_program;
    }
    else if (n == 9)
    {
        if (strneqi(s, "procedure", n))
            t->kind = tok_kw_procedure;
    }
}

/**
 * Performs a case-insensitive comparison for equality on two strings.
 **/
bool strneqi(char *s1, char *s2, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++)
        if (tolower(s1[i]) != tolower(s2[i]))
            return false;
    return true;
}
