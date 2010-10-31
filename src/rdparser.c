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
        unsigned int pos = bufferPos(); \
        fprintf(stdout, "(%d, %d): error: %s '%.*s', expected %s\n", \
                bufferLineNumber(), pos - t->lexeme.len + 1, \
                errorString(err_unex), t->lexeme.len, \
                t->lexeme.buffer, tokenKindString(tok)); \
        bufferPrint(stdout); \
        for (int i = 0; i < pos - t->lexeme.len; i++) \
            fprintf(stdout, " "); \
        for (int i = 0; i < t->lexeme.len; i++) \
            fprintf(stdout, "^"); \
        fprintf(stdout, "\n"); \
        errorRecovery(); \
        goto label; \
    }

#define EXPECT(tok) \
    if (t->kind != tok) \
    { \
        fprintf(stdout, "(%d, %d): error: %s '%.*s', expected %s\n", \
                bufferLineNumber(), bufferPos() - t->lexeme.len, \
                errorString(err_unex), t->lexeme.len, \
                t->lexeme.buffer, tokenKindString(tok)); \
        bufferPrint(stdout); \
    }

/**
 * Recursive Descent Parser Grammar (EBNF)
 *
 * program -> 'PROGRAM' id '(' prog_arg_list ')' ';' block '.'
 * prog_arg_list -> id {',' id}
 * block -> ['CONST' const_decl {const_decl}]
 *          ['VAR' var_decl {var_decl}]
 *          {'PROCEDURE' proc}
 *          'BEGIN' stmt_list 'END'
 * const_decl -> id '=' constant ';'
 * var_decl -> var_id_list type_decl ';'
 * var_id_list -> id {',' id}
 * type_decl -> ':' ['ARRAY' range_decl 'OF'] stype
 * proc -> id ['(' param_list ')'] ';' block ';'
 * param_list -> param {';' param}
 * param -> id type_decl
 * range_decl -> '[' (int|id) '..' (int|id) ']'
 * stmt_list -> stmt {';' stmt}
 * stmt -> id id_stmt
 *      -> fileptr fileptr_stmt
 *      -> 'IF' if_stmt
 *      -> 'WHILE' while_stmt
 *      -> 'FOR' for_stmt
 *      -> 'BEGIN' [stmt_list] 'END'
 * id_stmt -> ':=' expr
 *         -> '[' expr ']' ':=' expr
 *         -> ['(' arg_list ')']
 * fileptr_stmt -> ':=' expr
 * if_stmt -> expr 'THEN' stmt ['ELSE' stmt]
 * while_stmt -> expr 'DO' stmt
 * for_stmt -> id ':=' expr ('DOWNTO'|'TO') expr 'DO' stmt
 * arg_list -> expr {',' expr}
 * stype -> ('INTEGER'|'REAL'|'ALFA'|'BOOLEAN'|'CHAR'|'TEXT')
 * constant -> (real|integer|boolean|alfa|char|string)
 **/

/* Private helper functions */
void errorRecovery(void);
void initStopSet(void);

void program(void);
void prog_arg_list(void);
void block(void);
void const_decl(void);
void var_decl(void);
void var_id_list(void);
void type_decl(void);
void proc(void);
void param_list(void);
void param(void);
void range_decl(void);
void stmt_list(void);
void stmt(void);
void id_stmt(void);
void fileptr_stmt(void);
void if_stmt(void);
void while_stmt(void);
void for_stmt(void);
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
    initStopSet();
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
    while (t->kind != tok_undef && !bstContains(followSet, t->kind)
           && !bstContains(stopSet, t->kind))
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
    bstInsert(stopSet, tok_kw_begin);
    bstInsert(stopSet, tok_kw_const);
    bstInsert(stopSet, tok_kw_do);
    bstInsert(stopSet, tok_kw_else);
    bstInsert(stopSet, tok_kw_end);
    bstInsert(stopSet, tok_kw_for);
    bstInsert(stopSet, tok_kw_if);
    bstInsert(stopSet, tok_kw_procedure);
    bstInsert(stopSet, tok_kw_program);
    bstInsert(stopSet, tok_kw_then);
    bstInsert(stopSet, tok_kw_var);
    bstInsert(stopSet, tok_kw_while);
}

