#include "rdparser.h"

#include <stdlib.h>

#include "bst.h"
#include "buffer.h"
#include "directive.h"
#include "error.h"
#include "exprparser.h"
#include "lexer.h"
#include "list.h"
#include "str.h"
#include "symbol.h"
#include "symtable.h"
#include "token.h"
#include "tokenbst.h"

#define EXPECT_GOTO(tok, label) \
    if (t->kind != tok) \
    { \
        errorParse(err_unex, t, tok); \
        errorRecovery(); \
        goto label; \
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
void addFilename(bst *, string *);
int stringToInt(string *);

/* print_func for strings */
void bstPrintString(void *);

/* del_func for strings */
void bstDelString(void *);

void program(void);
void prog_arg_list(bst *);
void block(bst *);
void const_decl(void);
void var_decl(bst *);
void var_id_list(void);
void type_decl(sym_type *, unsigned int *, unsigned int *, pjtype *);
void proc(void);
void param_list(list *);
void param(list *);
void range_decl(unsigned int *, unsigned int *);
void stmt_list(void);
void stmt(void);
void id_stmt(void);
void fileptr_stmt(void);
void if_stmt(void);
void while_stmt(void);
void for_stmt(void);
void arg_list(void);
pjtype stype(void);
pjtype constant(void);

/* cmp_func for bst */
int stringCompareTo(void *, void *);

/* Private variables used by parser */
token *t;
symtable *st;
list *ids;
tokenbst *followSet;
tokenbst *stopSet;

void parse()
{
    lexerInit();
    st = stCreate();
    if (directives[dir_sym_table])
        stPrintBlocks(st, 1);
    ids = listCreate();
    followSet = tokenbstCreate();
    stopSet = tokenbstCreate();
    initStopSet();
    t = lexerGetToken();
    program();
    tokenbstDestroy(followSet);
    tokenbstDestroy(stopSet);
    listDestroy(ids);
    stDestroy(st);
    lexerCleanup();
}

void errorRecovery()
{
    string *s;
    if (directives[dir_rd_flush_echo])
        s = stringCreate();
    while (t->kind != tok_undef && !tokenbstContains(followSet, t->kind)
           && !tokenbstContains(stopSet, t->kind))
    {
        if (directives[dir_rd_flush_echo])
        {
            stringAppendChar(s, ' ');
            stringAppendString(s, t->lexeme);
        }
        t = lexerGetToken();
    }
    if (directives[dir_rd_flush_echo])
    {
        fprintf(stdout, "\tFlushed Tokens:%.*s\n", stringGetLength(s),
                stringGetBuffer(s));
        stringDestroy(s);
    }
}

void initStopSet()
{
    tokenbstInsert(stopSet, tok_kw_begin);
    tokenbstInsert(stopSet, tok_kw_const);
    tokenbstInsert(stopSet, tok_kw_do);
    tokenbstInsert(stopSet, tok_kw_else);
    tokenbstInsert(stopSet, tok_kw_end);
    tokenbstInsert(stopSet, tok_kw_for);
    tokenbstInsert(stopSet, tok_kw_if);
    tokenbstInsert(stopSet, tok_kw_procedure);
    tokenbstInsert(stopSet, tok_kw_program);
    tokenbstInsert(stopSet, tok_kw_then);
    tokenbstInsert(stopSet, tok_kw_var);
    tokenbstInsert(stopSet, tok_kw_while);
}

void addFilename(bst *t, string *str)
{
    if (stringCompareCharArray(str, "input", 5) != 0 &&
        stringCompareCharArray(str, "output", 6) != 0)
    {
        string *filename = stringCreate();
        stringAppendString(filename, str);
        bstInsert(t, filename);
    }
}

void program()
{
    string *progName = stringCreate();
    bst *progFiles;
    dirTrace("program", tr_enter);
    EXPECT_GOTO(tok_kw_program, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_id, blockStart);
    stringAppendString(progName, t->lexeme);
    t = lexerGetToken();
    EXPECT_GOTO(tok_lparen, blockStart);
    t = lexerGetToken();
    progFiles = bstCreate(stringCompareTo);
    prog_arg_list(progFiles);
    EXPECT_GOTO(tok_rparen, blockStart);
    t = lexerGetToken();
    EXPECT_GOTO(tok_semicolon, blockStart);
    t = lexerGetToken();
blockStart:
    stEnterBlock(st, progName);
    block(progFiles);
    bstDestroy(progFiles, bstDelString);
    stExitBlock(st);
    stringDestroy(progName);
    if (t->kind != tok_dot)
        errorParse(err_exp_dot, t, tok_undef);
    dirTrace("program", tr_exit);
}

void prog_arg_list(bst *progFiles)
{
    dirTrace("prog_arg_list", tr_enter);
    tokenbstInsert(followSet, tok_rparen);
    EXPECT_GOTO(tok_id, prog_arg_listEnd);
    addFilename(progFiles, t->lexeme);
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, prog_arg_listEnd);
        addFilename(progFiles, t->lexeme);
        t = lexerGetToken();
    }
