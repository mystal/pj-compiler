#pragma once

#include <stdbool.h>

#include "pjlang.h"
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

string *symConstVarGetValue(symbol *);

unsigned int symArrayGetRange(symbol *);

bool symArrayInRange(symbol *, unsigned int);

unsigned int symArrayGetLowBound(symbol *);

unsigned int symProcGetNumParams(symbol *);

const pjtype *symProcGetParams(symbol *);

pjbuiltin symBuiltinGetType(symbol *);

void symVarSetLocation(symbol *, unsigned int);

void symArraySetLocation(symbol *, unsigned int);

void symProcSetLocation(symbol *, unsigned int);

unsigned int symVarGetLocation(symbol *);

unsigned int symArrayGetLocation(symbol *);

unsigned int symProcGetLocation(symbol *);
