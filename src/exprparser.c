#include "exprparser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "codegen.h"
#include "directive.h"
#include "error.h"
#include "exprprods.h"
#include "exprsymbol.h"
#include "exprtables.h"
#include "lexer.h"
#include "stack.h"
#include "str.h"
#include "symtable.h"
#include "token.h"

typedef struct __slr_stack_entry
{
    unsigned int state : 8;
    expr_symbol exp;
    bool isTerm;
} slr_stack_entry;

/* Private helper function */
slr_stack_entry *slrEntryCreate(unsigned int, expr_symbol, bool);
slr_semantics *slrSemanticsCreate(token *, symtable *);
void slrSemanticsDestroy(slr_semantics *);
void flushStack(stack *);

/**
 * Simple LR Expression Parser Grammar (BNF)
 *
 *  1 start --> expr 
 *  2 expr --> expr relop primary 
 *  3 expr --> primary 
 *  4 primary --> primary addop term 
 *  5 primary --> term 
 *  6 term --> term multop factor 
 *  7 term --> factor 
 *  8 factor --> array 
 *  9 factor --> builtin 
 * 10 factor --> constant 
 * 11 factor --> id 
 * 12 factor --> fileptr 
 * 13 factor --> not factor 
 * 14 factor --> - factor 
 * 15 factor --> + factor 
 * 16 factor --> ( expr ) 
 * 17 builtin --> id ( expr ) 
 * 18 array --> id [ expr ] 
 * 19 relop --> < 
 * 20 relop --> <= 
 * 21 relop --> = 
 * 22 relop --> >= 
 * 23 relop --> > 
 * 24 relop --> <> 
 * 25 addop --> + 
 * 26 addop --> - 
 * 27 addop --> or 
 * 28 multop --> * 
 * 29 multop --> / 
 * 30 multop --> div 
 * 31 multop --> mod 
 * 32 multop --> and 
 * 33 constant --> realcon 
 * 34 constant --> intgrcon 
 * 35 constant --> boolcon 
 * 36 constant --> alfacon 
 * 37 constant --> charcon 
 * 38 constant --> strngcon 
 * 39 boolcon --> true 
 * 40 boolcon --> false 
 * 41 s' --> start $
 **/

