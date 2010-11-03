#pragma once

#include "token.h"

typedef enum __error_kind
{
    err_unex,
    err_unex_expr,
    err_exp_idint,
    err_exp_downto,
    err_exp_const,
    err_exp_stype,
    err_exp_end,
    err_exp_dot,
    err_badstmt,
    err_num
} error_kind;

/**
 * Prints the given error and the line it was found on.
 **/
void error(error_kind, token *, token_kind);
