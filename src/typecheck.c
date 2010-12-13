#include "typecheck.h"

#include <stdbool.h>

#include "pjlang.h"

#define DEF {pj_undef, false, false}
op_check operatorTypes[op_none][pj_undef][pj_undef+1] =
{
    /* op_add */
    {{{pj_integer, false, false}, {pj_real, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_real, false, true}, {pj_real, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_sub */
    {{{pj_integer, false, false}, {pj_real, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_real, false, true}, {pj_real, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_mult */
    {{{pj_integer, false, false}, {pj_real, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_real, false, true}, {pj_real, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_slash */
    {{{pj_real, true, true}, {pj_real, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_real, false, true}, {pj_real, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_div */
    {{{pj_integer, false, false}, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_mod */
    {{{pj_integer, false, false}, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_negate */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, {pj_integer, false, false}},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, {pj_real, false, false}},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_and */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF}},
    /* op_or */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF}},
    /* op_not */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, {pj_boolean, false, false}}},
    /* op_lt */
    {{{pj_boolean, false, false}, {pj_boolean, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_boolean, false, true}, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_lteq */
    {{{pj_boolean, false, false}, {pj_boolean, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_boolean, false, true}, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_eq */
    {{{pj_boolean, false, false}, {pj_boolean, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_boolean, false, true}, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_gteq */
    {{{pj_boolean, false, false}, {pj_boolean, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_boolean, false, true}, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_gt */
    {{{pj_boolean, false, false}, {pj_boolean, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_boolean, false, true}, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_neq */
    {{{pj_boolean, false, false}, {pj_boolean, true, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {{pj_boolean, false, true}, {pj_boolean, false, false}, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}}
};

pjtype builtinFunctionTypes[7][pj_undef] =
{
    /* builtin_chr */
    {pj_char, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef},
    /* builtin_eoln */
    {pj_undef, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef, pj_boolean},
    /* builtin_eof */
    {pj_undef, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef, pj_boolean},
    /* builtin_ord */
    {pj_undef, pj_undef, pj_undef, pj_undef, pj_integer, pj_undef, pj_undef},
    /* builtin_round */
    {pj_undef, pj_integer, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef},
    /* builtin_sqr */
    {pj_integer, pj_real, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef},
    /* builtin_trunc */
    {pj_undef, pj_integer, pj_undef, pj_undef, pj_undef, pj_undef, pj_undef}
};

bool builtinProcedureChecks[8][pj_undef+1] =
{
    /* builtin_read */
    {true, true, true, true, true, false, false, false},
    /* builtin_write */
    {true, true, true, true, true, true, false, false},
    /* builtin_readln */
    {true, true, true, true, true, false, false, false},
    /* builtin_writeln */
    {true, true, true, true, true, true, false, true}
};

op_check typeCheckOperator(pjop op, pjtype pjt1, pjtype pjt2)
{
    if (pjt1 > pj_boolean)
    {
        op_check ret = DEF;
        return ret;
    }
    return operatorTypes[op][pjt1][pjt2];
}

pjtype typeCheckBuiltinFunction(pjbuiltin pjb, pjtype pjt)
{
    if (!ispjbuiltinFunction(pjb))
        return pj_undef;
    return builtinFunctionTypes[pjb][pjt];
}

bool typeCheckBuiltinProcedure(pjbuiltin pjb, pjtype pjt1, pjtype pjt2)
{
    if (!ispjbuiltinProcedure(pjb) || pjt1 != pj_text)
        return false;
    if (pjb < builtin_read)
        return true;
    return builtinProcedureChecks[pjb-builtin_read][pjt2];
}
