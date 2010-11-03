#include "exprparser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "directive.h"
#include "error.h"
#include "exprprods.h"
#include "exprsymbol.h"
#include "exprtables.h"
#include "lexer.h"
#include "stack.h"
#include "str.h"
#include "token.h"

typedef struct __slr_stack_pair
{
    unsigned int state : 8;
    expr_symbol sym;
    bool isTerm;
} slr_stack_pair;

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

void expr(token *t)
{
    dirTrace("expr", tr_enter);
    //Initialize stack
    stack *stk = stackCreate();
    slr_stack_pair bot;
    bot.state = 0;
    bot.sym.nonterm = slr_start;
    bot.isTerm = false;
    stackPush(stk, &bot);
    //Perform SLR algorithm
    while (true)
    {
        slr_stack_pair *s = (slr_stack_pair *) stackPeek(stk);
        terminal term;
        //Handle special case of expr in position 1 and on top of stack
        if (!s->isTerm && (s->sym.nonterm == slr_expr || s->sym.nonterm == slr_start)
            && stackSize(stk) == 2 && (t->kind == tok_rparen || t->kind == tok_rbrack))
            term = slr_dollar;
        else
            //No check for non-expression, will return slr_dollar if needed
            term = lookupTerminal(t->kind);
        action_entry entry = actions[s->state][term];
        if (entry.act == act_shift)
        {
            //Push term and actions[s->state][term].num onto stack
            //Allocate memory for pushed item
            s = (slr_stack_pair *) malloc(sizeof(slr_stack_pair));
            s->state = entry.num;
            s->sym.term = term;
            s->isTerm = true;
            stackPush(stk, s);
            if (directives[dir_print_reduction])
                fprintf(stdout, "SHIFT: %s '%.*s' \n", exprSymbolString(s->sym, s->isTerm),
                        t->lexeme.len, t->lexeme.buffer);
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
                s = (slr_stack_pair *) stackPop(stk);
                free(s);
            }
            //Push gotox[top stack state][lhs] state and lhs onto stack
            //Allocate memory for pushed item
            s = (slr_stack_pair *) malloc(sizeof(slr_stack_pair));
            s->state = gotox[((slr_stack_pair *) stackPeek(stk))->state][p.lhs];
            s->sym.nonterm = p.lhs;
            s->isTerm = false;
            stackPush(stk, s);
            if (directives[dir_print_reduction])
                fprintf(stdout, "REDUCE[%d]: %s\n", entry.num, prodString(entry.num));
        }
        else if (entry.act == act_accept) //Parsing is done
        {
            if (directives[dir_expr_flush_echo])
                fprintf(stdout, "ACCEPT\n");
            break;
        }
        else //Error routine
        {
            string s;
            error(err_unex_expr, t, tok_undef);
            if (directives[dir_expr_flush_echo])
            {
                fprintf(stdout, "\tFlushed Stack:");
                flushStack(stk);
                stringInit(&s);
            }
            //Flush input until next nonexpression token
            while (isExprToken(t))
            {
                if (directives[dir_expr_flush_echo])
                {
                    stringAppendChar(&s, ' ');
                    stringAppendString(&s, t->lexeme.buffer, t->lexeme.len);
                }
                t = lexerGetToken();
            }
            if (directives[dir_expr_flush_echo])
            {
                fprintf(stdout, "\n\tFlushed Input:%.*s\n", s.len, s.buffer);
                stringFree(&s);
            }
            break;
        }
    }
    //Cleanup stack
    while (stackSize(stk) > 1)
    {
        slr_stack_pair *s = (slr_stack_pair *) stackPop(stk);
        free(s);
    }
    stackDestroy(stk);
    //No push-back, next token ready for caller to use
    dirTrace("expr", tr_exit);
}

void flushStack(stack *stk)
{
    if (stackSize(stk) > 1)
    {
        slr_stack_pair *s = (slr_stack_pair *) stackPop(stk);
        flushStack(stk);
        fprintf(stdout, " %s", exprSymbolString(s->sym, s->isTerm));
        free(s);
    }
}
