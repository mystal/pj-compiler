#include "token.h"

#include <stdbool.h>

#include "str.h"

const char *kindStrings[] = 
{
    "id",
    "fileid",
    "integer constant",
    "real constant",
    "literal",
    "char constant",
    "alfa constant",
    "string constant",
    "comment",
    "left parenthesis",
    "right parenthesis",
    "left bracket",
    "right bracket",
    "less than",
    "greater than",
    "less than or equal",
    "greater than or equal",
    "not equal",
    "equal",
    "dot",
    "range dots",
    "comma",
    "semi colon",
    "colon",
    "colon equal",
    "plus",
    "minus",
    "asterisk",
    "slash",
    "keyword alfa",
    "keyword and",
    "keyword array",
    "keyword begin",
    "keyword boolean",
    "keyword char",
    "keyword const",
    "keyword div",
    "keyword do",
    "keyword downto",
    "keyword else",
    "keyword end",
    "keyword false",
    "keyword for",
    "keyword if",
    "keyword integer",
    "keyword mod",
    "keyword not",
    "keyword of",
    "keyword or",
    "keyword procedure",
    "keyword program",
    "keyword real",
    "keyword text",
    "keyword then",
    "keyword to",
    "keyword true",
    "keyword var",
    "keyword while",
    "undefined"
};

void tokenInit(token *t)
{
    t->lexeme = stringCreate();
    t->kind = tok_undef;
}

void tokenClean(token *t)
{
    stringDestroy(t->lexeme);
}

const char *tokenKindString(token_kind k)
{
    return kindStrings[k];
}