pjtype expr(token *t, symtable *st, bool isIO, bool isRead)
{
    pjtype retType = pj_undef;
    stack *slrStk = stackCreate();
    stack *semStk = stackCreate();
    slr_stack_entry *bot = slrEntryCreate(0, (expr_symbol) (nonterminal) slr_start, false);
    slr_semantics *botSem = (slr_semantics *) malloc(sizeof(slr_semantics));
    botSem->kind = slr_sem_none;
    dirTrace("expr", tr_enter);
    //Initialize stacks
    stackPush(slrStk, bot);
    stackPush(semStk, botSem);
    //Perform SLR algorithm
    while (true)
    {
        slr_stack_entry *s = (slr_stack_entry *) stackPeek(slrStk);
        slr_semantics *sem;
        terminal term;
        //Handle special case of expr in position 1 and on top of stack
        if (!s->isTerm && (s->exp.nonterm == slr_expr || s->exp.nonterm == slr_start)
            && stackSize(slrStk) == 2 && (t->kind == tok_rparen || t->kind == tok_rbrack))
            term = slr_dollar;
        else
            //No check for non-expression, will return slr_dollar if needed
            term = lookupTerminal(t->kind);
        action_entry entry = actions[s->state][term];
        if (entry.act == act_shift)
        {
            //Push term and actions[s->state][term].num onto stack
            //Allocate memory for pushed item
            s = slrEntryCreate(entry.num, (expr_symbol) term, true);
            sem = slrSemanticsCreate(t, st);
            stackPush(slrStk, s);
            stackPush(semStk, sem);
            if (dirGet(dir_print_reduction))
                fprintf(stdout, "SHIFT: %s '%.*s' \n", exprSymbolString(s->exp, s->isTerm),
                        stringGetLength(t->lexeme), stringGetBuffer(t->lexeme));
            //Grab next input token
            t = lexerGetToken();
        }
        else if (entry.act == act_reduce)
        {
            //Pop rhsLen of reduction off of stack
            //Free memory for popped items
            production p = productions[entry.num];
            list *l = listCreate();
            for (unsigned int i = 0; i < p.rhsLen; i++)
            {
                s = (slr_stack_entry *) stackPop(slrStk);
                free(s);
                sem = (slr_semantics *) stackPop(semStk);
                listAddFront(l, sem);
            }
            if (entry.num <= 17) //Generate code
            {
                pjtype type = codegenExpr(entry.num, l, st);
                if (type != pj_undef) //Push returned type
                {
                    sem = (slr_semantics *) malloc(sizeof(slr_semantics));
                    sem->kind = slr_sem_type;
                    sem->sem.type = type;
                    stackPush(semStk, sem);
                }
                else //Error
                {
                    //TODO report/handle error
                }
            }
            else
            {
                sem = (slr_semantics *) listRemoveBack(l);
                stackPush(semStk, sem);
            }
            //Cleanup list
            while (listSize(l) > 0)
            {
                sem = (slr_semantics *) listRemoveBack(l);
                slrSemanticsDestroy(sem);
            }
            listDestroy(l);
            //Push gotox[top stack state][lhs] state and lhs onto stack
            //Allocate memory for pushed item
            s = slrEntryCreate(gotox[((slr_stack_entry *) stackPeek(slrStk))->state][p.lhs],
                               (expr_symbol) p.lhs, false);
            stackPush(slrStk, s);
            if (dirGet(dir_print_reduction))
                fprintf(stdout, "REDUCE[%d]: %s\n", entry.num, prodString(entry.num));
        }
        else if (entry.act == act_accept) //Parsing is done
        {
            slr_semantics *sem = (slr_semantics *) stackPeek(semStk);
            retType = sem->sem.type;
            if (dirGet(dir_expr_flush_echo))
                fprintf(stdout, "ACCEPT\n");
            break;
        }
        else //Error routine
        {
            string *s;
            errorParse(err_unex_expr, t, tok_undef);
            if (dirGet(dir_expr_flush_echo))
            {
                fprintf(stdout, "\tFlushed Stack:");
                flushStack(slrStk);
                s = stringCreate();
            }
            //Flush input until next nonexpression token
            while (isExprToken(t))
            {
                if (dirGet(dir_expr_flush_echo))
                {
                    stringAppendChar(s, ' ');
                    stringAppendString(s, t->lexeme);
                }
                t = lexerGetToken();
            }
            if (dirGet(dir_expr_flush_echo))
            {
                fprintf(stdout, "\n\tFlushed Input:%.*s\n", stringGetLength(s),
                        stringGetBuffer(s));
                stringDestroy(s);
            }
            break;
        }
    }
    //Cleanup stacks
    while (stackSize(slrStk) > 0)
    {
        slr_stack_entry *s = (slr_stack_entry *) stackPop(slrStk);
        free(s);
    }
    while (stackSize(semStk) > 0)
    {
        slr_semantics *s = (slr_semantics *) stackPop(semStk);
        slrSemanticsDestroy(s);
    }
    stackDestroy(semStk);
    stackDestroy(slrStk);
    //No push-back, next token ready for caller to use
    dirTrace("expr", tr_exit);
    return retType;
}

slr_stack_entry *slrEntryCreate(unsigned int state, expr_symbol exp, bool isTerm)
{
    slr_stack_entry *s = (slr_stack_entry *) malloc(sizeof(slr_stack_entry));
    s->state = state;
    s->exp = exp;
    s->isTerm = isTerm;
    return s;
}

slr_semantics *slrSemanticsCreate(token *t, symtable *st)
{
    slr_semantics *sem = (slr_semantics *) malloc(sizeof(slr_semantics));
    sem->kind = slr_sem_none;
    token_kind tk = t->kind;
    if (tk == tok_id || tk == tok_fileid)
    {
        unsigned int level;
        if (tk == tok_fileid)
            stringDrop(t->lexeme, 1);
        symbol *sym = stLookup(st, t->lexeme, &level);
        if (tk == tok_fileid)
            stringAppendChar(t->lexeme, '^');
        if (sym != NULL)
        {
            sem->kind = slr_sem_symbol;
            sem->sem.sym.val = sym;
            sem->sem.sym.level = level;
        }
        else
        {
            errorST(err_undef_sym, t->lexeme);
        }
    }
    else if (ispjop(tk))
    {
        sem->kind = slr_sem_operator;
        sem->sem.op = pjopLookup(tk);
    }
    else
    {
        pjtype pjt = isConstant(tk);
        if (pjt != pj_undef)
        {
            //Get constant value and store in semantics
            string *value = stringCreate();
            stringAppendString(value, t->lexeme);
            sem->kind = slr_sem_constant;
            sem->sem.constVal.val = value;
            sem->sem.constVal.type = pjt;
        }
    }
    return sem;
}

void slrSemanticsDestroy(slr_semantics *sem)
{
    if (sem->kind == slr_sem_constant)
    {
        stringDestroy(sem->sem.constVal.val);
    }
    free(sem);
}

void flushStack(stack *stk)
{
    if (stackSize(stk) > 1)
    {
        slr_stack_entry *s = (slr_stack_entry *) stackPop(stk);
        flushStack(stk);
        fprintf(stdout, " %s", exprSymbolString(s->exp, s->isTerm));
        free(s);
    }
}
