#pragma once

#include <stdbool.h>

#include "token.h"

typedef enum __terminal
{
    slr_plus,
    slr_minus,
    slr_ast,
    slr_slash,
    slr_kw_div,
    slr_kw_mod,
    slr_kw_and,
    slr_kw_or,
    slr_lthan,
    slr_lteq,
    slr_eq,
    slr_gteq,
    slr_gthan,
    slr_neq,
    slr_kw_not,
    slr_fileptr,
    slr_id,
    slr_lparen,
    slr_rparen,
    slr_realcon,
    slr_intgrcon,
    slr_alfacon,
    slr_charcon,
    slr_strngcon,
    slr_kw_true,
    slr_kw_false,
    slr_lbrack,
    slr_rbrack,
    slr_dollar
} terminal;

typedef enum __nonterminal
{
    slr_start,
    slr_expr,
    slr_primary,
    slr_term,
    slr_factor,
    slr_builtin,
    slr_relop,
    slr_addop,
    slr_multop,
    slr_constant,
    slr_boolcon,
    slr_array,
    slr_sprime
} nonterminal;

typedef union __symbol
{
    terminal term;
    nonterminal nonterm;
} symbol;

bool isExprToken(token_kind);

terminal lookupTerminal(token_kind);

const char *symbolString(symbol, bool);
