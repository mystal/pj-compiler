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
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_identity */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_negate */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_and */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_or */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_not */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_lt */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_lteq */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_eq */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_gteq */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_gt */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF}},
    /* op_neq */
    {{DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
     {DEF, DEF, DEF, DEF, DEF, DEF, DEF, DEF},
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
    if (pjb > builtin_trunc)
        return pj_undef;
    return builtinFunctionTypes[pjb][pjt];
}
