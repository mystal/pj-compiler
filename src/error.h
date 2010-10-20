#pragma once

#include "token.h"

typedef enum __error_kind
{
    err_unextoken,
    err_exp_const,
    err_exp_stype,
    err_exp_end,
    err_exp_dot,
    err_num
} error_kind;

void error(error_kind, token *);

char *errorString(error_kind);