void program()
{
    dirTrace("program", tr_enter);
    EXPECT_GOTO(tok_kw_program, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_id, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_lparen, blockStart);
    t = lexerGetToken();
    prog_arg_list();
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

void prog_arg_list()
{
    dirTrace("prog_arg_list", tr_enter);
    bstInsert(followSet, tok_rparen);
    EXPECT_GOTO(tok_id, prog_arg_listEnd);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, prog_arg_listEnd);
        t = lexerGetToken();
    }
prog_arg_listEnd:
    bstRemove(followSet, tok_rparen);
    dirTrace("prog_arg_list", tr_exit);
}

void block()
{
    dirTrace("block", tr_enter);
    bstInsert(followSet, tok_dot);
    bstInsert(followSet, tok_semicolon);
    if (t->kind == tok_kw_const)
    {
        t = lexerGetToken();
        const_decl();
        while (t->kind == tok_id)
            const_decl();
    }
    if (t->kind == tok_kw_var)
    {
        t = lexerGetToken();
        var_decl();
        while (t->kind == tok_id)
            var_decl();
    }
    while (t->kind == tok_kw_procedure)
    {
        t = lexerGetToken();
        proc();
    }
    bstInsert(followSet, tok_semicolon);
    bstInsert(followSet, tok_id);
    EXPECT_GOTO(tok_kw_begin, stmt_listStart);
    t = lexerGetToken();
stmt_listStart:
    bstRemove(followSet, tok_id);
    stmt_list();
    bstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_kw_end, blockEnd);
    t = lexerGetToken();
blockEnd:
    bstRemove(followSet, tok_dot);
    bstRemove(followSet, tok_semicolon);
    dirTrace("block", tr_exit);
}

void const_decl()
{
    dirTrace("const_decl", tr_enter);
    bstInsert(followSet, tok_id);
    EXPECT_GOTO(tok_id, const_declSemi);
    t = lexerGetToken();
    EXPECT_GOTO(tok_equal, const_declSemi);
    t = lexerGetToken();
    constant();
const_declSemi:
    EXPECT_GOTO(tok_semicolon, const_declEnd);
    t = lexerGetToken();
const_declEnd:
    bstRemove(followSet, tok_id);
    dirTrace("const_decl", tr_exit);
}

void var_decl()
{
    dirTrace("var_decl", tr_enter);
    bstInsert(followSet, tok_id);
    var_id_list();
    type_decl();
    EXPECT_GOTO(tok_semicolon, var_declEnd);
    t = lexerGetToken();
var_declEnd:
    bstRemove(followSet, tok_id);
    dirTrace("var_decl", tr_exit);
}

void var_id_list()
{
    dirTrace("var_id_list", tr_enter);
    bstInsert(followSet, tok_colon);
    EXPECT_GOTO(tok_id, var_id_listEnd);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, var_id_listEnd);
        t = lexerGetToken();
    }
var_id_listEnd:
    bstRemove(followSet, tok_colon);
    dirTrace("var_id_list", tr_exit);
}

void type_decl()
{
    dirTrace("type_decl", tr_enter);
    bstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_colon, type_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_kw_array)
    {
        t = lexerGetToken();
        range_decl();
        EXPECT_GOTO(tok_kw_of, type_declEnd);
        t = lexerGetToken();
    }
    stype();
type_declEnd:
    bstRemove(followSet, tok_semicolon);
    dirTrace("type_decl", tr_exit);
}

void proc()
{
    dirTrace("proc", tr_enter);
    bstInsert(followSet, tok_lparen);
    EXPECT_GOTO(tok_id, param_listStart);
    t = lexerGetToken();
param_listStart:
    bstRemove(followSet, tok_lparen);
    if (t->kind == tok_lparen)
    {
        t = lexerGetToken();
        param_list();
        EXPECT_GOTO(tok_rparen, param_listEnd);
        t = lexerGetToken();
    }
param_listEnd:
    EXPECT_GOTO(tok_semicolon, procBlockStart);
    t = lexerGetToken();
procBlockStart:
    block();
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
    bstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_id, paramEnd);
    t = lexerGetToken();
    type_decl();
paramEnd:
    bstRemove(followSet, tok_semicolon);
    dirTrace("param", tr_exit);
}