prog_arg_listEnd:
    tokenbstRemove(followSet, tok_rparen);
    dirTrace("prog_arg_list", tr_exit);
}

void block(bst *progFiles)
{
    dirTrace("block", tr_enter);
    tokenbstInsert(followSet, tok_dot);
    tokenbstInsert(followSet, tok_semicolon);
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
        var_decl(progFiles);
        while (t->kind == tok_id)
            var_decl(progFiles);
        //Check if progFiles is not empty
        if (progFiles != NULL && bstSize(progFiles) != 0)
        {
            //Undeclared files in program var section...
            errorST(err_undecl_file, NULL);
            bstPrint(progFiles, bstPrintString);
            fprintf(stdout, "\n");
        }
    }
    if (directives[dir_sym_table])
        stPrintBlocks(st, 2);
    while (t->kind == tok_kw_procedure)
    {
        t = lexerGetToken();
        proc();
    }
    tokenbstInsert(followSet, tok_semicolon);
    tokenbstInsert(followSet, tok_id);
    EXPECT_GOTO(tok_kw_begin, stmt_listStart);
    t = lexerGetToken();
stmt_listStart:
    tokenbstRemove(followSet, tok_id);
    stmt_list();
    tokenbstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_kw_end, blockEnd);
    t = lexerGetToken();
blockEnd:
    if (directives[dir_sym_table])
        stPrintBlocks(st, 2);
    tokenbstRemove(followSet, tok_dot);
    tokenbstRemove(followSet, tok_semicolon);
    dirTrace("block", tr_exit);
}

void const_decl()
{
    symbol *sym;
    dirTrace("const_decl", tr_enter);
    tokenbstInsert(followSet, tok_id);
    EXPECT_GOTO(tok_id, const_declSemi);
    sym = symbolCreate(t->lexeme);
    symbolSetType(sym, symt_const_var);
    t = lexerGetToken();
    EXPECT_GOTO(tok_equal, const_declSemi);
    t = lexerGetToken();
    symbolConstSetValue(sym, t->lexeme);
    symbolSetPJType(sym, constant());
    if (!stAddSymbol(st, sym))
    {
        //Print error, free symbol memory
        errorST(err_dup_sym, symbolGetName(sym));
        symbolDestroy(sym);
    }
const_declSemi:
    EXPECT_GOTO(tok_semicolon, const_declEnd);
    t = lexerGetToken();
const_declEnd:
    tokenbstRemove(followSet, tok_id);
    dirTrace("const_decl", tr_exit);
}

void var_decl(bst *progFiles)
{
    symbol *sym;
    sym_type symt;
    pjtype pjt;
    unsigned int low, up;
    dirTrace("var_decl", tr_enter);
    tokenbstInsert(followSet, tok_id);
    var_id_list();
    type_decl(&symt, &low, &up, &pjt);
    while (listSize(ids) != 0)
    {
        string *name;
        sym = (symbol *) listRemoveFront(ids); //Pull first symbol from list
        symbolSetType(sym, symt);
        symbolSetPJType(sym, pjt);
        if (symt == symt_array) //Set bounds if needed
            symbolArraySetBounds(sym, low, up);
        name = symbolGetName(sym);
        if (progFiles != NULL && (stringCompareCharArray(name, "input", 5) == 0
            || stringCompareCharArray(name, "output", 6) == 0))
        {
            //Ignore input and output when in program block
            symbolDestroy(sym);
        }
        else if (!stAddSymbol(st, sym))
        {
            //Print error, free symbol memory
            errorST(err_dup_sym, symbolGetName(sym));
            symbolDestroy(sym);
        }
        else if (progFiles != NULL && bstContains(progFiles, symbolGetName(sym)))
        {
            if (symbolGetType(sym) == symt_var &&
                symbolGetPJType(sym) == pj_text)
                bstRemove(progFiles, symbolGetName(sym));
            else
            {
                //Print error, file must be declared of type text
                errorST(err_file_not_text, symbolGetName(sym));
            }
        }
    }
    EXPECT_GOTO(tok_semicolon, var_declEnd);
    t = lexerGetToken();
var_declEnd:
    tokenbstRemove(followSet, tok_id);
    dirTrace("var_decl", tr_exit);
}

