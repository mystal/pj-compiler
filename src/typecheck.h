#pragma once

#include <stdbool.h>

#include "pjlang.h"

typedef struct __op_check
{
    pjtype ret;
    bool coerce1;
    bool coerce2;
} op_check;

op_check typeCheckOperator(pjop, pjtype, pjtype);

pjtype typeCheckBuiltinFunction(pjbuiltin, pjtype);
