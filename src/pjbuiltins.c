#include "pjbuiltins.h"

char *builtinNames[builtin_num] =
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

//pjtype builtinParamTypes[builtin_num][2] =
//{
//    {pj_integer, pj_undef}, /* builtin_chr */
//    {pj_text, pj_undef}, /* builtin_eoln */
//    {pj_text, pj_undef}, /* builtin_eof */
//    {pj_char, pj_undef}, /* builtin_ord */
//    {pj_integer, pj_undef}, /* builtin_round */
//    {pj_real, pj_undef}, /* builtin_sqr */
//    {pj_real, pj_undef}, /* builtin_trunc */
//    {pj_text, pj_undef}, /* builtin_reset */
//    {pj_text, pj_undef}, /* builtin_rewrite */
//    {pj_text, pj_undef}, /* builtin_put */
//    {pj_text, pj_undef}, /* builtin_get */
//    {pj_text, pj_integer}, /* builtin_read */
//    {pj_text, pj_string}, /* builtin_write */
//    {pj_text, pj_integer}, /* builtin_readln */
//    {pj_text, pj_string} /* builtin_writeln */
//};

//bool builtinOptParams[builtin_num] [2]=
//{
//    {false, false}, /* builtin_chr */
//    {false, false}, /* builtin_eoln */
//    {false, false}, /* builtin_eof */
//    {false, false}, /* builtin_ord */
//    {false, false}, /* builtin_round */
//    {false, false}, /* builtin_sqr */
//    {false, false}, /* builtin_trunc */
//    {false, false}, /* builtin_reset */
//    {false, false}, /* builtin_rewrite */
//    {false, false}, /* builtin_put */
//    {false, false}, /* builtin_get */
//    {false, true}, /* builtin_read */
//    {false, true}, /* builtin_write */
//    {false, true}, /* builtin_readln */
//    {false, true} /* builtin_writeln */
//};

//pjtype builtinRetTypes[builtin_num] =
//{
//    pj_char, /* builtin_chr */
//    pj_boolean, /* builtin_eoln */
//    pj_boolean, /* builtin_eof */
//    pj_integer, /* builtin_ord */
//    pj_integer, /* builtin_round */
//    pj_real, /* builtin_sqr */
//    pj_integer, /* builtin_trunc */
//    pj_undef, /* builtin_reset */
//    pj_undef, /* builtin_rewrite */
//    pj_undef, /* builtin_put */
//    pj_undef, /* builtin_get */
//    pj_undef, /* builtin_read */
//    pj_undef, /* builtin_write */
//    pj_undef, /* builtin_readln */
//    pj_undef /* builtin_writeln */
//};

char *builtinGetString(pjbuiltin pjb)
{
    return builtinNames[pjb];
}
