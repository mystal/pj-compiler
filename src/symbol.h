#pragma once

#include "pjtype.h"
#include "str.h"

typedef enum __sym_type
{
    sym_var,
    sym_const_var,
    sym_array,
    sym_proc,
    sym_num
} sym_type;

typedef struct __var
{
    pjtype type;
    unsigned int loc;
} var;

typedef struct __const_var
{
    pjtype type;
    string *value;
} const_var;

typedef struct __array
{
    pjtype type;
    unsigned int lowBound;
    unsigned int upBound;
    unsigned int loc;
} array;

typedef struct __proc
{
    unsigned int numParams;
    pjtype *params;
    unsigned int loc;
} proc;

typedef union __sym_info
{
    var v;
    const_var c;
    array a;
    proc p;
} sym_info;

typedef struct __symbol
{
    string *name;
    sym_type type;
    sym_info data;
} symbol;
