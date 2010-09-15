#include "lexfsm.h"

#include <ctype.h>
#include <stdio.h>

#include "buffer.h"
#include "state.h"
#include "token.h"

typedef void (*action_func)(token *, char);

typedef struct __transition
{
    action_func action;
    state next;
} transition;

transition trans_table [state_num][ccls_num];

/* Actions */
void error(token *, char);

char_class lookupClass(char);

void fsmInit()
{
    transition err = {error, state_begin};
    for (int i = 0; i < state_num; i++)
        for (int j = 0; j < ccls_num; j++)
            trans_table[i][j] = err;

    //Set all other states
}

state performAction(state s, char c, token *t)
{
    char_class ccls = lookupClass(c);
    trans_table[s][ccls].action(t, c);
    return trans_table[s][ccls].next;
}

void error(token *t, char c)
{
    tokenClean(t);
    tokenInit(t);
    if (c == '\n')
        fprintf(stderr, "Unexpected symbol \'\\n\':\n%s", lineBuffer);
    else
        fprintf(stderr, "Unexpected symbol \'%c\':\n%s", c, lineBuffer);
}

char_class lookupClass(char c)
{
    if (c == 'e' || c == 'E')
        return ccls_e;
    if (isalpha(c))
        return ccls_ad_fz;
    if (isdigit(c))
        return ccls_09;
    if (c == '\n')
        return ccls_eol;
    if (isspace(c))
        return ccls_whitespace;
    switch (c)
    {
        case '+': return ccls_plus;
        case '-': return ccls_minus;
        case '*': return ccls_ast;
        case '/': return ccls_div;
        case '=': return ccls_eq;
        case '<': return ccls_lt;
        case '>': return ccls_gt;
        case '[': return ccls_lbrack;
        case ']': return ccls_rbrack;
        case '.': return ccls_dot;
        case ',': return ccls_comma;
        case ':': return ccls_colon;
        case ';': return ccls_semi;
        case '^': return ccls_carat;
        case '(': return ccls_lparen;
        case ')': return ccls_rparen;
        default: return ccls_lambda;
    }
}
