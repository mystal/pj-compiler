#include "codegen.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "directive.h"
#include "error.h"
#include "hypomac.h"
#include "list.h"
#include "pjlang.h"
#include "string.h"
#include "symbol.h"
#include "symtable.h"
#include "typecheck.h"

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
void genSetDspace(unsigned int, varval);
void genSetRegister(unsigned int, int);
void getConstValue(pjtype, string *, varval *);
void writeCode(loadrec *);
void printCode(loadrec *);
void initDspace(void);
void initRegisters(void);

/* Function pointer type for code generating functions */
typedef pjtype (*expr_code_func)(list *, symtable *);

/* Expression code generating functions */
pjtype codeBinaryOp(list *, symtable *);
pjtype codePushConst(list *, symtable *);
pjtype codePushId(list *, symtable *);
pjtype codePushFileid(list *, symtable *);
pjtype codeUnaryOp(list *, symtable *);
pjtype codeCallBuiltin(list *, symtable *);
pjtype codePushArray(list *, symtable *);

/* Lookup table for determining which expr codegen function to run */
expr_code_func genFuncs[18] =
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

/* File pointer to loadfile to be run using hypomac */
FILE *loadfile = NULL;

/* Next available location in ispace */
unsigned int nextLoc = 0;

void codegenInit()
{
    loadfile = fopen("loadfile", "w");
    initDspace();
    initRegisters();
}

void codegenCleanup()
{
    varval vv;
    codegenInstruction(hop_halt, vv, 0);
    if (loadfile != NULL)
        fclose(loadfile);
}

void codegenInstruction(hypo_op opcode, varval oper1, int oper2)
{
    codegenInstructionAt(opcode, oper1, oper2, nextLoc++);
}

void codegenInstructionAt(hypo_op opcode, varval oper1, int oper2, unsigned int loc)
{
    //TODO initialize loadrec and varval
    loadrec lr;
    //memset(&lr, 0, sizeof(loadrec));
    lr.loc = loc;
    lr.space = ispace;
    instruction in;
    in.opcode = opcode;
    in.oper1 = oper1;
    in.oper2 = oper2;
    lr.idr_value.ivalue = in;
    writeCode(&lr);
}

int codegenIdAddress(symbol *id, unsigned int level)
{
    //TODO support direct addressing
    //TODO check type and get proper location
    int addr = 0;
    addr += 1*pow(10, 8);
    addr += 1*pow(10, 7);
    addr += (12+level)*pow(10, 5);
    addr += (symbolGetType(id) == symt_var) ? symVarGetType(id) :
            symArrayGetType(id) - 1;
    return addr;
}

void codegenArrayAddress(symbol *arr, unsigned int level)
{
    //TODO generate code for array address
    varval vv;
    vv.type = h_int;
    vv.ircab_val.intval = symArrayGetLowBound(arr);
    codegenInstruction(hop_pushi, vv, 0);
    vv.ircab_val.intval = 0;
    codegenInstruction(hop_sub, vv, 0);
    vv.ircab_val.intval = codegenIdAddress(arr, level);
    codegenInstruction(hop_pushi, vv, 0);
    vv.ircab_val.intval = 0;
    codegenInstruction(hop_add, vv, 0);
    vv.ircab_val.intval = 5;
    codegenInstruction(hop_popr, vv, 0);
    vv.ircab_val.intval = 5*pow(10, 7);
    codegenInstruction(hop_push, vv, 1);
}

