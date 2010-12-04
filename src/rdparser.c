#include "rdparser.h"

#include <stdlib.h>

#include "bst.h"
#include "buffer.h"
#include "codegen.h"
#include "directive.h"
#include "error.h"
#include "exprparser.h"
#include "hypomac.h"
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
 * range_decl -> '[' range_const '..' range_const ']'
 * range_const -> (int|id)
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

/* Parsing procedures */
void program(void);
void prog_arg_list(bst *);
void block(bst *);
void const_decl(void);
void var_decl(bst *);
void var_id_list(list *);
void type_decl(sym_type *, unsigned int *, unsigned int *, pjtype *);
void proc(void);
void param_list(list *);
void param(list *);
void range_decl(unsigned int *, unsigned int *);
void range_const(unsigned int *);
void stmt_list(void);
void stmt(void);
void id_stmt(symbol *, unsigned int);
void fileptr_stmt(void);
void if_stmt(void);
void while_stmt(void);
void for_stmt(void);
void arg_list(void);
pjtype stype(void);
void constant(symbol *);

/* cmp_func for strings */
int bstCompareString(void *, void *);

/* print_func for strings */
void bstPrintString(void *);

/* del_func for strings */
void bstDelString(void *);

/* Private variables used by parser */
token *t;
symtable *st;
tokenbst *followSet;
tokenbst *stopSet;

void parse()
{
    lexerInit();
    codegenInit();
    st = stCreate();
    if (dirGet(dir_sym_table))
        stPrintBlocks(st, 1);
    followSet = tokenbstCreate();
    stopSet = tokenbstCreate();
    initStopSet();
    t = lexerGetToken();
    program();
    tokenbstDestroy(followSet);
    tokenbstDestroy(stopSet);
    stDestroy(st);
    lexerCleanup();
    codegenCleanup();
}

void errorRecovery()
{
    string *s;
    if (dirGet(dir_rd_flush_echo))
        s = stringCreate();
    while (t->kind != tok_undef && !tokenbstContains(followSet, t->kind)
           && !tokenbstContains(stopSet, t->kind))
    {
        if (dirGet(dir_rd_flush_echo))
        {
            stringAppendChar(s, ' ');
            stringAppendString(s, t->lexeme);
        }
        t = lexerGetToken();
    }
    if (dirGet(dir_rd_flush_echo))
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
    if (stringCompareCharArray(str, "input", 5*sizeof(char)) != 0 &&
        stringCompareCharArray(str, "output", 6*sizeof(char)) != 0)
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
    progFiles = bstCreate(bstCompareString);
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
    if (dirGet(dir_sym_table))
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
    if (dirGet(dir_sym_table))
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
    t = lexerGetToken();
    EXPECT_GOTO(tok_equal, const_declSemi);
    t = lexerGetToken();
    constant(sym);
    if (symbolGetType(sym) == symt_undef)
    {
        symbolDestroy(sym);
    }
    else if (!stAddSymbol(st, sym))
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
    list *ids = listCreate();
    symbol *sym;
    sym_type symt;
    pjtype pjt;
    unsigned int low, up;
    dirTrace("var_decl", tr_enter);
    tokenbstInsert(followSet, tok_id);
    var_id_list(ids);
    type_decl(&symt, &low, &up, &pjt);
    while (listSize(ids) != 0)
    {
        string *name;
        sym = (symbol *) listRemoveFront(ids); //Pull first symbol from list
        //TODO Check for pjtype error?
        if (symt == symt_var)
            symbolSetVar(sym, pjt);
        else if (symt == symt_array)
            symbolSetArray(sym, pjt, low, up);
        name = symbolGetName(sym);
        if (progFiles != NULL && (stringCompareCharArray(name, "input", 5) == 0
            || stringCompareCharArray(name, "output", 6) == 0))
        {
            //TODO check that they are of correct types?
            //Ignore input and output when in program block
            symbolDestroy(sym);
        }
        else if (!stAddSymbol(st, sym))
        {
            //Print error, free symbol memory
            errorST(err_dup_sym, name);
            symbolDestroy(sym);
        }
        else if (progFiles != NULL && bstContains(progFiles, name))
        {
            string *filename;
            if (symt == symt_var && pjt == pj_text)
            {
                filename = (string *) bstGet(progFiles, name);
                bstRemove(progFiles, name);
                stringDestroy(filename);
            }
            else
            {
                //Print error, file must be declared of type text
                errorST(err_file_not_text, name);
            }
        }
    }
    EXPECT_GOTO(tok_semicolon, var_declEnd);
    t = lexerGetToken();
var_declEnd:
    tokenbstRemove(followSet, tok_id);
    listDestroy(ids);
    dirTrace("var_decl", tr_exit);
}

