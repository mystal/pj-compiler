#include "exprsymbol.h"
#include "exprparser.h"
#include "lexer.h"
#include "token.h"

int main()
{
    token t;
    tokenInit(&t);
    lexerGetToken(&t);
    while (t.kind != tok_undef)
    {
        while (t.kind != tok_undef && (!isExprToken(t.kind)
               || t.kind == tok_rparen || t.kind == tok_rbrack))
        {
            tokenClean(&t);
            tokenInit(&t);
            lexerGetToken(&t);
        }
        if (t.kind != tok_undef)
            expr(&t);
    }
    tokenClean(&t);
}
