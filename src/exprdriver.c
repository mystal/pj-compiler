#include "exprsymbol.h"
#include "lexer.h"
#include "token.h"

int main()
{
    token t;
    tokenInit(&t);
    lexerGetToken(&t);
    while (t.kind != tok_undef)
    {
        tokenClean(&t);
        tokenInit(&t);
        lexerGetToken(&t);
    }
    tokenClean(&t);
}
