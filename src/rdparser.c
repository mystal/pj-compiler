#include "rdparser.h"

#include "bst.h"
#include "buffer.h"
#include "directive.h"
#include "error.h"
#include "exprparser.h"
#include "lexer.h"
//#include "rbtree.h"
#include "str.h"
#include "token.h"

#define EXPECT_GOTO(tok, label) \
    if (t->kind != tok) \
    { \
        fprintf(stdout, "(%d, %d): error: %s '%.*s', expected %s\n", \
                bufferLineNumber(), bufferPos() - t->lexeme.len, \
                errorString(err_unextoken), t->lexeme.len, \
                t->lexeme.buffer, tokenKindString(tok)); \
        bufferPrint(stdout); \
        errorRecovery(); \
        goto label; \
    }

#define EXPECT(tok) \
    if (t->kind != tok) \
    { \
        fprintf(stdout, "(%d, %d): error: %s '%.*s', expected %s\n", \
                bufferLineNumber(), bufferPos() - t->lexeme.len, \
                errorString(err_unextoken), t->lexeme.len, \
                t->lexeme.buffer, tokenKindString(tok)); \
        bufferPrint(stdout); \
    }

/**
 * Recursive Descent Parser Grammar (EBNF)
 *
 * program -> 'PROGRAM' id '(' id {',' id} ')' ';' block '.'
 * block -> ['CONST' id '=' constant ';' {id '=' constant ';'}]
 *          ['VAR' var_decl {var_decl}]
 *          {'PROCEDURE' proc}
 *          'BEGIN' stmt_list 'END'
 * var_decl -> id {',' id} ':' ['ARRAY' range_decl 'OF'] stype ';'
 * proc -> id ['(' param_list ')'] ';' block ';'
 * param_list -> param {';' param}
 * param -> id ':' ['ARRAY' range_decl 'OF'] stype
 * range_decl -> '[' (int|id) '..' (int|id) ']'
 * stmt_list -> stmt {';' stmt}
 * stmt -> id ':=' expr
 *      -> fileptr ':=' expr
 *      -> id ['(' arg_list ')']
 *      -> id '[' expr ']' ':=' expr
 *      -> 'IF' expr 'THEN' stmt ['ELSE' stmt]
 *      -> 'WHILE' expr 'DO' stmt
 *      -> 'FOR' id ':=' expr ('DOWNTO'|'TO') expr 'DO' stmt
 *      -> 'BEGIN' [stmt_list] 'END'
 * arg_list -> expr {',' expr}
 * stype -> ('INTEGER'|'REAL'|'ALFA'|'BOOLEAN'|'CHAR'|'TEXT')
 * constant -> (real|integer|boolean|alfa|char|string)
 **/

/* Private helper functions */
void errorRecovery(void);
void initStopSet(void);

void program(void);
void block(void);
void var_decl(void);
void proc(void);
void param_list(void);
void param(void);
void range_decl(void);
void stmt_list(void);
void stmt(void);
void arg_list(void);
void stype(void);
void constant(void);

/**
 * Element comparison function required by rbtree.
 **/
/*int cmp(void *e1, void *e2)
{
}*/

/* Private variables used by parser */
token *t;
bst *followSet;
bst *stopSet;

void parse()
{
    lexerInit();
    followSet = bstCreate();
    stopSet = bstCreate();
    //initStopSet();
    t = lexerGetToken();
    program();
    lexerCleanup();
    bstDestroy(followSet);
    bstDestroy(stopSet);
}

void errorRecovery()
{
    string s;
    if (directives[dir_rd_flush_echo])
        stringInit(&s);
    while (t->kind != tok_undef && !bstContains(stopSet, t->kind))
    {
        if (directives[dir_rd_flush_echo])
        {
            stringAppendChar(&s, ' ');
            stringAppendString(&s, t->lexeme.buffer, t->lexeme.len);
        }
        t = lexerGetToken();
    }
    if (directives[dir_rd_flush_echo])
    {
        fprintf(stdout, "\tFlushed Tokens:%.*s\n", s.len, s.buffer);
        stringFree(&s);
    }
}

