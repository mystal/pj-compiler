#include "exprparser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "directive.h"
#include "exprprods.h"
#include "exprsymbol.h"
#include "exprtables.h"
#include "lexer.h"
#include "stack.h"
#include "token.h"

void expr(token *t)
{
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
        slr_stack_pair *s = stackPeek(stk);
        terminal term;
        //Handle special case of expr in position 1 and on top of stack
        if (!s->isTerm && (s->sym.nonterm == slr_expr || s->sym.nonterm == slr_start)
            && stackSize(stk) == 2 && (t->kind == tok_rparen || t->kind == tok_rbrack))
            term = slr_dollar;
        else
            //No check for non-expression, will return slr_dollar if needed
            term = lookupTerminal(t->kind);
        action_entry entry = actions[s->state][term];
        /*if (directives[dir_print_reduction])
            fprintf(stdout, "\tState: %d\n", s->state);*/
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
            {
                char cstr[t->lexeme.len+1];
                stringToCString(&t->lexeme, cstr, t->lexeme.len+1);
                fprintf(stdout, "SHIFT: %s '%s' \n", symbolString(s->sym, s->isTerm), cstr);
            }
            //Grab next input symbol
            tokenClean(t);
            tokenInit(t);
            lexerGetToken(t);
        }
        else if (entry.act == act_reduce)
        {
            //Pop rhsLen of reduction off of stack
            //Free memory for popped items
            production p = productions[entry.num];
            for (unsigned int i = 0; i < p.rhsLen; i++)
            {
                s = stackPop(stk);
                free(s);
            }
            //Push gotox[top stack state][lhs] state and lhs onto stack
            //Allocate memory for pushed item
            s = (slr_stack_pair *) malloc(sizeof(slr_stack_pair));
            s->state = gotox[stackPeek(stk)->state][p.lhs];
            s->sym.nonterm = p.lhs;
            s->isTerm = false;
            stackPush(stk, s);
            if (directives[dir_print_reduction])
                fprintf(stdout, "REDUCE[%d]: %s\n", entry.num, prodString(entry.num));
        }
        else if (entry.act == act_accept) //Parsing is done
        {
            if (directives[dir_flush_echo])
                fprintf(stdout, "ACCEPT\n");
            break;
        }
        else //Error routine
        {
            char cstr[t->lexeme.len+1];
            stringToCString(&t->lexeme, cstr, t->lexeme.len+1);
            fprintf(stdout, "Error: Unexpected token '%s' found in expression at (%d,%d):\n",
                    cstr, bufferLineNumber(), bufferPos()-t->lexeme.len);
            bufferPrint(stdout);
            if (directives[dir_flush_echo])
                fprintf(stdout, "\tFlushed Stack:");
            stackPrint(stk, stdout);
            if (directives[dir_flush_echo])
                fprintf(stdout, "\tFlushed Input:");
            //Flush input until next nonexpression token
            while (isExprToken(t->kind))
            {
                if (directives[dir_flush_echo])
                {
                    char cstr[t->lexeme.len+1];
                    stringToCString(&t->lexeme, cstr, t->lexeme.len+1);
                    fprintf(stdout, " %s", cstr);
                }
                tokenClean(t);
                tokenInit(t);
                lexerGetToken(t);
            }
            if (directives[dir_flush_echo])
                fprintf(stdout, "\n");
            break;
        }
    }
    //Cleanup stack
    while (stackSize(stk) > 1)
    {
        slr_stack_pair *s = stackPop(stk);
        free(s);
    }
    stackDestroy(stk);
    //No push-back, next token ready for driver above to use
}
