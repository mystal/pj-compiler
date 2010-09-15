#include "token.h"

#include "str.h"

char *kindStrings[] = 
{
    "id\0",
    "fileid\0",
    "integer constant\0",
    "real constant\0",
    "literal\0",
    "char constant\0",
    "alfa constant\0",
    "string constant\0",
    "comment\0",
    "left parenthesis\0",
    "right parenthesis\0",
    "left bracket\0",
    "right bracket\0",
    "less than\0",
    "greater than\0",
    "less than or equal\0",
    "greater than or equal\0",
    "not equal\0",
    "dot\0",
    "range dots\0",
    "comma\0",
    "semi colon\0",
    "colon\0",
    "colon equal\0",
    "plus\0",
    "minus\0",
    "asterisk\0",
    "slash\0",
    "keyword alfa\0",
    "keyword and\0",
    "keyword array\0",
    "keyword begin\0",
    "keyword boolean\0",
    "keyword char\0",
    "keyword const\0",
    "keyword div\0",
    "keyword do\0",
    "keyword downto\0",
    "keyword else\0",
    "keyword end\0",
    "keyword false\0",
    "keyword for\0",
    "keyword if\0",
    "keyword integer\0",
    "keyword mod\0",
    "keyword not\0",
    "keyword of\0",
    "keyword or\0",
    "keyword procedure\0",
    "keyword program\0",
    "keyword real\0",
    "keyword text\0",
    "keyword then\0",
    "keyword to\0",
    "keyword true\0",
    "keyword var\0",
    "keyword while\0",
    "undefined\0"
};

void tokenInit(token *t)
{
    stringInit(&t->lexeme);
    t->kind = tok_undef;
}

void tokenClean(token *t)
{
    stringFree(&t->lexeme);
}

char * tokenKindString(token_kind k)
{
    return kindStrings[k];
}
