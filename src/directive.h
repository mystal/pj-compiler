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
    dir_sym_table,
    dir_echo_code,
    dir_num
} directive;

/* Enumeration for specifying entrance or exit of parser productions */
typedef enum __trace_status
{
    tr_enter,
    tr_exit
} trace_status;

/**
 * Sets the value of the directive represented by the first char. If the
 * second char is '+', sets to true, else sets to false.
 **/
void dirSet(char, char);

/**
 * Gets the current value of the given directive.
 **/
bool dirGet(directive);

/**
 * Gets the char representation of the given directive.
 **/
char dirChar(directive);

/**
 * If dir_tracing is turned on,
 * Prints ENTER or EXIT and the string passed in, depending on whether
 * entering or exiting the production.
 **/
void dirTrace(char *, trace_status);
