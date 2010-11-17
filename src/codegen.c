#include "codegen.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "directive.h"
#include "error.h"
#include "list.h"
#include "pjlang.h"
#include "string.h"
#include "symbol.h"
#include "typecheck.h"

typedef enum __hypo_op
{
    hop_push = 1,
    hop_pushr,
    hop_pushi,
    hop_pushc,
    hop_pop,
    hop_popc,
    hop_popr,
    hop_move,
    hop_swap,
    hop_load,
    hop_loadr,
    hop_loada,
    hop_loadi,
    hop_store,
    hop_storeregs,
    hop_loadregs,
    hop_halt,
    hop_add,
    hop_sub,
    hop_negate,
    hop_mult,
    hop_div,
    hop_mod,
    hop_or,
    hop_and,
    hop_not,
    hop_trunc,
    hop_round,
    hop_float,
    hop_shift,
    hop_gt,
    hop_ge,
    hop_lt,
    hop_le,
    hop_eq,
    hop_ne,
    hop_b,
    hop_bct,
    hop_bcf,
    hop_noop,
    hop_pack,
    hop_unpack,
    hop_syscall,
    hop_call,
    hop_return,
    hop_num
} hypo_op;

typedef enum __hypo_syscall
{
    hsys_open = 1,
    hsys_close,
    hsys_get,
    hsys_put,
    hsys_readln,
    hsys_writeln,
    hsys_readbuff,
    hsys_writebuff,
    hsys_readint,
    hsys_readreal,
    hsys_writeint,
    hsys_writereal,
    hsys_clock,
    hsys_eoln,
    hsys_eof,
    hsys_dumpit,
    hsys_chr,
    hsys_ord,
    hsys_sqr,
    hsys_dumpr,
    hsys_dumpi,
    hsys_dumpd,
    hsys_linelimit,
    hsys_instlim,
    hsys_num
} hypo_syscall;

char *hypoOpStrings[hop_num] =
{
    "push", /* hop_push */
    "pushr", /* hop_pushr */
    "pushi", /* hop_pushi */
    "pushc", /* hop_pushc */
    "pop", /* hop_pop */
    "popc", /* hop_popc */
    "popr", /* hop_popr */
    "move", /* hop_move */
    "swap", /* hop_swap */
    "load", /* hop_load */
    "loadr", /* hop_loadr */
    "loada", /* hop_loada */
    "loadi", /* hop_loadi */
    "store", /* hop_store */
    "storeregs", /* hop_storeregs */
    "loadregs", /* hop_loadregs */
    "halt", /* hop_halt */
    "add", /* hop_add */
    "sub", /* hop_sub */
    "negate", /* hop_negate */
    "mult", /* hop_mult */
    "div", /* hop_div */
    "mod", /* hop_mod */
    "or", /* hop_or */
    "and", /* hop_and */
    "not", /* hop_not */
    "trunc", /* hop_trunc */
    "round", /* hop_round */
    "float", /* hop_float */
    "shift", /* hop_shift */
    "gt", /* hop_gt */
    "ge", /* hop_ge */
    "lt", /* hop_lt */
    "le", /* hop_le */
    "eq", /* hop_eq */
    "ne", /* hop_ne */
    "b", /* hop_b */
    "bct", /* hop_bct */
    "bcf", /* hop_bcf */
    "noop", /* hop_noop */
    "pack", /* hop_pack */
    "unpack", /* hop_unpack */
    "syscall", /* hop_syscall */
    "call", /* hop_call */
    "return" /* hop_return */
};

typedef char alfa[10];
typedef enum __hypo_type {h_int, h_real, h_char, h_alfa, h_bool} hypo_type;
typedef enum __spaces { ispace, dspace, regs } spaces;

typedef struct __varval
{
	hypo_type type;
	union {
		int intval;
		float realval;
		char charval;
		alfa alfaval;   /* array of 10 characters */
		char boolval;   /* set to NUMERIC 0 or 1 */
	} ircab_val;
} varval;

