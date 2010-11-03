#pragma once

#include "pjtype.h"
#include "str.h"

typedef enum __sym_type
{
    symt_var,
    symt_const_var,
    symt_array,
    symt_proc,
    symt_num
} sym_type;

typedef struct __sym_var
{
    pjtype type;
    unsigned int loc;
} sym_var;

typedef struct __sym_const_var
{
    pjtype type;
    string *value;
} sym_const_var;

typedef struct __sym_array
{
    pjtype type;
    unsigned int lowBound;
    unsigned int upBound;
    unsigned int loc;
} sym_array;

typedef struct __sym_proc
{
    unsigned int numParams;
    pjtype *params;
    unsigned int loc;
} sym_proc;

typedef union __sym_info
{
    sym_var v;
    sym_const_var c;
    sym_array a;
    sym_proc p;
} sym_info;

typedef struct __symbol
{
    string *name;
    sym_type type;
    sym_info data;
} symbol;
