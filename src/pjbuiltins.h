#pragma once

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

char *builtinGetString(pjbuiltin);