void var_id_list(list *ids)
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
    unsigned int numParams = 0;
    pjtype *paramTypes = NULL;
    symbol *sym;
    list *params = listCreate();
    dirTrace("proc", tr_enter);
    tokenbstInsert(followSet, tok_lparen);
    EXPECT_GOTO(tok_id, param_listStart);
    procName = stringCreate(); //Get procedure name
    stringAppendString(procName, t->lexeme);
    sym = symbolCreate(procName); //Create new symbol for procedure
    t = lexerGetToken();
param_listStart:
    tokenbstRemove(followSet, tok_lparen);
    if (t->kind == tok_lparen)
    {
        t = lexerGetToken();
        param_list(params); //Process parameters
        //Store params for procedure
        numParams = listSize(params);
        paramTypes = (pjtype *) malloc(sizeof(pjtype)*numParams);
        for (unsigned int i = 0; i < numParams; i++)
        {
            symbol *curParam = (symbol *) listGet(params, i);
            if (symbolGetType(curParam) == symt_var)
                paramTypes[i] = symVarGetType(curParam);
            else
                paramTypes[i] = symArrayGetType(curParam);
        }
        EXPECT_GOTO(tok_rparen, param_listEnd);
        t = lexerGetToken();
    }
    symbolSetProc(sym, numParams, paramTypes);
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
    if (symt == symt_var)
        symbolSetVar(sym, pjt);
    else if (symt == symt_array)
        symbolSetArray(sym, pjt, low, up);
    listAddBack(params, sym);
paramEnd:
    tokenbstRemove(followSet, tok_semicolon);
    dirTrace("param", tr_exit);
}

void range_decl(unsigned int *low, unsigned int *up)
{
    dirTrace("range_decl", tr_enter);
    tokenbstInsert(followSet, tok_kw_of);
    EXPECT_GOTO(tok_lbrack, range_declEnd);
    t = lexerGetToken();
    range_const(low);
    EXPECT_GOTO(tok_rdots, range_declEnd);
    t = lexerGetToken();
    range_const(up);
    EXPECT_GOTO(tok_rbrack, range_declEnd);
    t = lexerGetToken();
range_declEnd:
    tokenbstRemove(followSet, tok_kw_of);
    dirTrace("range_decl", tr_exit);
}