void var_id_list()
{
    symbol *sym;
    dirTrace("var_id_list", tr_enter);
    tokenbstInsert(followSet, tok_colon);
    EXPECT_GOTO(tok_id, var_id_listEnd);
    sym = symbolCreate(t->lexeme);
    listAddBack(ids, sym);
    sym = NULL;
    t = lexerGetToken();
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        EXPECT_GOTO(tok_id, var_id_listEnd);
        sym = symbolCreate(t->lexeme);
        listAddBack(ids, sym);
        sym = NULL;
        t = lexerGetToken();
    }
var_id_listEnd:
    tokenbstRemove(followSet, tok_colon);
    dirTrace("var_id_list", tr_exit);
}

void type_decl(sym_type *symt, unsigned int *low, unsigned int *up, pjtype *pjt)
{
    dirTrace("type_decl", tr_enter);
    tokenbstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_colon, type_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_kw_array)
    {
        *symt = symt_array;
        t = lexerGetToken();
        range_decl(low, up);
        EXPECT_GOTO(tok_kw_of, type_declEnd);
        t = lexerGetToken();
    }
    else
        *symt = symt_var;
    *pjt = stype();
type_declEnd:
    tokenbstRemove(followSet, tok_semicolon);
    dirTrace("type_decl", tr_exit);
}

void proc()
{
    string *procName;
    symbol *sym;
    list *params = listCreate();
    dirTrace("proc", tr_enter);
    tokenbstInsert(followSet, tok_lparen);
    EXPECT_GOTO(tok_id, param_listStart);
    procName = stringCreate(); //Get procedure name
    stringAppendString(procName, t->lexeme);
    sym = symbolCreate(procName); //Create new symbol for procedure
    symbolSetType(sym, symt_proc);
    t = lexerGetToken();
param_listStart:
    tokenbstRemove(followSet, tok_lparen);
    if (t->kind == tok_lparen)
    {
        unsigned int numParams;
        pjtype *paramTypes;
        bool *optParams;
        t = lexerGetToken();
        param_list(params); //Process parameters
        //Add params to procedure
        numParams = listSize(params);
        paramTypes = (pjtype *) malloc(sizeof(pjtype)*numParams);
        optParams = (bool *) malloc(sizeof(bool)*numParams);
        for (unsigned int i = 0; i < numParams; i++)
        {
            paramTypes[i] = symbolGetPJType((symbol *) listGet(params, i));
            optParams[i] = false;
        }
        symbolProcSetParams(sym, numParams, paramTypes, optParams);
        EXPECT_GOTO(tok_rparen, param_listEnd);
        t = lexerGetToken();
    }
    if (!stAddSymbol(st, sym)) //Try to add procedure symbol
    {
        //Print error, free memory
        errorST(err_dup_sym, symbolGetName(sym));
        symbolDestroy(sym);
    }
param_listEnd:
    EXPECT_GOTO(tok_semicolon, procBlockStart);
    t = lexerGetToken();
procBlockStart:
    sym = NULL;
    stEnterBlock(st, procName);
    //Add params to symbol table in new block
    while (listSize(params) != 0)
    {
        sym = (symbol *) listRemoveFront(params);
        if (!stAddSymbol(st, sym))
        {
            //Print error, free symbol memory
            errorST(err_dup_sym, symbolGetName(sym));
            symbolDestroy(sym);
        }
    }
    block(NULL);
    stExitBlock(st);
    stringDestroy(procName);
    EXPECT_GOTO(tok_semicolon, procEnd);
    t = lexerGetToken();
procEnd:
    listDestroy(params);
    dirTrace("proc", tr_exit);
}

void param_list(list *params)
{
    dirTrace("param_list", tr_enter);
    param(params);
    while (t->kind == tok_semicolon)
    {
        t = lexerGetToken();
        param(params);
    }
    dirTrace("param_list", tr_exit);
}

void param(list *params)
{
    symbol *sym;
    sym_type symt;
    pjtype pjt;
    unsigned int low, up;
    dirTrace("param", tr_enter);
    tokenbstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_id, paramEnd);
    sym = symbolCreate(t->lexeme); //Create new symbol for parameter
    t = lexerGetToken();
    type_decl(&symt, &low, &up, &pjt);
    symbolSetType(sym, symt);
    if (symt == symt_array)
        symbolArraySetBounds(sym, low, up);
    symbolSetPJType(sym, pjt);
    listAddBack(params, sym);
