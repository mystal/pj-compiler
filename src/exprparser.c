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

typedef enum __sem_kind
{
    sem_symbol,
    sem_operator,
    sem_constant,
    sem_type,
    sem_none
} sem_kind;

typedef union __slr_semantics
{
    symbol *sym;
    pjop op;
    string *constVal;
    pjtype type;
} slr_semantics;

typedef struct __slr_stack_entry
{
    unsigned int state : 8;
    expr_symbol exp;
    bool isTerm;
    sem_kind semType;
    slr_semantics sem;
} slr_stack_entry;

/* Private helper function */
void initSLREntry(slr_stack_entry *, unsigned int, expr_symbol, bool);
void addSLRSemantics(slr_stack_entry *, token *, symtable *);
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

void expr(token *t, symtable *st)
{
    stack *slrStk = stackCreate();
    slr_stack_entry bot;
    dirTrace("expr", tr_enter);
    //Initialize stack
    initSLREntry(&bot, 0, (expr_symbol) (nonterminal) slr_start, false);
    stackPush(slrStk, &bot);
    //Perform SLR algorithm
    while (true)
    {
        slr_stack_entry *s = (slr_stack_entry *) stackPeek(slrStk);
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
            s = (slr_stack_entry *) malloc(sizeof(slr_stack_entry));
            initSLREntry(s, entry.num, (expr_symbol) term, true);
            addSLRSemantics(s, t, st);
            stackPush(slrStk, s);
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
            for (unsigned int i = 0; i < p.rhsLen; i++)
            {
                s = (slr_stack_entry *) stackPop(slrStk);
                free(s);
            }
            //TODO generate code and push type(?)
            //codegenExpr(p);
            //Push gotox[top stack state][lhs] state and lhs onto stack
            //Allocate memory for pushed item
            s = (slr_stack_entry *) malloc(sizeof(slr_stack_entry));
            initSLREntry(s, gotox[((slr_stack_entry *) stackPeek(slrStk))->state][p.lhs],
                        (expr_symbol) p.lhs, false);
            stackPush(slrStk, s);
            if (dirGet(dir_print_reduction))
                fprintf(stdout, "REDUCE[%d]: %s\n", entry.num, prodString(entry.num));
        }
        else if (entry.act == act_accept) //Parsing is done
        {
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
    //Cleanup stack
    while (stackSize(slrStk) > 1)
    {
        slr_stack_entry *s = (slr_stack_entry *) stackPop(slrStk);
        free(s);
    }
    stackDestroy(slrStk);
    //No push-back, next token ready for caller to use
    dirTrace("expr", tr_exit);
}

void initSLREntry(slr_stack_entry *s, unsigned int state, expr_symbol exp, bool isTerm)
{
    s->state = state;
    s->exp = exp;
    s->isTerm = isTerm;
    s->semType = sem_none;
}

void addSLRSemantics(slr_stack_entry *s, token *t, symtable *st)
{
    token_kind tk = t->kind;
    if (tk == tok_id || tk == tok_fileid)
    {
        symbol *sym = stLookup(st, t->lexeme);
        if (sym != NULL)
        {
            s->semType = sem_symbol;
            s->sem.sym = sym;
        }
        else
        {
            errorST(err_undef_sym, t->lexeme);
        }
    }
    else if (ispjop(tk))
    {
        //TODO add operator to semantics
        s->semType = sem_operator;
        s->sem.op = pjopLookup(tk);
    }
    else
    {
        pjtype pjt = isConstant(tk);
        if (isConstant(tk))
        {
            //Get constant value and store in semantics
            string *value = stringCreate();
            stringAppendString(value, t->lexeme);
            s->semType = sem_constant;
            s->sem.constVal = value;
        }
    }
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