pjtype codegenExpr(unsigned int prod, list *l, symtable *st)
{
    if (prod > 17)
        return pj_undef;
    expr_code_func genFunc = genFuncs[prod];
    if (genFunc != NULL)
        return genFunc(l, st);
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

unsigned int codegenGetNextLocation()
{
    return nextLoc;
}

void codegenReportError()
{
    if (loadfile != NULL)
    {
        fclose(loadfile);
        loadfile = NULL;
    }
}

pjtype codeBinaryOp(list *l, symtable *st)
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
        hypo_op opcode = pjopToHypoOp[op];
        varval vv;
        vv.type = h_int;
        if (check.coerce1)
        {
            vv.ircab_val.intval = 1;
            codegenInstruction(hop_float, vv, 0);
        }
        else if (check.coerce2)
        {
            vv.ircab_val.intval = 0;
            codegenInstruction(hop_float, vv, 0);
        }
        if (opcode == hop_add || opcode == hop_sub || opcode == hop_negate ||
            opcode == hop_mult || opcode == hop_div)
            vv.ircab_val.intval = (pjt1 == pj_integer && pjt2 == pj_integer) ? 0 : 1;
        else
            vv.ircab_val.intval = 0;
        codegenInstruction(opcode, vv, 0);
    }
    else
        errorType(err_op_type_mismatch);
    return check.ret;
}

pjtype codePushConst(list *l, symtable *st)
{
    slr_semantics *sem = (slr_semantics *) listGetFront(l);
    string *val = sem->sem.constVal.val;
    pjtype ret = sem->sem.constVal.type;
    varval vv;
    getConstValue(ret, val, &vv);
    codegenInstruction(hop_pushi, vv, 0);
    return ret;
}

pjtype codePushId(list *l, symtable *st)
{
    slr_semantics *sem = (slr_semantics *) listGetFront(l);
    symbol *sym = sem->sem.sym.val;
    sym_type type = symbolGetType(sym);
    pjtype ret = pj_undef;
    hypo_op opcode;
    varval vv;
    int oper2 = 0;
    if (type == symt_const_var)
    {
        string *val = symConstVarGetValue(sym);
        ret = symConstVarGetType(sym);
        opcode = hop_pushi;
        getConstValue(ret, val, &vv);
    }
    else if (type == symt_var)
    {
        ret = symVarGetType(sym);
        if (ret == pj_text)
        {
            string *filename = symbolGetName(sym);
            opcode = hop_pushi;
            vv.type = h_alfa;
            memset(vv.ircab_val.alfaval, ' ', sizeof(alfa));
            strncpy(vv.ircab_val.alfaval, stringGetBuffer(filename),
                    stringGetLength(filename));
        }
        else
        {
            opcode = hop_push;
            oper2 = 1;
            vv.type = h_int;
            vv.ircab_val.intval = codegenIdAddress(sym, sem->sem.sym.level);
        }
    }
    codegenInstruction(opcode, vv, oper2);
    return ret;
}

pjtype codePushFileid(list *l, symtable *st)
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
    varval vv;
    string *filename = symbolGetName(sym);
    vv.type = h_alfa;
    memset(vv.ircab_val.alfaval, ' ', sizeof(alfa));
    strncpy(vv.ircab_val.alfaval, stringGetBuffer(filename),
            stringGetLength(filename));
    codegenInstruction(hop_pushi, vv, 0);
    vv.type = h_int;
    vv.ircab_val.intval = hsys_readbuff;
    codegenInstruction(hop_syscall, vv, 0);
    return pj_char;
}

pjtype codeUnaryOp(list *l, symtable *st)
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
        hypo_op opcode = pjopToHypoOp[op];
        varval vv;
        vv.type = h_int;
        if (opcode == hop_negate)
            vv.ircab_val.intval = (check.ret == pj_integer) ? 0 : 1;
        else
            vv.ircab_val.intval = 0;
        codegenInstruction(opcode, vv, 0);
    }
    return check.ret;
}

pjtype codeCallBuiltin(list *l, symtable *st)
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
    hypo_op opcode = hop_syscall;
    varval vv;
    vv.type = h_int;
    vv.ircab_val.intval = 0;
    switch (pjb)
    {
        case builtin_chr:
            vv.ircab_val.intval = hsys_chr;
            break;
        case builtin_eof:
            vv.ircab_val.intval = hsys_eof;
            break;
        case builtin_eoln:
            vv.ircab_val.intval = hsys_eoln;
            break;
        case builtin_ord:
            vv.ircab_val.intval = hsys_ord;
            break;
        case builtin_sqr:
            vv.ircab_val.intval = hsys_sqr;
            break;
        case builtin_round:
            opcode = hop_round;
            break;
        case builtin_trunc:
            opcode = hop_trunc;
            break;
        default:
            //TODO error
            break;
    }
    codegenInstruction(opcode, vv, 0);
    ret = typeCheckBuiltinFunction(pjb, pjt);
    return ret;
}