paramEnd:
    tokenbstRemove(followSet, tok_semicolon);
    dirTrace("param", tr_exit);
}

void range_decl(unsigned int *low, unsigned int *up)
{
    symbol *sym;
    dirTrace("range_decl", tr_enter);
    tokenbstInsert(followSet, tok_kw_of);
    EXPECT_GOTO(tok_lbrack, range_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_id)
    {
        //Lookup symbol
        sym = stLookup(st, t->lexeme);
        if (sym == NULL)
            errorST(err_undef_sym, t->lexeme);
        else if (!(symbolGetType(sym) == symt_const_var && 
                   symbolGetPJType(sym) == pj_integer))
            errorST(err_range_not_const, t->lexeme);
        *low = stringToInt(symbolConstGetValue(sym));
        t = lexerGetToken();
    }
    else if (t->kind == tok_integer_const)
    {
        *low = stringToInt(t->lexeme);
        t = lexerGetToken();
    }
    else
        errorParse(err_exp_idint, t, tok_undef);
    EXPECT_GOTO(tok_rdots, range_declEnd);
    t = lexerGetToken();
    if (t->kind == tok_id)
    {
        //Lookup symbol
        sym = stLookup(st, t->lexeme);
        if (sym == NULL)
            errorST(err_undef_sym, t->lexeme);
        else if (!(symbolGetType(sym) == symt_const_var && 
                   symbolGetPJType(sym) == pj_integer))
            errorST(err_range_not_const, t->lexeme);
        *up = stringToInt(symbolConstGetValue(sym));
        t = lexerGetToken();
    }
    else if (t->kind == tok_integer_const)
    {
        *up = stringToInt(t->lexeme);
        t = lexerGetToken();
    }
    else
        errorParse(err_exp_idint, t, tok_undef);
    EXPECT_GOTO(tok_rbrack, range_declEnd);
    t = lexerGetToken();
range_declEnd:
    tokenbstRemove(followSet, tok_kw_of);
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
    tokenbstInsert(followSet, tok_semicolon);
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
        errorParse(err_badstmt, t, tok_undef);
stmtEnd:
    tokenbstRemove(followSet, tok_semicolon);
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
        errorParse(err_exp_downto, t, tok_undef);
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

pjtype stype()
{
    pjtype pjt = pj_undef;
    dirTrace("stype", tr_enter);
    switch (t->kind)
    {
        case tok_kw_integer:
            pjt = pj_integer;
            break;
        case tok_kw_real:
            pjt = pj_real;
            break;
        case tok_kw_alfa:
            pjt = pj_alfa;
            break;
        case tok_kw_boolean:
            pjt = pj_boolean;
            break;
        case tok_kw_char:
            pjt = pj_char;
            break;
        case tok_kw_text:
            pjt = pj_text;
            break;
        default:
            break;
    }
    if (pjt != pj_undef)
        t = lexerGetToken();
    else
        errorParse(err_exp_stype, t, tok_undef);
    dirTrace("stype", tr_exit);
    return pjt;
}

pjtype constant()
{
    pjtype pjt = pj_undef;
    dirTrace("constant", tr_enter);
    switch (t->kind)
    {
        case tok_real_const:
            pjt = pj_real;
            break;
        case tok_integer_const:
            pjt = pj_integer;
            break;
        case tok_kw_true:
        case tok_kw_false:
            pjt = pj_boolean;
            break;
        case tok_alfa_const:
            pjt = pj_alfa;
            break;
        case tok_char_const:
            pjt = pj_char;
            break;
        case tok_string_const:
            pjt = pj_string;
            break;
        default:
            break;
    }
    if (pjt != pj_undef)
        t = lexerGetToken();
    else
        errorParse(err_exp_const, t, tok_undef);
    dirTrace("constant", tr_exit);
    return pjt;
}

int stringCompareTo(void *v1, void *v2)
{
    string *s1 = (string *) v1;
    string *s2 = (string *) v2;
    return stringCompareString(s1, s2);
}

int stringToInt(string *str)
{
    int ret;
    sscanf(stringGetBuffer(str), "%d", &ret);
    return ret;
}

void bstPrintString(void *v)
{
    string *str = (string *) v;
    fprintf(stdout, " %.*s", stringGetLength(str), stringGetBuffer(str));
}

void bstDelString(void *v)
{
    string *str = (string *) v;
    stringDestroy(str);
}
