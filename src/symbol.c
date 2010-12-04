#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>

#include "pjlang.h"
#include "str.h"

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
    pjtype *paramTypes;
    unsigned int loc;
} sym_proc;

typedef struct __sym_builtin
{
    pjbuiltin pjb;
} sym_builtin;

typedef union __sym_data
{
    sym_var v;
    sym_const_var c;
    sym_array a;
    sym_proc p;
    sym_builtin b;
} sym_data;

struct __symbol
{
    string *name;
    sym_type type;
    sym_data data;
};

/* Private helper functions */
void printVar(string *, sym_var);
void printConst(string *, sym_const_var);
void printArray(string *, sym_array);
void printProc(string *, sym_proc);
void printBuiltin(string *, sym_builtin);

symbol *symbolCreate(string *str)
{
    symbol *sym = (symbol *) malloc(sizeof(symbol));
    sym->name = stringCreate();
    stringAppendString(sym->name, str);
    sym->type = symt_undef;
    return sym;
}

void symbolDestroy(symbol *sym)
{
    if (sym->type == symt_const_var)
    {
        stringDestroy(sym->data.c.value);
    }
    else if (sym->type == symt_proc && sym->data.p.numParams != 0)
    {
        free(sym->data.p.paramTypes);
    }
    stringDestroy(sym->name);
    free(sym);
    sym = NULL;
}

void symbolSetVar(symbol *sym, pjtype pjt)
{
    sym->type = symt_var;
    sym->data.v.type = pjt;
}

void symbolSetConstVar(symbol *sym, pjtype pjt, string *val)
{
    sym->type = symt_const_var;
    sym->data.c.type = pjt;
    sym->data.c.value = stringCreate();
    stringAppendString(sym->data.c.value, val);
}

void symbolSetArray(symbol *sym, pjtype pjt, unsigned int low, unsigned int up)
{
    sym->type = symt_array;
    sym->data.a.type = pjt;
    sym->data.a.lowBound = low;
    sym->data.a.upBound = up;
}

void symbolSetProc(symbol *sym, unsigned int n, pjtype *types)
{
    sym->type = symt_proc;
    sym->data.p.numParams = n;
    sym->data.p.paramTypes = types;
}

void symbolSetBuiltin(symbol *sym, pjbuiltin pjb)
{
    sym->type = symt_builtin;
    sym->data.b.pjb = pjb;
}

string *symbolGetName(symbol *sym)
{
    return sym->name;
}

sym_type symbolGetType(symbol *sym)
{
    return sym->type;
}

void symbolPrint(symbol *sym)
{
    switch (sym->type)
    {
        case symt_var:
            printVar(sym->name, sym->data.v);
            break;
        case symt_const_var:
            printConst(sym->name, sym->data.c);
            break;
        case symt_array:
            printArray(sym->name, sym->data.a);
            break;
        case symt_proc:
            printProc(sym->name, sym->data.p);
            break;
        case symt_builtin:
            printBuiltin(sym->name, sym->data.b);
            break;
        default:
            break;
    }
}

pjtype symVarGetType(symbol *sym)
{
    return sym->data.v.type;
}

pjtype symConstVarGetType(symbol *sym)
{
    return sym->data.c.type;
}

pjtype symArrayGetType(symbol *sym)
{
    return sym->data.a.type;
}

string *symConstVarGetValue(symbol *sym)
{
    return sym->data.c.value;
}

unsigned int symArrayGetRange(symbol *sym)
{
    return sym->data.a.upBound - sym->data.a.lowBound + 1;
}

bool symArrayInRange(symbol *sym, unsigned int index)
{
    return index >= sym->data.a.lowBound && index <= sym->data.a.upBound;
}

unsigned int symArrayGetLowBound(symbol *sym)
{
    return sym->data.a.lowBound;
}

unsigned int symProcGetNumParams(symbol *sym)
{
    return sym->data.p.numParams;
}

const pjtype *symProcGetParams(symbol *sym)
{
    return sym->data.p.paramTypes;
}

pjbuiltin symBuiltinGetType(symbol *sym)
{
    return sym->data.b.pjb;
}

void symVarSetLocation(symbol *sym, unsigned int loc)
{
    sym->data.v.loc = loc;
}

void symArraySetLocation(symbol *sym, unsigned int loc)
{
    sym->data.a.loc = loc;
}

void symProcSetLocation(symbol *sym, unsigned int loc)
{
    sym->data.p.loc = loc;
}

unsigned int symVarGetLocation(symbol *sym)
{
    return sym->data.v.loc;
}

unsigned int symArrayGetLocation(symbol *sym)
{
    return sym->data.a.loc;
}

unsigned int symProcGetLocation(symbol *sym)
{
    return sym->data.p.loc;
}

void printVar(string *name, sym_var var)
{
    fprintf(stdout, "\tVar(Name: %.*s, Type: %s, Location: %d)\n",
            stringGetLength(name), stringGetBuffer(name),
            pjtypeString(var.type), var.loc);
}

void printConst(string *name, sym_const_var const_var)
{
    fprintf(stdout, "\tConst(Name: %.*s, Type: %s, Value: %.*s)\n",
            stringGetLength(name), stringGetBuffer(name),
            pjtypeString(const_var.type), stringGetLength(const_var.value),
            stringGetBuffer(const_var.value));
}

void printArray(string *name, sym_array arr)
{
    fprintf(stdout, "\tArray(Name: %.*s, Type: %s, Bounds: [%d, %d], Location: %d)\n",
            stringGetLength(name), stringGetBuffer(name),
            pjtypeString(arr.type), arr.lowBound, arr.upBound, arr.loc);
}

void printProc(string *name, sym_proc proc)
{
    fprintf(stdout, "\tProc(Name: %.*s, Params: (", stringGetLength(name),
            stringGetBuffer(name));
    if (proc.numParams > 0)
    {
        for (unsigned int i = 0; i < proc.numParams-1; i++)
            fprintf(stdout, "%s, ", pjtypeString(proc.paramTypes[i]));
        fprintf(stdout, "%s", pjtypeString(proc.paramTypes[proc.numParams-1]));
    }
    fprintf(stdout, "), Start: %d)\n", proc.loc);
}

void printBuiltin(string *name, sym_builtin builtin)
{
    fprintf(stdout, "\tBuiltin(Name: %.*s)\n", stringGetLength(name),
            stringGetBuffer(name));
}
