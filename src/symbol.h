#pragma once

#include <stdbool.h>

#include "pjtype.h"
#include "str.h"

typedef enum __sym_type
{
    symt_var,
    symt_const_var,
    symt_array,
    symt_proc,
    symt_builtin,
    symt_undef
} sym_type;

typedef struct __symbol symbol;

symbol *symbolCreate(string *);

void symbolDestroy(symbol *);

string *symbolGetName(symbol *);

void symbolSetType(symbol *, sym_type);

sym_type symbolGetType(symbol *);

void symbolSetPJType(symbol *, pjtype);

pjtype symbolGetPJType(symbol *);

void symbolSetLocation(symbol *, unsigned int);

void symbolConstSetValue(symbol *, string *);

string *symbolConstGetValue(symbol *);

void symbolArraySetBounds(symbol *, unsigned int, unsigned int);

void symbolProcSetParams(symbol *, unsigned int, pjtype *, bool *);

void symbolProcSetReturnType(symbol *, pjtype);

void symbolProcSetBuiltin(symbol *);

void symbolPrint(symbol *);
