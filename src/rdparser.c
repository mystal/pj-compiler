#include "rdparser.h"

#include "buffer.h"
#include "directive.h"
#include "error.h"
#include "exprparser.h"
#include "lexer.h"
#include "token.h"

#define EXPECT(tok) \
    if (t->kind != tok) \
    { \
        fprintf(stdout, "error: %s '%.*s' found at (%d, %d)\n", \
                errorString(err_unextoken), t->lexeme.len, \
                t->lexeme.buffer, bufferLineNumber(), \
                bufferPos()-t->lexeme.len); \
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

void program(token *);
void block(token *);
void var_decl(token *);
void proc(token *);
void param_list(token *);
void param(token *);
void range_decl(token *);
void stmt_list(token *);
void stmt(token *);
void arg_list(token *);
void stype(token *);
void constant(token *);

void parse()
{
    lexerInit();
    token *t = lexerGetToken();
    program(t);
    lexerCleanup();
}

void program(token *t)
{
    dirTrace("program", tr_enter);
    EXPECT(tok_kw_program);
    t = lexerGetToken();
    EXPECT(tok_id);
    t = lexerGetToken();
    EXPECT(tok_lparen);
    t = lexerGetToken();
    EXPECT(tok_id);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT(tok_id);
        t = lexerGetToken();
    }
    EXPECT(tok_rparen);
    t = lexerGetToken();
    EXPECT(tok_semicolon);
    t = lexerGetToken();
    block(t);
    EXPECT(tok_dot);
    dirTrace("program", tr_exit);
}

void block(token *t)
{
    dirTrace("block", tr_enter);
    if (t->kind == tok_kw_const)
    {
        t = lexerGetToken();
        EXPECT(tok_id);
        t = lexerGetToken();
        EXPECT(tok_equal);
        t = lexerGetToken();
        constant(t);
        EXPECT(tok_semicolon);
        t = lexerGetToken();
        while (t->kind == tok_id)
        {
            t = lexerGetToken();
            EXPECT(tok_equal);
            t = lexerGetToken();
            constant(t);
            EXPECT(tok_semicolon);
            t = lexerGetToken();
        }
    }
    if (t->kind == tok_kw_var)
    {
        t = lexerGetToken();
        var_decl(t);
        while (t->kind == tok_id)
            var_decl(t);
    }
    while (t->kind == tok_kw_procedure)
    {
        t = lexerGetToken();
        proc(t);
    }
    EXPECT(tok_kw_begin);
    t = lexerGetToken();
    stmt_list(t);
    EXPECT(tok_kw_end);
    t = lexerGetToken();
    dirTrace("block", tr_exit);
}

void var_decl(token *t)
{
    dirTrace("var_decl", tr_enter);
    EXPECT(tok_id);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT(tok_id);
        t = lexerGetToken();
    }
    EXPECT(tok_colon);
    t = lexerGetToken();
    if (t->kind == tok_kw_array)
    {
        t = lexerGetToken();
        range_decl(t);
        EXPECT(tok_kw_of);
        t = lexerGetToken();
    }
    stype(t);
    EXPECT(tok_semicolon);
    t = lexerGetToken();
    dirTrace("var_decl", tr_exit);
}

void proc(token *t)
{
    dirTrace("proc", tr_enter);
    EXPECT(tok_id);
    t = lexerGetToken();
    if (t->kind == tok_lparen)
    {
        t = lexerGetToken();
        param_list(t);
        EXPECT(tok_rparen);
        t = lexerGetToken();
    }
    EXPECT(tok_semicolon);
    t = lexerGetToken();
    block(t);
    EXPECT(tok_semicolon);
    t = lexerGetToken();
    dirTrace("proc", tr_exit);
}

void param_list(token *t)
{
    dirTrace("param_list", tr_enter);
    param(t);
    while (t->kind == tok_semicolon)
    {
        t = lexerGetToken();
        param(t);
    }
    dirTrace("param_list", tr_exit);
}

void param(token *t)
{
    dirTrace("param", tr_enter);
    EXPECT(tok_id);
    t = lexerGetToken();
    EXPECT(tok_colon);
    t = lexerGetToken();
    if (t->kind == tok_kw_array)
    {
        t = lexerGetToken();
        range_decl(t);
        EXPECT(tok_kw_of);
        t = lexerGetToken();
    }
    stype(t);
    dirTrace("param", tr_exit);
}

void range_decl(token *t)
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

void stmt_list(token *t)
{
    dirTrace("stmt_list", tr_enter);
    stmt(t);
    while (t->kind == tok_semicolon)
    {
        t = lexerGetToken();
        stmt(t);
    }
    dirTrace("stmt_list", tr_exit);
}

void stmt(token *t)
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
            arg_list(t);
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
        stmt(t);
        if (t->kind == tok_kw_else)
        {
            t = lexerGetToken();
            stmt(t);
        }
    }
    else if (t->kind == tok_kw_while)
    {
        t = lexerGetToken();
        expr(t);
        EXPECT(tok_kw_do);
        t = lexerGetToken();
        stmt(t);
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
        stmt(t);
    }
    else if (t->kind == tok_kw_begin)
    {
        t = lexerGetToken();
        if (t->kind != tok_kw_end)
        {
            stmt_list(t);
            if (t->kind == tok_kw_end)
                t = lexerGetToken();
        }
        else
            t = lexerGetToken();
    }
    else
        ; //error
    dirTrace("stmt", tr_exit);
}

void arg_list(token *t)
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

void stype(token *t)
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
        default:
            //error
            break;
    }
    dirTrace("stype", tr_exit);
}

void constant(token *t)
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
        default:
            //error
            break;
    }
    dirTrace("constant", tr_exit);
}
