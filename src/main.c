#include "lexer.h"
#include "token.h"

int main()
{
    lexerInit();
    token t;
    tokenInit(&t);
    while (getToken(&t))
    {
        tokenClean(&t);
        tokenInit(&t);
    }
    tokenClean(&t);
}
