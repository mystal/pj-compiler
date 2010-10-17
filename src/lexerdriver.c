#include "lexer.h"
#include "token.h"

int main()
{
    lexerInit();
    token *t = lexerGetToken();
    while (t->kind != tok_undef)
        t = lexerGetToken();
    lexerCleanup();
}
