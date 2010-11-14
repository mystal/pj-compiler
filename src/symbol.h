#pragma once

#include <stdbool.h>

#include "pjbuiltins.h"
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

/* General symbol functions */

symbol *symbolCreate(string *);

void symbolDestroy(symbol *);

void symbolSetVar(symbol *, pjtype);

void symbolSetConstVar(symbol *, pjtype, string *);

void symbolSetArray(symbol *, pjtype, unsigned int, unsigned int);

void symbolSetProc(symbol *, unsigned int, pjtype *);

void symbolSetBuiltin(symbol *, pjbuiltin);

string *symbolGetName(symbol *);

sym_type symbolGetType(symbol *);

void symbolPrint(symbol *);

/* Specific symbol type functions */

pjtype symVarGetType(symbol *);

pjtype symConstVarGetType(symbol *);

pjtype symArrayGetType(symbol *);

string *symConstGetValue(symbol *);

void symVarSetLocation(symbol *, unsigned int);

void symArraySetLocation(symbol *, unsigned int);

void symProcSetLocation(symbol *, unsigned int);
