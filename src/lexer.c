#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>

#include "buffer.h"
#include "directive.h"
#include "lexfsm.h"
#include "state.h"

void classifyLiteral(token *);
void lookupKeyword(token *);

void lexerInit()
{
    lineBuffer[0] = '\0';
    fsmInit();
}

bool getToken(token *t)
{
    state curState = state_begin;
    char curChar;
    while (curState != state_end)
    {
        curChar = getChar();
        if (curChar == '\0')
            return false;
        curState = performAction(curState, curChar, t);
    }
    if (t->kind == tok_literal)
        classifyLiteral(t);
    else if (t->kind == tok_id)
        lookupKeyword(t);
    if (directives[dir_token_echo])
    {
        char cstr[t->lexeme.len+1];
        stringToCString(&t->lexeme, cstr, t->lexeme.len+1);
        fprintf(stdin, "Lexeme: %s\tLength: %d\tKind: %s",
                cstr, t->lexeme.len, tokenKindString(t->kind));
    }
    return true;
}

void classifyLiteral(token *t)
{
    if (t->lexeme.len == 1)
        t->kind = tok_char_const;
    else if (t->lexeme.len == 10)
        t->kind = tok_alfa_const;
    else
        t->kind = tok_string_const;
}

void lookupKeyword(token *t)
{
}