typedef struct __instruction {
	int opcode;
	varval oper1;
	int oper2;
} instruction;

typedef struct __reg
{
	int regnum, regval;
} reg;

typedef struct __loadrec
{
	int loc;
	spaces space;
	union {
		instruction ivalue;
		varval dvalue;
		reg rvalue;
	} idr_value;
} loadrec;

char *spaceStrings[regs+1] = {"ispace", "dspace", "regs"};

hypo_op pjopToHypoOp[op_none] =
{
    hop_add, /* op_add */
    hop_sub, /* op_sub */
    hop_mult, /* op_mult */
    hop_div, /* op_slash */
    hop_div, /* op_div */
    hop_mod, /* op_mod */
    hop_negate, /* op_negate */
    hop_and, /* op_and */
    hop_or, /* op_or */
    hop_not, /* op_not */
    hop_lt, /* op_lt */
    hop_le, /* op_lteq */
    hop_eq, /* op_eq */
    hop_ge, /* op_gteq */
    hop_gt, /* op_gt */
    hop_ne /* op_neq */
};

/* Private helper functions */
void writeCode(loadrec *);
void printCode(loadrec *);
int genaddr(unsigned int, unsigned int);
loadrec coerceInstruction(bool);

/* Function pointer type for code generating functions */
typedef pjtype (*code_func)(list *);

/* Code generating functions */
pjtype codeBinaryOp(list *);
pjtype codePushConst(list *);
pjtype codePushId(list *);
pjtype codePushFileid(list *);
pjtype codeUnaryOp(list *);
pjtype codeCallBuiltin(list *);
pjtype codePushArray(list *);

code_func genFuncs[18] =
{
    NULL, /* start --> expr */
    codeBinaryOp, /* expr --> expr relop primary */
    NULL, /* expr --> primary */
    codeBinaryOp, /* primary --> primary addop term */
    NULL, /* primary --> term */
    codeBinaryOp, /* term --> term multop factor */
    NULL, /* term --> factor */
    NULL, /* factor --> array */
    NULL, /* factor --> builtin */
    codePushConst, /* factor --> constant */
    codePushId, /* factor --> id */
    codePushFileid, /* factor --> fileptr */
    codeUnaryOp, /* factor --> not factor */
    codeUnaryOp, /* factor --> - factor */
    NULL, /* factor --> + factor */
    NULL, /* factor --> ( expr ) */
    codeCallBuiltin, /* builtin --> id ( expr ) */
    codePushArray /* array --> id [ expr ] */
};

FILE *loadfile = NULL;

unsigned int nextLoc = 1;

void codegenInit()
{
    loadfile = fopen("loadfile", "w");
}

void codegenCleanup()
{
    if (loadfile != NULL)
        fclose(loadfile);
}

pjtype codegenExpr(unsigned int prod, list *l)
{
    if (prod > 17)
        return pj_undef;
    code_func genFunc = genFuncs[prod];
    if (genFunc != NULL)
        return genFunc(l);
    if (prod == 14 || prod == 15)
    {
        slr_semantics *sem = (slr_semantics *) listGet(l, 1);
        pjtype type = sem->sem.type;
        if (prod == 14 && type != pj_integer && type != pj_real)
            errorType(err_op_type_mismatch);
        return sem->sem.type;
    }
    slr_semantics *sem = (slr_semantics *) listGetFront(l);
    return sem->sem.type;
}

void codegenReportError()
{
    if (loadfile != NULL)
    {
        fclose(loadfile);
        loadfile = NULL;
    }
}

