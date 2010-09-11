#pragma once

#include "string.h"

typedef enum __token_kind
{
    tok_id,
    tok_fileid,
    tok_integer_const,
    tok_real_const,
    tok_literal,
    tok_char_const,
    tok_alfa_const,
    tok_string_const,
    tok_comment,
    tok_lparen,
    tok_rparen,
    tok_lsquare,
    tok_rsquare,
    tok_lthan,
    tok_gthan,
    tok_ltequal,
    tok_gtequal,
    tok_nequal,
    tok_dot,
    tok_rdots,
    tok_comma,
    tok_semicolon,
    tok_colon,
    tok_colonequal,
    tok_plus,
    tok_minus,
    tok_asterisk,
    tok_slash,
    tok_key_alfa,
    tok_key_and,
    tok_key_array,
    tok_key_begin,
    tok_key_boolean,
    tok_key_char,
    tok_key_const,
    tok_key_div,
    tok_key_do,
    tok_key_downto,
    tok_key_else,
    tok_key_end,
    tok_key_false,
    tok_key_for,
    tok_key_if,
    tok_key_integer,
    tok_key_mod,
    tok_key_not,
    tok_key_of,
    tok_key_or,
    tok_key_procedure,
    tok_key_program,
    tok_key_real,
    tok_key_text,
    tok_key_then,
    tok_key_to,
    tok_key_true,
    tok_key_var,
    tok_key_while
    tok_end
} token_kind;

typedef struct __token
{
    string lexeme;
    token_kind kind;
} token;