void range_decl()
{
    dirTrace("range_decl", tr_enter);
    bstInsert(followSet, tok_kw_of);
    EXPECT_GOTO(tok_lbrack, range_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_integer_const || t->kind == tok_id)
        t = lexerGetToken();
    else
        error(err_exp_idint, t);
    EXPECT_GOTO(tok_rdots, range_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_integer_const || t->kind == tok_id)
        t = lexerGetToken();
    else
        error(err_exp_idint, t);
    EXPECT_GOTO(tok_rbrack, range_declEnd);
    t = lexerGetToken();
range_declEnd:
    bstRemove(followSet, tok_kw_of);
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
    bstInsert(followSet, tok_semicolon);
    if (t->kind == tok_id)
    {
        t = lexerGetToken();
        id_stmt();
    }
    else if (t->kind == tok_fileid)
    {
        t = lexerGetToken();
        fileptr_stmt();
    }
    else if (t->kind == tok_kw_if)
    {
        t = lexerGetToken();
        if_stmt();
    }
    else if (t->kind == tok_kw_while)
    {
        t = lexerGetToken();
        while_stmt();
    }
    else if (t->kind == tok_kw_for)
    {
        t = lexerGetToken();
        for_stmt();
    }
    else if (t->kind == tok_kw_begin)
    {
        t = lexerGetToken();
        if (t->kind != tok_kw_end)
            stmt_list();
        EXPECT_GOTO(tok_kw_end, stmtEnd);
        t = lexerGetToken();
    }
    else
        error(err_badstmt, t);
stmtEnd:
    bstRemove(followSet, tok_semicolon);
    dirTrace("stmt", tr_exit);
}

void id_stmt()
{
    dirTrace("id_stmt", tr_enter);
    if (t->kind == tok_colonequal)
    {
        t = lexerGetToken();
        expr(t);
    }
    else if (t->kind == tok_lbrack)
    {
        t = lexerGetToken();
        expr(t);
        EXPECT_GOTO(tok_rbrack, id_stmtEnd);
        t = lexerGetToken();
        EXPECT_GOTO(tok_colonequal, id_stmtEnd);
        t = lexerGetToken();
        expr(t);
    }
    else if (t->kind == tok_lparen)
    {
        t = lexerGetToken();
        arg_list();
        EXPECT_GOTO(tok_rparen, id_stmtEnd);
        t = lexerGetToken();
    }
    //else, procedure call with no arguments
id_stmtEnd:
    dirTrace("id_stmt", tr_exit);
}

void fileptr_stmt()
{
    dirTrace("fileptr_stmt", tr_enter);
    EXPECT_GOTO(tok_colonequal, fileptr_stmtEnd);
    t = lexerGetToken();
    expr(t);
fileptr_stmtEnd:
    dirTrace("fileptr_stmt", tr_exit);
}

void if_stmt()
{
    dirTrace("if_stmt", tr_enter);
    expr(t);
    EXPECT_GOTO(tok_kw_then, if_stmtEnd);
    t = lexerGetToken();
    stmt();
    if (t->kind == tok_kw_else)
    {
        t = lexerGetToken();
        stmt();
    }
if_stmtEnd:
    dirTrace("if_stmt", tr_exit);
}

void while_stmt()
{
    dirTrace("while_stmt", tr_enter);
    expr(t);
    EXPECT_GOTO(tok_kw_do, while_stmtEnd);
    t = lexerGetToken();
    stmt();
while_stmtEnd:
    dirTrace("while_stmt", tr_exit);
}

void for_stmt()
{
    dirTrace("for_stmt", tr_enter);
    EXPECT_GOTO(tok_id, for_stmtEnd);
    t = lexerGetToken();
    EXPECT_GOTO(tok_colonequal, for_stmtEnd);
    t = lexerGetToken();
    expr(t);
    if (t->kind == tok_kw_downto || t->kind == tok_kw_to)
        t = lexerGetToken();
    else
        error(err_exp_downto, t);
    expr(t);
    EXPECT_GOTO(tok_kw_do, for_stmtEnd);
    t = lexerGetToken();
    stmt();
for_stmtEnd:
    dirTrace("for_stmt", tr_exit);
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
