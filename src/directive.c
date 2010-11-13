#include "directive.h"

#include <ctype.h>
#include <stdio.h>

bool directives[dir_num] =
{
    true, /* dir_listing */
    false, /* dir_token_echo */
    false, /* dir_print_reduction */
    false, /* dir_expr_flush_echo */
    false, /* dir_rd_flush_echo */
    false, /* dir_tracing */
    false, /* dir_sym_table */
    true /* dir_echo_code */
};

const char dirChars[dir_num] =
{
    'l', /* dir_listing */
    't', /* dir_token_echo */
    'p', /* dir_print_reduction */
    'f', /* dir_flush_echo */
    'r', /* dir_rd_flush_echo */
    'i', /* dir_tracing */
    's', /* dir_sym_table */
    'c' /* dir_echo_code */
};

unsigned int traceLevel = 0;

void dirSet(char d, char f)
{
    for (unsigned int i = 0; i < dir_num; i++)
    {
        if (tolower(d) == dirChars[i])
        {
            if (f == '+')
                directives[i] = true;
            else
                directives[i] = false;
            break;
        }
    }
}

bool dirGet(directive d)
{
    return directives[d];
}

char dirChar(directive d)
{
    return dirChars[d];
}

void dirTrace(char *s, trace_status t)
{
    if (!directives[dir_tracing])
    {
        if (t == tr_enter)
            traceLevel++;
        else
            traceLevel--;
        return;
    }
    if (t == tr_exit)
        traceLevel--;
    for (int i = 0; i < traceLevel; i++)
        fprintf(stdout, "   ");
    if (t == tr_enter)
        fprintf(stdout, "ENTER(%d) %s\n", traceLevel++, s);
    else if (t == tr_exit)
        fprintf(stdout, "EXIT(%d) %s\n", traceLevel, s);
}