void range_const(unsigned int *bound)
{
    symbol *sym;
    if (t->kind == tok_id)
    {
        //Lookup symbol
        sym = stLookup(st, t->lexeme, NULL);
        if (sym == NULL)
            errorST(err_undef_sym, t->lexeme);
        else if (!(symbolGetType(sym) == symt_const_var && 
                   symConstVarGetType(sym) == pj_integer))
            errorST(err_range_not_const, t->lexeme);
        else
            *bound = stringToInt(symConstVarGetValue(sym));
    }
    else if (t->kind == tok_integer_const)
    {
        *bound = stringToInt(t->lexeme);
    }
    else
        errorParse(err_exp_idint, t, tok_undef);
    t = lexerGetToken();
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
        //TODO lookup id in symbol table
        unsigned int level;
        symbol *id = stLookup(st, t->lexeme, &level);
        t = lexerGetToken();
        id_stmt(id, level);
    }
    else if (t->kind == tok_fileid)
    {
        //TODO lookup id in symbol table
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

void id_stmt(symbol *id, unsigned int level)
{
    dirTrace("id_stmt", tr_enter);
    varval vv;
    if (t->kind == tok_colonequal) //id assignment
    {
        t = lexerGetToken();
        expr(t, st);
        //TODO type checking
        vv.type = h_int;
        vv.ircab_val.intval = codegenIdAddress(id, level);
        codegenInstruction(hop_pop, vv, 1);
    }
    else if (t->kind == tok_lbrack) //array value assignment
    {
        t = lexerGetToken();
        expr(t, st);
        //TODO codegenArrayAddress
        EXPECT_GOTO(tok_rbrack, id_stmtEnd);
        t = lexerGetToken();
        EXPECT_GOTO(tok_colonequal, id_stmtEnd);
        t = lexerGetToken();
        expr(t, st);
        //TODO type checking
        //TODO pop ADDR_ARRAY(id, int) 1
    }
    else if (t->kind == tok_lparen) //procedure call with arguments
    {
        //TODO procedure parameter type check
        //TODO procedure call code
        t = lexerGetToken();
        arg_list();
        EXPECT_GOTO(tok_rparen, id_stmtEnd);
        t = lexerGetToken();
    }
    else //procedure call with no arguments
    {
        //TODO procedure call code
    }
id_stmtEnd:
    dirTrace("id_stmt", tr_exit);
}

void fileptr_stmt()
{
    dirTrace("fileptr_stmt", tr_enter);
    EXPECT_GOTO(tok_colonequal, fileptr_stmtEnd);
    //TODO fileid assignment code
    //TODO type checking
    //writebuff
    t = lexerGetToken();
    expr(t, st);
fileptr_stmtEnd:
    dirTrace("fileptr_stmt", tr_exit);
}

void if_stmt()
{
    dirTrace("if_stmt", tr_enter);
    expr(t, st);
    EXPECT_GOTO(tok_kw_then, if_stmtEnd);
    //TODO if statement THEN code
    t = lexerGetToken();
    stmt();
    if (t->kind == tok_kw_else)
    {
        //TODO if statement ELSE code
        t = lexerGetToken();
        stmt();
    }
    //TODO if statement end code
if_stmtEnd:
    dirTrace("if_stmt", tr_exit);
}

void while_stmt()
{
    dirTrace("while_stmt", tr_enter);
    expr(t, st);
    EXPECT_GOTO(tok_kw_do, while_stmtEnd);
    //TODO while statement DO code
    t = lexerGetToken();
    stmt();
    //TODO while statement end code
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
    expr(t, st);
    //TODO for statement counter assignment code
    if (t->kind == tok_kw_downto || t->kind == tok_kw_to)
        t = lexerGetToken();
    else
        errorParse(err_exp_downto, t, tok_undef);
    expr(t, st);
    //TODO for statement limit code
    EXPECT_GOTO(tok_kw_do, for_stmtEnd);
    //TODO for statement DO code
    t = lexerGetToken();
    stmt();
    //TODO for statement end code
for_stmtEnd:
    dirTrace("for_stmt", tr_exit);
}

void arg_list()
{
    dirTrace("arg_list", tr_enter);
    expr(t, st);
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        expr(t, st);
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

void constant(symbol *sym)
{
    dirTrace("constant", tr_enter);
    pjtype pjt = isConstant(t->kind);
    if (pjt != pj_undef)
    {
        symbolSetConstVar(sym, pjt, t->lexeme);
        t = lexerGetToken();
    }
    else
        errorParse(err_exp_const, t, tok_undef);
    dirTrace("constant", tr_exit);
}

int bstCompareString(void *v1, void *v2)
{
    string *s1 = (string *) v1;
    string *s2 = (string *) v2;
    return stringCompareString(s1, s2);
}

void bstPrintString(void *v)
{
    string *str = (string *) v;
    fprintf(stdout, " \"%.*s\"", stringGetLength(str), stringGetBuffer(str));
}

void bstDelString(void *v)
{
    string *str = (string *) v;
    stringDestroy(str);
}