void initStopSet()
{
    bstInsert(stopSet, tok_kw_alfa);
    bstInsert(stopSet, tok_kw_and);
    bstInsert(stopSet, tok_kw_array);
    bstInsert(stopSet, tok_kw_begin);
    bstInsert(stopSet, tok_kw_boolean);
    bstInsert(stopSet, tok_kw_char);
    bstInsert(stopSet, tok_kw_const);
    bstInsert(stopSet, tok_kw_div);
    bstInsert(stopSet, tok_kw_do);
    bstInsert(stopSet, tok_kw_downto);
    bstInsert(stopSet, tok_kw_else);
    bstInsert(stopSet, tok_kw_end);
    bstInsert(stopSet, tok_kw_false);
    bstInsert(stopSet, tok_kw_for);
    bstInsert(stopSet, tok_kw_if);
    bstInsert(stopSet, tok_kw_integer);
    bstInsert(stopSet, tok_kw_mod);
    bstInsert(stopSet, tok_kw_not);
    bstInsert(stopSet, tok_kw_of);
    bstInsert(stopSet, tok_kw_or);
    bstInsert(stopSet, tok_kw_procedure);
    bstInsert(stopSet, tok_kw_program);
    bstInsert(stopSet, tok_kw_real);
    bstInsert(stopSet, tok_kw_text);
    bstInsert(stopSet, tok_kw_then);
    bstInsert(stopSet, tok_kw_to);
    bstInsert(stopSet, tok_kw_true);
    bstInsert(stopSet, tok_kw_var);
    bstInsert(stopSet, tok_kw_while);
}

void program()
{
    dirTrace("program", tr_enter);
    bstInsert(stopSet, tok_kw_const);
    bstInsert(stopSet, tok_kw_var);
    bstInsert(stopSet, tok_kw_procedure);
    bstInsert(stopSet, tok_kw_begin);
    EXPECT_GOTO(tok_kw_program, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_id, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_lparen, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_id, blockStart);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, blockStart);
        t = lexerGetToken();
    }
    EXPECT_GOTO(tok_rparen, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_semicolon, blockStart);
    t = lexerGetToken();
blockStart:
    block();
    if (t->kind != tok_dot)
        error(err_exp_dot, t);
    dirTrace("program", tr_exit);
}

void block()
{
    dirTrace("block", tr_enter);
    bstInsert(stopSet, tok_kw_const);
    bstInsert(stopSet, tok_kw_var);
    bstInsert(stopSet, tok_kw_procedure);
    bstInsert(stopSet, tok_kw_begin);
    if (t->kind == tok_kw_const)
    {
        bstRemove(stopSet, tok_kw_const);
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, varStart);
        t = lexerGetToken();
        EXPECT_GOTO(tok_equal, varStart);
        t = lexerGetToken();
        constant();
        EXPECT_GOTO(tok_semicolon, varStart);
        t = lexerGetToken();
        while (t->kind == tok_id)
        {
            t = lexerGetToken();
            EXPECT_GOTO(tok_equal, varStart);
            t = lexerGetToken();
            constant();
            EXPECT_GOTO(tok_semicolon, varStart);
            t = lexerGetToken();
        }
    }
    bstRemove(stopSet, tok_kw_const);
varStart:
    if (t->kind == tok_kw_var)
    {
        bstRemove(stopSet, tok_kw_var);
        t = lexerGetToken();
        var_decl();
        while (t->kind == tok_id)
            var_decl();
    }
    bstRemove(stopSet, tok_kw_var);
procStart:
    while (t->kind == tok_kw_procedure)
    {
        bstRemove(stopSet, tok_kw_procedure);
        t = lexerGetToken();
        proc();
    }
    bstRemove(stopSet, tok_kw_procedure);
    bstInsert(stopSet, tok_id);
    bstInsert(stopSet, tok_fileid);
    bstInsert(stopSet, tok_kw_if);
    bstInsert(stopSet, tok_kw_while);
    bstInsert(stopSet, tok_kw_for);
    bstInsert(stopSet, tok_kw_begin);
    EXPECT_GOTO(tok_kw_begin, stmt_listStart);
