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
#include "typecheck.h"

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
void block(symbol *, unsigned int, bst *);
void const_decl(void);
unsigned int var_decl(bst *);
void var_id_list(list *);
void type_decl(sym_type *, unsigned int *, unsigned int *, pjtype *);
void proc(void);
void param_list(list *);
void param(list *);
void range_decl(unsigned int *, unsigned int *);
void range_const(unsigned int *);
void stmt_list(void);
void stmt(symbol *);
void id_stmt(symbol *, unsigned int);
void fileptr_stmt(symbol *);
void if_stmt(void);
void while_stmt(void);
void for_stmt(void);
void arg_list(symbol *);
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
    block(NULL, 0, progFiles);
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

void block(symbol *procSym, unsigned int numArgs, bst *progFiles)
{
    varval vv;
    unsigned int branchLoc, varSpace = 0;
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
        varSpace += var_decl(progFiles);
        while (t->kind == tok_id)
        {
            varSpace += var_decl(progFiles);
        }
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
    vv.type = h_int;
    if (procSym != NULL)
    {
        symProcSetLocation(procSym, codegenGetNextLocation());
        vv.ircab_val.intval = 10000003;
        codegenInstruction(hop_storeregs, vv, 7); //store caller's registers
    }
    if (numArgs > 0)
    {
        vv.ircab_val.intval = 40000000;
        codegenInstruction(hop_push, vv, numArgs); //copy args
    }
    if (varSpace > 0)
    {
        vv.ircab_val.intval = 0;
        codegenInstruction(hop_push, vv, varSpace); //push varSpace
    }
    branchLoc = codegenGetNextLocation();
    codegenIncrementNextLocation();
    while (t->kind == tok_kw_procedure)
    {
        t = lexerGetToken();
        proc();
    }
    tokenbstInsert(followSet, tok_semicolon);
    tokenbstInsert(followSet, tok_id);
    EXPECT_GOTO(tok_kw_begin, stmt_listStart);
    vv.ircab_val.intval = codegenGetNextLocation();
    codegenInstructionAt(hop_b, vv, 0, branchLoc); //branch to next instruction
    t = lexerGetToken();
stmt_listStart:
    tokenbstRemove(followSet, tok_id);
    stmt_list();
    tokenbstInsert(followSet, tok_semicolon);
    EXPECT_GOTO(tok_kw_end, blockEnd);
    if (procSym != NULL)
    {
        vv.ircab_val.intval = 10000003;
        codegenInstruction(hop_loadregs, vv, 7); //load caller's registers
        codegenInstruction(hop_return, vv, 0); //return to the caller
    }
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

unsigned int var_decl(bst *progFiles)
{
    unsigned int varSpace = 0;
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
        {
            symbolSetVar(sym, pjt);
            if (pjt != pj_text)
                varSpace = 1;
        }
        else if (symt == symt_array)
        {
            symbolSetArray(sym, pjt, low, up);
            varSpace = up - low + 1;
        }
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
    return varSpace;
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
    symbol *procSym;
    list *params = listCreate();
    dirTrace("proc", tr_enter);
    tokenbstInsert(followSet, tok_lparen);
    EXPECT_GOTO(tok_id, param_listStart);
    procName = stringCreate(); //Get procedure name
    stringAppendString(procName, t->lexeme);
    procSym = symbolCreate(procName); //Create new symbol for procedure
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
    symbolSetProc(procSym, numParams, paramTypes);
    if (!stAddSymbol(st, procSym)) //Try to add procedure symbol
    {
        //Print error, free memory
        errorST(err_dup_sym, symbolGetName(procSym));
        symbolDestroy(procSym);
    }
param_listEnd:
    EXPECT_GOTO(tok_semicolon, procBlockStart);
    t = lexerGetToken();
procBlockStart:
    stEnterBlock(st, procName);
    //Add params to symbol table in new block
    while (listSize(params) != 0)
    {
        symbol *sym = (symbol *) listRemoveFront(params);
        if (!stAddSymbol(st, sym))
        {
            //Print error, free symbol memory
            errorST(err_dup_sym, symbolGetName(sym));
            symbolDestroy(sym);
        }
    }
    block(procSym, numParams, NULL);
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
    stmt(NULL);
    while (t->kind == tok_semicolon)
    {
        t = lexerGetToken();
        stmt(NULL);
    }
    dirTrace("stmt_list", tr_exit);
}

void stmt(symbol *lcv)
{
    dirTrace("stmt", tr_enter);
    tokenbstInsert(followSet, tok_semicolon);
    if (t->kind == tok_id)
    {
        unsigned int level;
        symbol *id = stLookup(st, t->lexeme, &level);
        if (lcv != NULL && lcv == id)
            errorST(err_for_lcv_mod, symbolGetName(lcv));
        t = lexerGetToken();
        id_stmt(id, level);
    }
    else if (t->kind == tok_fileid)
    {
        unsigned int level;
        stringDrop(t->lexeme, 1);
        symbol *id = stLookup(st, t->lexeme, &level);
        stringAppendChar(t->lexeme, '^');
        t = lexerGetToken();
        fileptr_stmt(id);
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
    pjtype type;
    varval vv;
    vv.type = h_int;
    dirTrace("id_stmt", tr_enter);
    if (t->kind == tok_colonequal) //id assignment
    {
        //TODO make sure it's a valid var to assign to
        pjtype varType = symVarGetType(id);
        t = lexerGetToken();
        type = expr(t, st, false, false);
        if (varType != type) //type check assignment
        {
            if (type == pj_integer && varType == pj_real)
            {
                vv.ircab_val.intval = 0;
                codegenInstruction(hop_float, vv, 0);
            }
            else
            {
                //TODO error
            }
        }
        vv.ircab_val.intval = codegenIdAddress(id, level);
        codegenInstruction(hop_pop, vv, 1);
    }
    else if (t->kind == tok_lbrack) //array value assignment
    {
        //TODO make sure it's a valid array var to assign to
        t = lexerGetToken();
        type = expr(t, st, false, false);
        //TODO make sure type is int
        codegenArrayAddress(id, level);
        EXPECT_GOTO(tok_rbrack, id_stmtEnd);
        t = lexerGetToken();
        EXPECT_GOTO(tok_colonequal, id_stmtEnd);
        t = lexerGetToken();
        type = expr(t, st, false, false);
        //TODO type checking
        vv.ircab_val.intval = 0;
        codegenInstruction(hop_swap, vv, 0);
        vv.ircab_val.intval = 5;
        codegenInstruction(hop_popr, vv, 0);
        vv.ircab_val.intval = 50000000; //10^7
        codegenInstruction(hop_pop, vv, 1);
    }
    else if (t->kind == tok_lparen)
    {
        if (symbolGetType(id) == symt_proc) //procedure call with arguments
        {
            t = lexerGetToken();
            vv.ircab_val.intval = 4;
            codegenInstruction(hop_loada, vv, 20000001);
            arg_list(id);
            EXPECT_GOTO(tok_rparen, id_stmtEnd);
            codegenProcedureCall(id, level);
            t = lexerGetToken();
        }
        else if (symbolGetType(id) == symt_builtin &&
                 ispjbuiltinProcedure(symBuiltinGetType(id))) //builtin procedure call
        {
            pjbuiltin pjb = symBuiltinGetType(id);
            t = lexerGetToken();
            if (pjb == builtin_reset)
            {
                unsigned int level;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                codegenBuiltinProcedure(pjb, symbolGetName(file), pj_undef, NULL, 0, NULL);
                t = lexerGetToken();
            }
            else if (pjb == builtin_rewrite)
            {
                unsigned int level;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                codegenBuiltinProcedure(pjb, symbolGetName(file), pj_undef, NULL, 0, NULL);
                t = lexerGetToken();
            }
            else if (pjb == builtin_put)
            {
                unsigned int level;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                codegenBuiltinProcedure(pjb, symbolGetName(file), pj_undef, NULL, 0, NULL);
                t = lexerGetToken();
            }
            else if (pjb == builtin_get)
            {
                unsigned int level;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                codegenBuiltinProcedure(pjb, symbolGetName(file), pj_undef, NULL, 0, NULL);
                t = lexerGetToken();
            }
            else if (pjb == builtin_read)
            {
                unsigned int level;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                t = lexerGetToken();
                EXPECT_GOTO(tok_comma, id_stmtEnd);
                t = lexerGetToken();
                symbol *readVar = stLookup(st, t->lexeme, &level);
                if (symbolGetType(readVar) != symt_array)
                    t = lexerGetToken();
                else
                    expr(t, st, false, true);
                codegenBuiltinProcedure(pjb, symbolGetName(file), pj_undef, readVar, level, NULL);
            }
            else if (pjb == builtin_write)
            {
                unsigned int level;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                string *str = NULL;
                t = lexerGetToken();
                EXPECT_GOTO(tok_comma, id_stmtEnd);
                t = lexerGetToken();
                if (t->kind != tok_string_const)
                {
                    type = expr(t, st, false, false);
                    codegenBuiltinProcedure(pjb, symbolGetName(file), type, NULL, 0, NULL);
                }
                else
                {
                    type = pj_string;
                    codegenBuiltinProcedure(pjb, symbolGetName(file), type, NULL, 0, t->lexeme);
                    t = lexerGetToken();
                }
            }
            else if (pjb == builtin_readln)
            {
                unsigned int level;
                symbol *readVar = NULL;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                t = lexerGetToken();
                if (t->kind == tok_comma)
                {
                    t = lexerGetToken();
                    readVar = stLookup(st, t->lexeme, &level);
                    t = lexerGetToken();
                }
                codegenBuiltinProcedure(pjb, symbolGetName(file), pj_undef, readVar, level, NULL);
            }
            else if (pjb == builtin_writeln)
            {
                unsigned int level;
                type = pj_undef;
                //TODO check arguments
                //TODO call expr instead of hard coding this...
                symbol *file = stLookup(st, t->lexeme, &level);
                t = lexerGetToken();
                if (t->kind == tok_comma)
                {
                    t = lexerGetToken();
                    if (t->kind != tok_string_const)
                    {
                        type = expr(t, st, false, false);
                        codegenBuiltinProcedure(pjb, symbolGetName(file), type, NULL, 0, NULL);
                    }
                    else
                    {
                        type = pj_string;
                        codegenBuiltinProcedure(pjb, symbolGetName(file), type, NULL, 0, t->lexeme);
                        t = lexerGetToken();
                    }
                }
                else
                    codegenBuiltinProcedure(pjb, symbolGetName(file), type, NULL, 0, NULL);
            }
            EXPECT_GOTO(tok_rparen, id_stmtEnd);
            t = lexerGetToken();
        }
        else
        {
            //TODO error
        }
    }
    else //procedure call with no arguments
    {
        if (symbolGetType(id) == symt_proc)
        {
            vv.ircab_val.intval = 4;
            codegenInstruction(hop_loada, vv, 20000001);
            codegenProcedureCall(id, level);
        }
        else
        {
            //TODO error
        }
    }
id_stmtEnd:
    dirTrace("id_stmt", tr_exit);
}

void fileptr_stmt(symbol *id)
{
    pjtype type;
    varval vv;
    dirTrace("fileptr_stmt", tr_enter);
    EXPECT_GOTO(tok_colonequal, fileptr_stmtEnd);
    string *filename = symbolGetName(id);
    vv.type = h_alfa;
    memset(vv.ircab_val.alfaval, ' ', sizeof(alfa));
    strncpy(vv.ircab_val.alfaval, stringGetBuffer(filename),
            stringGetLength(filename));
    codegenInstruction(hop_pushi, vv, 0);
    t = lexerGetToken();
    type = expr(t, st, false, false);
    //TODO check expr is of type char
    vv.type = h_int;
    vv.ircab_val.intval = hsys_writebuff;
    codegenInstruction(hop_syscall, vv, 0);
fileptr_stmtEnd:
    dirTrace("fileptr_stmt", tr_exit);
}

void if_stmt()
{
    pjtype type;
    varval vv;
    unsigned int bcfLoc;
    dirTrace("if_stmt", tr_enter);
    type = expr(t, st, false, false);
    if (type != pj_boolean)
        errorType(err_cond_not_bool);
    EXPECT_GOTO(tok_kw_then, if_stmtEnd);
    bcfLoc = codegenGetNextLocation();
    codegenIncrementNextLocation();
    t = lexerGetToken();
    stmt(NULL);
    vv.type = h_int;
    if (t->kind == tok_kw_else)
    {
        unsigned int bLoc = codegenGetNextLocation();
        codegenIncrementNextLocation();
        vv.ircab_val.intval = codegenGetNextLocation();
        codegenInstructionAt(hop_bcf, vv, 0, bcfLoc); //branch on false
        t = lexerGetToken();
        stmt(NULL);
        vv.ircab_val.intval = codegenGetNextLocation();
        codegenInstructionAt(hop_b, vv, 0, bLoc); //unconditional branch
    }
    else
    {
        vv.ircab_val.intval = codegenGetNextLocation();
        codegenInstructionAt(hop_bcf, vv, 0, bcfLoc); //branch on false
    }
if_stmtEnd:
    dirTrace("if_stmt", tr_exit);
}

void while_stmt()
{
    pjtype type;
    unsigned int bcfLoc;
    varval vv;
    dirTrace("while_stmt", tr_enter);
    vv.type = h_int;
    int whileStart = codegenGetNextLocation();
    type = expr(t, st, false, false);
    //TODO check expr is boolean
    EXPECT_GOTO(tok_kw_do, while_stmtEnd);
    bcfLoc = codegenGetNextLocation();
    codegenIncrementNextLocation();
    t = lexerGetToken();
    stmt(NULL);
    vv.ircab_val.intval = whileStart;
    codegenInstruction(hop_b, vv, 0);
    vv.ircab_val.intval = codegenGetNextLocation();
    codegenInstructionAt(hop_bcf, vv, 0, bcfLoc);
while_stmtEnd:
    dirTrace("while_stmt", tr_exit);
}

void for_stmt()
{
    pjtype type;
    varval vv;
    unsigned int lcvLevel, bctLoc, forStart;
    bool to = false;
    dirTrace("for_stmt", tr_enter);
    EXPECT_GOTO(tok_id, for_stmtEnd);
    symbol *lcv = stLookup(st, t->lexeme, &lcvLevel);
    if (lcv == NULL)
    {
        //TODO report error
    }
    else if (symbolGetType(lcv) != symt_var)
    {
        //TODO report error
    }
    else if (symVarGetType(lcv) != pj_integer)
        errorType(err_for_lcv_not_int);
    t = lexerGetToken();
    EXPECT_GOTO(tok_colonequal, for_stmtEnd);
    t = lexerGetToken();
    type = expr(t, st, false, false); //expr1
    if (type != pj_integer)
        errorType(err_for_lcv_not_int);
    if (t->kind == tok_kw_to)
    {
        to = true;
        t = lexerGetToken();
    }
    else if (t->kind == tok_kw_downto)
    {
        to = false;
        t = lexerGetToken();
    }
    else
        errorParse(err_exp_downto, t, tok_undef);
    type = expr(t, st, false, false); //expr2
    if (type != pj_integer)
        errorType(err_for_lim_not_int);
    vv.type = h_int;
    vv.ircab_val.intval = 0;
    codegenInstruction(hop_swap, vv, 0); //swap top stack elements
    vv.ircab_val.intval = codegenIdAddress(lcv, lcvLevel);
    codegenInstruction(hop_pop, vv, 1); //pop expr1 into lcv
    forStart = codegenGetNextLocation();
    vv.ircab_val.intval = 20000000;
    codegenInstruction(hop_push, vv, 1); //duplicate expr2 by push TOS
    vv.ircab_val.intval = codegenIdAddress(lcv, lcvLevel);
    codegenInstruction(hop_push, vv, 1); //push lcv
    //branch if (to) expr2 < lcv, (downto) expr2 > lcv
    vv.ircab_val.intval = 0;
    codegenInstruction((to) ? hop_lt : hop_gt, vv, 0);
    bctLoc = codegenGetNextLocation();
    codegenIncrementNextLocation();
    EXPECT_GOTO(tok_kw_do, for_stmtEnd);
    t = lexerGetToken();
    stmt(lcv);
    vv.ircab_val.intval = codegenIdAddress(lcv, lcvLevel);
    codegenInstruction(hop_push, vv, 1); //push lcv
    vv.ircab_val.intval = 1;
    codegenInstruction(hop_pushi, vv, 0); //pushi 1
    vv.ircab_val.intval = 0;
    if (to)
        codegenInstruction(hop_add, vv, 0); //add
    else
        codegenInstruction(hop_sub, vv, 0); //sub
    vv.ircab_val.intval = codegenIdAddress(lcv, lcvLevel);
    codegenInstruction(hop_pop, vv, 1); //pop lcv 1
    vv.ircab_val.intval = forStart;
    codegenInstruction(hop_b, vv, 0); //branch to forStart
    vv.ircab_val.intval = codegenGetNextLocation();
    codegenInstructionAt(hop_bct, vv, 0, bctLoc); //branch to end of for
    vv.ircab_val.intval = 0;
    codegenInstruction(hop_pop, vv, 1); //pop duplicate expr2
for_stmtEnd:
    dirTrace("for_stmt", tr_exit);
}

void arg_list(symbol *procSym)
{
    pjtype type;
    unsigned int paramCount = 0;
    unsigned int numParams = symProcGetNumParams(procSym);
    const pjtype *paramTypes = symProcGetParams(procSym);
    dirTrace("arg_list", tr_enter);
    //TODO procedure parameter type check
    type = expr(t, st, false, false);
    paramCount++;
    if (paramCount > numParams)
        errorType(err_wrong_num_params);
    else if (type != paramTypes[paramCount-1])
        errorType(err_wrong_param_type);
    while (t->kind == tok_comma)
    {
        t = lexerGetToken();
        type = expr(t, st, false, false);
        paramCount++;
        if (paramCount > numParams)
            errorType(err_wrong_num_params);
        else if (type != paramTypes[paramCount-1])
            errorType(err_wrong_param_type);
    }
    if (paramCount < numParams)
        errorType(err_wrong_num_params);
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
