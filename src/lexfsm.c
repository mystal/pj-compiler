#include "lexfsm.h"

#include <ctype.h>
#include <stdio.h>

#include "buffer.h"
#include "directive.h"
#include "state.h"
#include "token.h"

// Function pointer type for actions
typedef void (*action_func)(state, token *, char);

typedef struct __transition
{
    state next;
    action_func action;
} transition;

// Shortened action names
#define AAB acceptBuild
#define AAI acceptIntPushBack
#define AAP acceptPushBack
#define ABU build
#define ADI discard
#define AER error
#define AIG ignore
#define APD processDirective

/* Actions */
void acceptBuild(state, token *, char);
void acceptIntPushBack(state, token *, char);
void acceptPushBack(state, token *, char);
void build(state, token *, char);
void discard(state, token *, char);
void error(state, token *, char);
void ignore(state, token *, char);
void processDirective(state, token *, char);

/**
 * Transition table. Rows represent states, columns represent character
 * classes. A row's default transition is defined above it.
 **/
transition trans_table [state_num][ccls_num] =
{
          /* ad_fz, e    , 09   , pm   , ast  , eq   , lt    */
          /* gt   , lpar , rpar , acc  , dot  , colon, dsign */
          /* quote, carat, white, eol  , lmbd  */
#define DEF {BE,AER}
/* end */   {DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAB}
/* begin */ {{ID,ABU}, {ID,ABU}, {R0,ABU}, DEF, DEF, DEF, {LT,ABU},
             {GT,ABU}, {C0,ABU}, DEF, DEF, {DT,ABU}, {CL,ABU}, {BE,AER},
             {L0,AIG}, {BE,AER}, {BE,AIG}, {BE,AIG}, {BE,AER}},
#undef DEF
#define DEF {EN,AAP}
/* id */    {{ID,ABU}, {ID,ABU}, {ID,ABU}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, {EN,AAB}, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* real0 */ {DEF, DEF, {R0,ABU}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, {R1,ABU}, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {BE,AER}
/* real1 */ {DEF, DEF, {R2,ABU}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, {EN,AAI}, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* real2 */ {DEF, {R3,ABU}, {R2,ABU}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {BE,AER}
/* real3 */ {DEF, DEF, {R5,ABU}, {R4,ABU}, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {BE,AER}
/* real4 */ {DEF, DEF, {R5,ABU}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* real5 */ {DEF, DEF, {R5,ABU}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {L0,ABU}
/* lit0 */  {DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             {L1,AIG}, DEF, DEF, {BE,AER}, DEF},
#undef DEF
#define DEF {EN,AAP}
/* lit1 */  {DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             {L0,ABU}, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* com0 */  {DEF, DEF, DEF, DEF, {C1,ADI}, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {C2,AIG}
/* com1 */  {DEF, DEF, DEF, DEF, {C3,AIG}, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, {C2,APD},
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {C2,AIG}
/* com2 */  {DEF, DEF, DEF, DEF, {C3,AIG}, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {C2,AIG}
/* com3 */  {DEF, DEF, DEF, DEF, {C3,AIG}, DEF, DEF,
             DEF, DEF, {BE,AIG}, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* lt */    {DEF, DEF, DEF, DEF, DEF, {EN,AAB}, DEF,
             {EN,AAB}, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* gt */    {DEF, DEF, DEF, DEF, DEF, {EN,AAB}, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* dot */   {DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, {EN,AAB}, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF},
#undef DEF
#define DEF {EN,AAP}
/* colon */ {DEF, DEF, DEF, DEF, DEF, {EN,AAB}, DEF,
             DEF, DEF, DEF, DEF, DEF, DEF, DEF,
             DEF, DEF, DEF, DEF, DEF}
};

/* Helper functions */
char_class lookupCharClass(char);
token_kind classify(state s, char c);

state performAction(state s, char c, token *t)
{
    char_class ccls = lookupCharClass(c);
    trans_table[s][ccls].action(s, t, c);
    return trans_table[s][ccls].next;
}

/**
 * Appends the given character and accepts the current token.
 **/
void acceptBuild(state s, token *t, char c)
{
    stringAppend(&t->lexeme, c);
    t->kind = classify(s, c);
}

/**
 * Strips the last character from the lexeme (a dot), pushes back the current
 * character (a dot), and accepts the token (an integer constant).
 **/
void acceptIntPushBack(state s, token *t, char c)
{
    stringRemove(&t->lexeme, 1);
    bufferPushBack(2);
    t->kind = classify(s, c);
}

/**
 * Pushes back the current character and accepts the token.
 **/
void acceptPushBack(state s, token *t, char c)
{
    bufferPushBack(1);
    t->kind = classify(s, c);
}

/**
 * Builds the token by appending the given character.
 **/
void build(state s, token *t, char c)
{
    stringAppend(&t->lexeme, c);
}

/**
 * Discards the current token, preparing it to be built anew.
 **/
void discard(state s, token *t, char c)
{
    tokenClean(t);
    tokenInit(t);
}

/**
 * Discards the current token, preparing it to be built anew. Also prints
 * an error message and the line the error occurred on.
 **/
void error(state s, token *t, char c)
{
    if (c == '\n')
        fprintf(stderr, "Unexpected symbol \'\\n\'");
    else
        fprintf(stderr, "Unexpected symbol \'%c\'", c);
    fprintf(stderr, " at (%d,%d):\n", bufferLineNumber(), bufferPos());
    bufferPrint(stderr);
    tokenClean(t);
    tokenInit(t);
}

/**
 * Ignores the given character. Nothing is changed.
 **/
void ignore(state s, token *t, char c)
{
}

/**
 * Processes directives, expecting them to be well-formed. The given character
 * should be a dollar sign.
 **/
void processDirective(state s, token *t, char c)
{
    char test, d, f;
    do
    {
        d = bufferGetChar();
        f = bufferGetChar();
        if (d == 'l' || d == 'L')
        {
            if (f == '+')
                directives[dir_listing] = true;
            else
                directives[dir_listing] = false;
        }
        else if (d == 't' || d == 'T')
        {
            if (f == '+')
                directives[dir_token_echo] = true;
            else
                directives[dir_token_echo] = false;
        }
        else if (d == 'p' || d == 'P')
        {
            if (f == '+')
                directives[dir_print_reduction] = true;
            else
                directives[dir_print_reduction] = false;
        }
        else if (d == 'f' || d == 'F')
        {
            if (f == '+')
                directives[dir_flushed_echo] = true;
            else
                directives[dir_flushed_echo] = false;
        }
        test = bufferGetChar();
    } while (test == ',');
    bufferPushBack(1);
}

/**
 * Classifies the type of token given the current state and input. This should
 * only be called when accepting, therefore tok_undef should never be returned.
 **/
token_kind classify(state s, char c)
{
    if (s == state_begin)
        switch (c)
        {
            case '+': return tok_plus;
            case '-': return tok_minus;
            case '*': return tok_asterisk;
            case '/': return tok_slash;
            case '=': return tok_equal;
            case ')': return tok_rparen;
            case '[': return tok_lbrack;
            case ']': return tok_rbrack;
            case ',': return tok_comma;
            case ';': return tok_semicolon;
        }
    else if (s == state_id)
        switch (c)
        {
            case '^': return tok_fileid;
            default: return tok_id;
        }
    else if (s == state_real0 || s == state_real1)
        return tok_integer_const;
    else if (s == state_real2 || s == state_real5)
        return tok_real_const;
    else if (s == state_lit1)
        return tok_literal;
    else if (s == state_com0)
        return tok_lparen;
    else if (s == state_lt)
        switch (c)
        {
            case '>': return tok_nequal;
            case '=': return tok_ltequal;
            default: return tok_lthan;
        }
    else if (s == state_gt)
        switch (c)
        {
            case '=': return tok_gtequal;
            default: return tok_gthan;
        }
    else if (s == state_dot)
        switch (c)
        {
            case '.': return tok_rdots;
            default: return tok_dot;
        }
    else if (s == state_colon)
        switch (c)
        {
            case '=': return tok_colonequal;
            default: return tok_colon;
        }
    return tok_undef;
}

/**
 * Looks up the character class for the given character.
 **/
char_class lookupCharClass(char c)
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
        return ccls_white;
    switch (c)
    {
        case '+':
        case '-': return ccls_pm;
        case '*': return ccls_ast;
        case '=': return ccls_eq;
        case '<': return ccls_lt;
        case '>': return ccls_gt;
        case '(': return ccls_lpar;
        case ')': return ccls_rpar;
        case '/':
        case '[':
        case ']':
        case ',':
        case ';': return ccls_acc;
        case '.': return ccls_dot;
        case ':': return ccls_colon;
        case '$': return ccls_dsign;
        case '\'': return ccls_quote;
        case '^': return ccls_carat;
        default: return ccls_lmbd;
    }
}
