#pragma once

#include "str.h"

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
    tok_lbrack,
    tok_rbrack,
    tok_lthan,
    tok_gthan,
    tok_ltequal,
    tok_gtequal,
    tok_nequal,
    tok_equal,
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
    tok_kw_alfa,
    tok_kw_and,
    tok_kw_array,
    tok_kw_begin,
    tok_kw_boolean,
    tok_kw_char,
    tok_kw_const,
    tok_kw_div,
    tok_kw_do,
    tok_kw_downto,
    tok_kw_else,
    tok_kw_end,
    tok_kw_false,
    tok_kw_for,
    tok_kw_if,
    tok_kw_integer,
    tok_kw_mod,
    tok_kw_not,
    tok_kw_of,
    tok_kw_or,
    tok_kw_procedure,
    tok_kw_program,
    tok_kw_real,
    tok_kw_text,
    tok_kw_then,
    tok_kw_to,
    tok_kw_true,
    tok_kw_var,
    tok_kw_while,
    tok_undef
} token_kind;

typedef struct __token
{
    string lexeme;
    token_kind kind;
} token;

void tokenInit(token *);

void tokenClean(token *);

char * tokenKindString(token_kind);
