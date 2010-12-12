#pragma once

#include <stdbool.h>

#include "token.h"

typedef enum __pjtype
{
    pj_integer,
    pj_real,
    pj_boolean,
    pj_alfa,
    pj_char,
    pj_string,
    pj_text,
    pj_undef
} pjtype;

typedef enum __pjop
{
    op_add,
    op_sub,
    op_mult,
    op_slash,
    op_div,
    op_mod,
    op_negate,
    op_and,
    op_or,
    op_not,
    op_lt,
    op_lteq,
    op_eq,
    op_gteq,
    op_gt,
    op_neq,
    op_none
} pjop;

typedef enum __pjbuiltin
{
    builtin_chr,
    builtin_eoln,
    builtin_eof,
    builtin_ord,
    builtin_round,
    builtin_sqr,
    builtin_trunc,
    builtin_reset,
    builtin_rewrite,
    builtin_put,
    builtin_get,
    builtin_read,
    builtin_write,
    builtin_readln,
    builtin_writeln,
    builtin_input_var,
    builtin_output_var,
    builtin_num
} pjbuiltin;

const char *pjtypeString(pjtype);

bool ispjop(token_kind);

pjop pjopLookup(token_kind);

pjtype isConstant(token_kind);

bool ispjbuiltinFunction(pjbuiltin);

bool ispjbuiltinProcedure(pjbuiltin);

const char *pjbuiltinString(pjbuiltin);
