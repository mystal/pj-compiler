#include "exprparser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
        //Handle special case of expr in position 1 and on top of stack
        if (!s->isTerm && s->sym.nonterm == slr_expr &&
            stackSize(stk) == 2 && isExprToken(t->kind))
            break;
        //No check for non-expression, will return slr_dollar if needed
        terminal term = lookupTerminal(t->kind);
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
            fprintf(stdout, "Reduced by %d: %s\n", entry.num, prodString(entry.num));
        }
        else if (entry.act == act_accept)
            break; //Parsing is done
        else
        {
            //Error routine
            fprintf(stderr, "Error!\n");
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
}
