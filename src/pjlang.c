#include "pjlang.h"

const char *pjtypeStrings[pj_undef] =
{
    "integer", /* pj_integer */
    "real", /* pj_real */
    "boolean", /* pj_boolean */
    "alfa", /* pj_alfa */
    "char", /* pj_char */
    "string", /* pj_string */
    "text" /* pj_text */
};

pjop tokenToOp[tok_undef+1] =
{
    op_none, /* tok_id */
    op_none, /* tok_fileid */
    op_none, /* tok_integer_const */
    op_none, /* tok_real_const */
    op_none, /* tok_literal */
    op_none, /* tok_char_const */
    op_none, /* tok_alfa_const */
    op_none, /* tok_string_const */
    op_none, /* tok_comment */
    op_none, /* tok_lparen */
    op_none, /* tok_rparen */
    op_none, /* tok_lbrack */
    op_none, /* tok_rbrack */
    op_lt, /* tok_lthan */
    op_gt, /* tok_gthan */
    op_lteq, /* tok_ltequal */
    op_gteq, /* tok_gtequal */
    op_neq, /* tok_nequal */
    op_eq, /* tok_equal */
    op_none, /* tok_dot */
    op_none, /* tok_rdots */
    op_none, /* tok_comma */
    op_none, /* tok_semicolon */
    op_none, /* tok_colon */
    op_none, /* tok_colonequal */
    op_add, /* tok_plus */
    op_sub, /* tok_minus */
    op_mult, /* tok_asterisk */
    op_slash, /* tok_slash */
    op_none, /* tok_kw_alfa */
    op_and, /* tok_kw_and */
    op_none, /* tok_kw_array */
    op_none, /* tok_kw_begin */
    op_none, /* tok_kw_boolean */
    op_none, /* tok_kw_char */
    op_none, /* tok_kw_const */
    op_div, /* tok_kw_div */
    op_none, /* tok_kw_do */
    op_none, /* tok_kw_downto */
    op_none, /* tok_kw_else */
    op_none, /* tok_kw_end */
    op_none, /* tok_kw_false */
    op_none, /* tok_kw_for */
    op_none, /* tok_kw_if */
    op_none, /* tok_kw_integer */
    op_mod, /* tok_kw_mod */
    op_not, /* tok_kw_not */
    op_none, /* tok_kw_of */
    op_or, /* tok_kw_or */
    op_none, /* tok_kw_procedure */
    op_none, /* tok_kw_program */
    op_none, /* tok_kw_real */
    op_none, /* tok_kw_text */
    op_none, /* tok_kw_then */
    op_none, /* tok_kw_to */
    op_none, /* tok_kw_true */
    op_none, /* tok_kw_var */
    op_none, /* tok_kw_while */
    op_none, /* tok_undef */
};

const char *builtinNames[builtin_num] =
{
    "chr", /* builtin_chr */
    "eoln", /* builtin_eoln */
    "eof", /* builtin_eof */
    "ord", /* builtin_ord */
    "round", /* builtin_round */
    "sqr", /* builtin_sqr */
    "trunc", /* builtin_trunc */
    "reset", /* builtin_reset */
    "rewrite", /* builtin_rewrite */
    "put", /* builtin_put */
    "get", /* builtin_get */
    "read", /* builtin_read */
    "write", /* builtin_write */
    "readln", /* builtin_readln */
    "writeln", /* builtin_writeln */
    "input", /* builtin_input_var */
    "output" /* builtin_output_var */
};

const char *pjtypeString(pjtype type)
{
    return pjtypeStrings[type];
}

bool ispjop(token_kind tk)
{
    return tokenToOp[tk] != op_none;
}

pjop pjopLookup(token_kind tk)
{
    return tokenToOp[tk];
}

pjtype isConstant(token_kind tk)
{
    switch (tk)
    {
        case tok_real_const:
            return pj_real;
        case tok_integer_const:
            return pj_integer;
        case tok_kw_true:
        case tok_kw_false:
            return pj_boolean;
        case tok_alfa_const:
            return pj_alfa;
        case tok_char_const:
            return pj_char;
        case tok_string_const:
            return pj_string;
        default:
            return pj_undef;
    }
}

const char *pjbuiltinString(pjbuiltin pjb)
{
    return builtinNames[pjb];
}
