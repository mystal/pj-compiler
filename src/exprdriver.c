#include "exprsymbol.h"
#include "exprparser.h"
#include "lexer.h"
#include "token.h"

int main()
{
    lexerInit();
    token *t = lexerGetToken();
    while (t->kind != tok_undef)
    {
        while (t->kind != tok_undef && (!isExprToken(t)||
               t->kind == tok_rparen || t->kind == tok_rbrack))
            t = lexerGetToken();
        if (t->kind != tok_undef)
            expr(t);
    }
    lexerCleanup();
}
