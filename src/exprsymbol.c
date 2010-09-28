#include "exprsymbol.h"

#include <stdbool.h>

#include "token.h"

terminal tokToSLR[] =
{
    slr_id, /* tok_id */
    slr_fileptr, /* tok_fileid */
    slr_intgrcon, /* tok_integer_const */
    slr_realcon, /* tok_real_const */
    slr_dollar, /* tok_literal */
    slr_charcon, /* tok_char_const */
    slr_alfacon, /* tok_alfa_const */
    slr_strngcon, /* tok_string_const */
    slr_dollar, /* tok_comment */
    slr_lparen, /* tok_lparen */
    slr_rparen, /* tok_rparen */
    slr_lbrack, /* tok_lbrack */
    slr_rbrack, /* tok_rbrack */
    slr_lthan, /* tok_lthan */
    slr_gthan, /* tok_gthan */
    slr_lteq, /* tok_ltequal */
    slr_gteq, /* tok_gtequal */
    slr_neq, /* tok_nequal */
    slr_eq, /* tok_equal */
    slr_dollar, /* tok_dot */
    slr_dollar, /* tok_rdots */
    slr_dollar, /* tok_comma */
    slr_dollar, /* tok_semicolon */
    slr_dollar, /* tok_colon */
    slr_dollar, /* tok_colonequal */
    slr_plus, /* tok_plus */
    slr_minus, /* tok_minus */
    slr_ast, /* tok_asterisk */
    slr_slash, /* tok_slash */
    slr_dollar, /* tok_kw_alfa */
    slr_kw_and, /* tok_kw_and */
    slr_dollar, /* tok_kw_array */
    slr_dollar, /* tok_kw_begin */
    slr_dollar, /* tok_kw_boolean */
    slr_dollar, /* tok_kw_char */
    slr_dollar, /* tok_kw_const */
    slr_kw_div, /* tok_kw_div */
    slr_dollar, /* tok_kw_do */
    slr_dollar, /* tok_kw_downto */
    slr_dollar, /* tok_kw_else */
    slr_dollar, /* tok_kw_end */
    slr_kw_false, /* tok_kw_false */
    slr_dollar, /* tok_kw_for */
    slr_dollar, /* tok_kw_if */
    slr_dollar, /* tok_kw_integer */
    slr_kw_mod, /* tok_kw_mod */
    slr_kw_not, /* tok_kw_not */
    slr_dollar, /* tok_kw_of */
    slr_kw_or, /* tok_kw_or */
    slr_dollar, /* tok_kw_procedure */
    slr_dollar, /* tok_kw_program */
    slr_dollar, /* tok_kw_real */
    slr_dollar, /* tok_kw_text */
    slr_dollar, /* tok_kw_then */
    slr_dollar, /* tok_kw_to */
    slr_kw_true, /* tok_kw_true */
    slr_dollar, /* tok_kw_var */
    slr_dollar, /* tok_kw_while */
    slr_dollar, /* tok_undef */
};

const char *termStrings[29] =
{
    "+",
    "-",
    "*",
    "/",
    "div",
    "mod",
    "and",
    "or",
    "<",
    "<=",
    "=",
    ">=",
    ">",
    "<>",
    "not",
    "fileptr",
    "id",
    "(",
    ")",
    "realcon",
    "intgrcon",
    "alfacon",
    "charcon",
    "strngcon",
    "true",
    "false",
    "[",
    "]",
    "$"
};

const char *nontermStrings[13] =
{
    "start",
    "expr",
    "primary",
    "term",
    "factor",
    "builtin",
    "relop",
    "addop",
    "multop",
    "constant",
    "boolcon",
    "array",
    "s'"
};

bool isExprToken(token_kind t)
{
    if (tokToSLR[t] != slr_dollar)
        return true;
    return false;
}

terminal lookupTerminal(token_kind t)
{
    return tokToSLR[t];
}

const char *symbolString(symbol sym, bool isTerm)
{
    return isTerm ? termStrings[sym.term] : nontermStrings[sym.nonterm];
}