pjtype codeBinaryOp(list *l)
{
    slr_semantics *sem1 = (slr_semantics *) listGetFront(l);
    slr_semantics *semOp = (slr_semantics *) listGet(l, 1);
    slr_semantics *sem2 = (slr_semantics *) listGetBack(l);
    pjop op = semOp->sem.op;
    pjtype pjt1 = sem1->sem.type;
    pjtype pjt2 = sem2->sem.type;
    op_check check = typeCheckOperator(op, pjt1, pjt2);
    if (check.ret != pj_undef)
    {
        hypo_op hop = pjopToHypoOp[op];
        loadrec lr;
        if (check.coerce1)
        {
            lr = coerceInstruction(true);
            writeCode(&lr);
        }
        else if (check.coerce2)
        {
            lr = coerceInstruction(false);
            writeCode(&lr);
        }
        lr.loc = nextLoc++;
        lr.space = ispace;
        instruction in;
        in.opcode = hop;
        in.oper2 = 0;
        in.oper1.type = h_int;
        if (hop == hop_add || hop == hop_sub || hop == hop_negate ||
            hop == hop_mult || hop == hop_div)
            in.oper1.ircab_val.intval = (pjt1 == pj_integer && pjt2 == pj_integer) ? 0 : 1;
        else
            in.oper1.ircab_val.intval = 0;
        lr.idr_value.ivalue = in;
        writeCode(&lr);
    }
    else
        errorType(err_op_type_mismatch);
    return check.ret;
}

pjtype codePushConst(list *l)
{
    slr_semantics *sem = (slr_semantics *) listGetFront(l);
    string *val = sem->sem.constVal.val;
    pjtype ret = sem->sem.constVal.type;
    loadrec lr;
    lr.loc = nextLoc++;
    lr.space = ispace;
    instruction in;
    in.opcode = hop_pushi;
    in.oper2 = 0;
    switch (ret)
    {
        case pj_integer:
            in.oper1.type = h_int;
            in.oper1.ircab_val.intval = stringToInt(val);
            break;
        case pj_real:
            in.oper1.type = h_real;
            in.oper1.ircab_val.realval = stringToFloat(val);
            break;
        case pj_boolean:
            in.oper1.type = h_bool;
            in.oper1.ircab_val.boolval = (stringToBool(val)) ? 1 : 0;
            break;
        case pj_alfa:
            in.oper1.type = h_alfa;
            strncpy(in.oper1.ircab_val.alfaval, stringGetBuffer(val), 10);
            break;
        case pj_char:
            in.oper1.type = h_char;
            in.oper1.ircab_val.charval = stringGetBuffer(val)[0];
            break;
        case pj_string:
            break;
        default:
            //TODO error
            break;
    }
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    return ret;
}

pjtype codePushId(list *l)
{
    slr_semantics *sem = (slr_semantics *) listGetFront(l);
    symbol *sym = sem->sem.sym.val;
    sym_type type = symbolGetType(sym);
    pjtype ret = pj_undef;
    loadrec lr;
    lr.loc = nextLoc++;
    lr.space = ispace;
    instruction in;
    in.oper2 = 0;
    if (type == symt_const_var)
    {
        string *val = symConstVarGetValue(sym);
        ret = symConstVarGetType(sym);
        in.opcode = hop_pushi;
        switch (ret)
        {
            case pj_integer:
                in.oper1.type = h_int;
                in.oper1.ircab_val.intval = stringToInt(val);
                break;
            case pj_real:
                in.oper1.type = h_real;
                in.oper1.ircab_val.realval = stringToFloat(val);
                break;
            case pj_boolean:
                in.oper1.type = h_bool;
                in.oper1.ircab_val.boolval = (char) stringToBool(val);
                break;
            case pj_alfa:
                in.oper1.type = h_alfa;
                strncpy(in.oper1.ircab_val.alfaval, stringGetBuffer(val), 10);
                break;
            case pj_char:
                in.oper1.type = h_char;
                in.oper1.ircab_val.charval = stringGetBuffer(val)[0];
                break;
            case pj_string:
                break;
            default:
                //TODO error
                break;
        }
    }
    else if (type == symt_var)
    {
        ret = symConstVarGetType(sym);
        if (ret == pj_text)
        {
            string *filename = symbolGetName(sym);
            in.opcode = hop_pushi;
            in.oper1.type = h_alfa;
            memset(in.oper1.ircab_val.alfaval, ' ', sizeof(alfa));
            strncpy(in.oper1.ircab_val.alfaval, stringGetBuffer(filename),
                    stringGetLength(filename));
        }
        else
        {
            in.opcode = hop_push;
            in.oper2 = 1;
            in.oper1.type = h_int;
            in.oper1.ircab_val.intval = genaddr(sem->sem.sym.level,
                                                symVarGetLocation(sym));
        }
    }
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    return ret;
}