stmt_listStart:
    bstRemove(stopSet, tok_kw_begin);
    t = lexerGetToken();
    stmt_list();
    bstRemove(stopSet, tok_id);
    bstRemove(stopSet, tok_fileid);
    bstRemove(stopSet, tok_kw_if);
    bstRemove(stopSet, tok_kw_while);
    bstRemove(stopSet, tok_kw_for);
    bstRemove(stopSet, tok_kw_begin);
    bstRemove(stopSet, tok_kw_for);
    bstRemove(stopSet, tok_kw_begin);
    bstInsert(stopSet, tok_semicolon);
    bstInsert(stopSet, tok_dot);
    EXPECT_GOTO(tok_kw_end, blockEnd);
    t = lexerGetToken();
blockEnd:
    bstRemove(stopSet, tok_semicolon);
    bstRemove(stopSet, tok_dot);
    dirTrace("block", tr_exit);
}

void var_decl()
{
    dirTrace("var_decl", tr_enter);
    EXPECT_GOTO(tok_id, var_declEnd);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, var_declEnd);
        t = lexerGetToken();
    }
    bstInsert(stopSet, tok_id);
    EXPECT_GOTO(tok_colon, var_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_kw_array)
    {
        t = lexerGetToken();
        range_decl();
        EXPECT_GOTO(tok_kw_of, var_declEnd);
        t = lexerGetToken();
    }
    stype();
    EXPECT_GOTO(tok_semicolon, var_declEnd);
    t = lexerGetToken();
    bstRemove(stopSet, tok_id);
var_declEnd:
    dirTrace("var_decl", tr_exit);
}

void proc()
{
    dirTrace("proc", tr_enter);
    bstInsert(stopSet, tok_kw_const);
    bstInsert(stopSet, tok_kw_var);
    bstInsert(stopSet, tok_kw_procedure);
    bstInsert(stopSet, tok_lparen);
    EXPECT_GOTO(tok_id, procParam_listStart);
    t = lexerGetToken();
procParam_listStart:
    if (t->kind == tok_lparen)
    {
        bstRemove(stopSet, tok_lparen);
        t = lexerGetToken();
        bstInsert(stopSet, tok_rparen);
        param_list();
        bstRemove(stopSet, tok_rparen);
        EXPECT_GOTO(tok_rparen, procParam_listEnd);
        t = lexerGetToken();
    }
    bstRemove(stopSet, tok_lparen);
procParam_listEnd:
    EXPECT_GOTO(tok_semicolon, procBlockStart);
    t = lexerGetToken();
procBlockStart:
    block();
    bstRemove(stopSet, tok_kw_const);
    bstRemove(stopSet, tok_kw_var);
    bstRemove(stopSet, tok_kw_procedure);
    EXPECT_GOTO(tok_semicolon, procEnd);
    t = lexerGetToken();
procEnd:
    dirTrace("proc", tr_exit);
}

void param_list()
{
    dirTrace("param_list", tr_enter);
    param();
    while (t->kind == tok_semicolon)
    {
        t = lexerGetToken();
        param();
    }
    dirTrace("param_list", tr_exit);
}

void param()
{
    dirTrace("param", tr_enter);
    bstInsert(stopSet, tok_semicolon);
    EXPECT_GOTO(tok_id, paramEnd);
    t = lexerGetToken();
    EXPECT_GOTO(tok_colon, paramEnd);
    t = lexerGetToken();
    if (t->kind == tok_kw_array)
    {
        t = lexerGetToken();
        range_decl();
        EXPECT_GOTO(tok_kw_of, paramEnd);
        t = lexerGetToken();
    }
    stype();
paramEnd:
    bstRemove(stopSet, tok_semicolon);
    dirTrace("param", tr_exit);
}