pjtype codePushArray(list *l, symtable *st)
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
    codegenArrayAddress(sym, level);
    return ret;
}

void genSetDspace(unsigned int loc, varval value)
{
    loadrec lr;
    lr.loc = loc;
    lr.space = dspace;
    lr.idr_value.dvalue = value;
    writeCode(&lr);
}

void genSetRegister(unsigned int num, int val)
{
    loadrec lr;
    lr.space = regs;
    lr.idr_value.rvalue.regnum = num;
    lr.idr_value.rvalue.regval = val;
    writeCode(&lr);
}

void getConstValue(pjtype pjt, string *val, varval *vv)
{
    switch (pjt)
    {
        case pj_integer:
            vv->type = h_int;
            vv->ircab_val.intval = stringToInt(val);
            break;
        case pj_real:
            vv->type = h_real;
            vv->ircab_val.realval = stringToFloat(val);
            break;
        case pj_boolean:
            vv->type = h_bool;
            vv->ircab_val.boolval = (char) stringToBool(val);
            break;
        case pj_alfa:
            vv->type = h_alfa;
            strncpy(vv->ircab_val.alfaval, stringGetBuffer(val), 10);
            break;
        case pj_char:
            vv->type = h_char;
            vv->ircab_val.charval = stringGetBuffer(val)[0];
            break;
        case pj_string:
            break;
        default:
            //TODO error
            break;
    }
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
    fprintf(stdout, "%s : ", hypomacGetSpaceString(lr->space));
    if (lr->space != regs)
        fprintf(stdout, "%4d : ", lr->loc);
    else
        fprintf(stdout, "%2d : ", lr->idr_value.rvalue.regnum);
    if (lr->space == ispace)
    {
        instruction ins = lr->idr_value.ivalue;
        fprintf(stdout, "%10s : ", hypomacGetOpString(ins.opcode));
        if (ins.opcode == hop_syscall)
            fprintf(stdout, "%10s : ",
                    hypomacGetSyscallString(ins.oper1.ircab_val.intval));
        else
            switch (ins.oper1.type)
            {
                case h_int:
                    fprintf(stdout, "%10d : ", ins.oper1.ircab_val.intval);
                    break;
                case h_real:
                    fprintf(stdout, "%10f : ", ins.oper1.ircab_val.realval);
                    break;
                case h_char:
                    fprintf(stdout, "%10c : ", ins.oper1.ircab_val.charval);
                    break;
                case h_alfa:
                    fprintf(stdout, "%10.10s : ", ins.oper1.ircab_val.alfaval);
                    break;
                case h_bool:
                    fprintf(stdout, "%10s : ", (ins.oper1.ircab_val.boolval == 1)
                            ? "true" : "false");
                    break;
            }
        fprintf(stdout, " %4d\n", ins.oper2);
    }
    else
        //TODO check type and print proper dpsace value
        fprintf(stdout, " %6d\n", (lr->space == dspace) ?
                lr->idr_value.dvalue.ircab_val.intval :
                lr->idr_value.rvalue.regval);
}

void initDspace()
{
    varval vv;
    vv.type = h_int;
    vv.ircab_val.intval = 0;
    genSetDspace(1, vv);
    genSetDspace(2, vv);
    genSetDspace(3, vv);
    vv.ircab_val.intval = 15;
    genSetDspace(14, vv);
}

void initRegisters()
{
    genSetRegister(1, 1);
    genSetRegister(2, 14);
    genSetRegister(10, 0);
}