pjtype codePushFileid(list *l)
{
    //TODO may be able to merge with PushId
    slr_semantics *sem = (slr_semantics *) listGetFront(l);
    symbol *sym = sem->sem.sym.val;
    sym_type type = symbolGetType(sym);
    pjtype pjt = symVarGetType(sym);
    if (type != symt_builtin && pjt != pj_text)
    {
        errorType(err_bad_fileptr);
        return pj_undef;
    }
    loadrec lr;
    lr.loc = nextLoc++;
    lr.space = ispace;
    instruction in;
    in.oper2 = 0;
    string *filename = symbolGetName(sym);
    in.opcode = hop_pushi;
    in.oper1.type = h_alfa;
    memset(in.oper1.ircab_val.alfaval, ' ', sizeof(alfa));
    strncpy(in.oper1.ircab_val.alfaval, stringGetBuffer(filename),
            stringGetLength(filename));
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    lr.loc = nextLoc++;
    lr.space = ispace;
    in.oper2 = 0;
    in.opcode = hop_syscall;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = hsys_readbuff;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    return pj_char;
}

pjtype codeUnaryOp(list *l)
{
    slr_semantics *semOp = (slr_semantics *) listGetFront(l);
    slr_semantics *semFactor = (slr_semantics *) listGetBack(l);
    pjop op = semOp->sem.op;
    if (op == op_sub)
        op = op_negate;
    pjtype ret = semFactor->sem.type;
    op_check check = typeCheckOperator(op, ret, pj_undef);
    if (check.ret != pj_undef)
    {
        hypo_op hop = pjopToHypoOp[op];
        loadrec lr;
        lr.loc = nextLoc++;
        lr.space = ispace;
        instruction in;
        in.opcode = hop;
        in.oper2 = 0;
        in.oper1.type = h_int;
        if (hop == hop_negate)
            in.oper1.ircab_val.intval = (check.ret == pj_integer) ? 0 : 1;
        else
            in.oper1.ircab_val.intval = 0;
        lr.idr_value.ivalue = in;
        writeCode(&lr);
    }
    return check.ret;
}

pjtype codeCallBuiltin(list *l)
{
    slr_semantics *semId = (slr_semantics *) listGetFront(l);
    symbol *sym = semId->sem.sym.val;
    if (symbolGetType(sym) != symt_builtin)
        //TODO error
        return pj_undef;
    pjbuiltin pjb = symBuiltinGetType(sym);
    pjtype ret;
    slr_semantics *semExpr = (slr_semantics *) listGet(l, 2);
    pjtype pjt = semExpr->sem.type;
    loadrec lr;
    lr.loc = nextLoc++;
    lr.space = ispace;
    instruction in;
    in.oper1.type = h_int;
    in.oper2 = 0;
    switch (pjb)
    {
        case builtin_chr:
            in.opcode = hop_syscall;
            in.oper1.ircab_val.intval = hsys_chr;
            break;
        case builtin_eof:
            in.opcode = hop_syscall;
            in.oper1.ircab_val.intval = hsys_eof;
            break;
        case builtin_eoln:
            in.opcode = hop_syscall;
            in.oper1.ircab_val.intval = hsys_eoln;
            break;
        case builtin_ord:
            in.opcode = hop_syscall;
            in.oper1.ircab_val.intval = hsys_ord;
            break;
        case builtin_sqr:
            in.opcode = hop_syscall;
            in.oper1.ircab_val.intval = hsys_sqr;
            break;
        case builtin_round:
            in.opcode = hop_round;
            in.oper1.ircab_val.intval = 0;
            break;
        case builtin_trunc:
            in.opcode = hop_trunc;
            in.oper1.ircab_val.intval = 0;
            break;
        default:
            //TODO error
            break;
    }
    lr.idr_value.ivalue = in;
    ret = typeCheckBuiltinFunction(pjb, pjt);
    if (ret != pj_undef)
    {
        writeCode(&lr);
    }
    return ret;
}