void range_decl()
{
    dirTrace("range_decl", tr_enter);
    EXPECT(tok_lbrack);
    t = lexerGetToken();
    if (t->kind == tok_integer_const || t->kind == tok_id)
        t = lexerGetToken();
    else
        ; //error
    EXPECT(tok_rdots);
    t = lexerGetToken();
    if (t->kind == tok_integer_const || t->kind == tok_id)
        t = lexerGetToken();
    else
        ; //error
    EXPECT(tok_rbrack);
    t = lexerGetToken();
    dirTrace("range_decl", tr_exit);
}

void stmt_list()
{
    dirTrace("stmt_list", tr_enter);
    stmt();
    while (t->kind == tok_semicolon)
    {
        t = lexerGetToken();
        stmt();
    }
    dirTrace("stmt_list", tr_exit);
}

void stmt()
{
    dirTrace("stmt", tr_enter);
    if (t->kind == tok_id)
    {
        t = lexerGetToken();
        if (t->kind == tok_colonequal)
        {
            t = lexerGetToken();
            expr(t);
        }
        else if (t->kind == tok_lbrack)
        {
            t = lexerGetToken();
            expr(t);
            EXPECT(tok_rbrack);
            t = lexerGetToken();
            EXPECT(tok_colonequal);
            t = lexerGetToken();
            expr(t);
        }
        else if (t->kind == tok_lparen)
        {
            t = lexerGetToken();
            arg_list();
            EXPECT(tok_rparen);
            t = lexerGetToken();
        }
        //else, procedure call with no arguments
    }
    else if (t->kind == tok_fileid)
    {
        t = lexerGetToken();
        EXPECT(tok_colonequal);
        t = lexerGetToken();
        expr(t);
    }
    else if (t->kind == tok_kw_if)
    {
        t = lexerGetToken();
        expr(t);
        EXPECT(tok_kw_then);
        t = lexerGetToken();
        stmt();
        if (t->kind == tok_kw_else)
        {
            t = lexerGetToken();
            stmt();
        }
    }
    else if (t->kind == tok_kw_while)
    {
        t = lexerGetToken();
        expr(t);
        EXPECT(tok_kw_do);
        t = lexerGetToken();
        stmt();
    }
    else if (t->kind == tok_kw_for)
    {
        t = lexerGetToken();
        EXPECT(tok_id);
        t = lexerGetToken();
        EXPECT(tok_colonequal);
        t = lexerGetToken();
        expr(t);
        if (t->kind == tok_kw_downto || t->kind == tok_kw_to)
            t = lexerGetToken();
        else
            ; //error
        expr(t);
        EXPECT(tok_kw_do);
        t = lexerGetToken();
        stmt();
    }
    else if (t->kind == tok_kw_begin)
    {
        t = lexerGetToken();
        if (t->kind != tok_kw_end)
            stmt_list();
        EXPECT(tok_kw_end);
        t = lexerGetToken();
    }
    else
        ; //error
    dirTrace("stmt", tr_exit);
}

void arg_list()
{
    dirTrace("arg_list", tr_enter);
    expr(t);
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        expr(t);
    }
    dirTrace("arg_list", tr_exit);
}

void stype()
{
    dirTrace("stype", tr_enter);
    switch (t->kind)
    {
        case tok_kw_integer:
        case tok_kw_real:
        case tok_kw_alfa:
        case tok_kw_boolean:
        case tok_kw_char:
        case tok_kw_text:
            t = lexerGetToken();
            break;
        default:
            error(err_exp_stype, t);
            break;
    }
    dirTrace("stype", tr_exit);
}

void constant()
{
    dirTrace("constant", tr_enter);
    switch (t->kind)
    {
        case tok_real_const:
        case tok_integer_const:
        case tok_kw_true:
        case tok_kw_false:
        case tok_alfa_const:
        case tok_char_const:
        case tok_string_const:
            t = lexerGetToken();
            break;
        default:
            error(err_exp_const, t);
            break;
    }
    dirTrace("constant", tr_exit);
}
