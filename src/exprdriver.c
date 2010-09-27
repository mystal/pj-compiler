#include "exprsymbol.h"
#include "exprparser.h"
#include "lexer.h"
#include "token.h"

int main()
{
    token t;
    tokenInit(&t);
    do
    {
        tokenClean(&t);
        tokenInit(&t);
        lexerGetToken(&t);
        while (t.kind != tok_undef && !isExprToken(t.kind))
        {
            tokenClean(&t);
            tokenInit(&t);
            lexerGetToken(&t);
        }
        if (t.kind != tok_undef)
            expr(&t);
    } while (t.kind != tok_undef);
    tokenClean(&t);
}