pjtype codePushArray(list *l)
{
    slr_semantics *semId = (slr_semantics *) listGetFront(l);
    symbol *sym = semId->sem.sym.val;
    unsigned int level = semId->sem.sym.level;
    if (symbolGetType(sym) != symt_array)
        //TODO error
        return pj_undef;
    pjtype ret = symArrayGetType(sym);
    slr_semantics *semExpr = (slr_semantics *) listGet(l, 2);
    pjtype pjt = semExpr->sem.type;
    if (pjt != pj_integer)
        //TODO error
        return pj_undef;
    loadrec lr;
    lr.loc = nextLoc++;
    lr.space = ispace;
    instruction in;
    in.opcode = hop_pushi;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = symArrayGetLowBound(sym);
    in.oper2 = 0;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    lr.loc = nextLoc++;
    lr.space = ispace;
    in.opcode = hop_sub;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = 0;
    in.oper2 = 0;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    lr.loc = nextLoc++;
    lr.space = ispace;
    in.opcode = hop_pushi;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = genaddr(level, symArrayGetLocation(sym));
    in.oper2 = 0;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    lr.loc = nextLoc++;
    lr.space = ispace;
    in.opcode = hop_add;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = 0;
    in.oper2 = 0;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    lr.loc = nextLoc++;
    lr.space = ispace;
    in.opcode = hop_popr;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = 5;
    in.oper2 = 0;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    lr.loc = nextLoc++;
    lr.space = ispace;
    in.opcode = hop_push;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = 5*pow(10, 7);
    in.oper2 = 1;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
    return ret;
}

void writeCode(loadrec *lr)
{
    if (dirGet(dir_echo_code))
        printCode(lr);
    if (loadfile != NULL)
        fwrite(lr, sizeof(loadrec), 1, loadfile);
}

void printCode(loadrec *lr)
{
    fprintf(stdout, "%s : ", spaceStrings[lr->space]);
    fprintf(stdout, "%4d : ", lr->loc);
    fprintf(stdout, "%10s : ", hypoOpStrings[lr->idr_value.ivalue.opcode-1]);
    switch (lr->idr_value.ivalue.oper1.type)
    {
        case h_int:
            fprintf(stdout, "%10d : ", lr->idr_value.ivalue.oper1.ircab_val.intval);
            break;
        case h_real:
            fprintf(stdout, "%10f : ", lr->idr_value.ivalue.oper1.ircab_val.realval);
            break;
        case h_char:
            fprintf(stdout, "%10c : ", lr->idr_value.ivalue.oper1.ircab_val.charval);
            break;
        case h_alfa:
            fprintf(stdout, "%10.10s : ", lr->idr_value.ivalue.oper1.ircab_val.alfaval);
            break;
        case h_bool:
            fprintf(stdout, "%10s : ", (lr->idr_value.ivalue.oper1.ircab_val.boolval == 1)
                    ? "true" : "false");
            break;
    }
    fprintf(stdout, " %4d\n", lr->idr_value.ivalue.oper2);
}

int genaddr(unsigned int level, unsigned int loc)
{
    //TODO don't just do indirect
    int addr = 0;
    addr += 1*pow(10, 8);
    addr += 1*pow(10, 7);
    addr += (12+level)*pow(10, 5);
    addr += (loc-1);
    return addr;
}

loadrec coerceInstruction(bool coerceFirst)
{
    loadrec lr;
    lr.loc = nextLoc++;
    lr.space = ispace;
    instruction in;
    in.opcode = hop_float;
    in.oper1.type = h_int;
    in.oper1.ircab_val.intval = (coerceFirst) ? 1 : 0;
    in.oper2 = 0;
    lr.idr_value.ivalue = in;
    return lr;
}
