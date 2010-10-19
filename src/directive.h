#pragma once

#include <stdbool.h>

/* Enumeration of supported directives */
typedef enum __directive
{
    dir_listing,
    dir_token_echo,
    dir_print_reduction,
    dir_expr_flush_echo,
    dir_rd_flush_echo,
    dir_tracing,
    dir_num
} directive;

/* Flags for current directives */
extern bool directives[];

/* Characters of directives */
extern const char dirChars[];

/* Enumeration for specifying entrance or exit of parser productions */
typedef enum __trace_status
{
    tr_enter,
    tr_exit
} trace_status;

/**
 * If dir_tracing is turned on,
 * Prints ENTER or EXIT and the string passed in, depending on whether
 * entering or exiting the production.
 **/
void dirTrace(char *, trace_status);
