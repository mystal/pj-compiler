#include "lexer.h"
#include "token.h"

int main()
{
    token t;
    tokenInit(&t);
    while (getToken(&t))
    {
        tokenClean(&t);
        tokenInit(&t);
    }
    tokenClean(&t);
}
