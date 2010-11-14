#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>

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
    bool *optParams;
    pjtype retType;
    unsigned int loc;
    bool isBuiltin;
} sym_proc;

typedef union __sym_data
{
    sym_var v;
    sym_const_var c;
    sym_array a;
    sym_proc p;
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
//void printBuiltin(string *, sym_builtin);

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
    else if (sym->type == symt_proc && !sym->data.p.isBuiltin)
    {
        if (sym->data.p.paramTypes != NULL)
            free(sym->data.p.paramTypes);
        if (sym->data.p.optParams != NULL)
            free(sym->data.p.optParams);
    }
    stringDestroy(sym->name);
    free(sym);
    sym = NULL;
}

string *symbolGetName(symbol *sym)
{
    return sym->name;
}

void symbolSetType(symbol *sym, sym_type type)
{
    sym->type = type;
    if (type == symt_const_var)
    {
        sym->data.c.value = stringCreate();
    }
    else if (symt_proc)
    {
        sym->data.p.numParams = 0;
        sym->data.p.paramTypes = NULL;
        sym->data.p.optParams = NULL;
        sym->data.p.retType = pj_undef;
        sym->data.p.isBuiltin = false;
    }
}

sym_type symbolGetType(symbol *sym)
{
    return sym->type;
}

void symbolSetPJType(symbol *sym, pjtype pjt)
{
    if (sym->type == symt_var)
        sym->data.v.type = pjt;
    else if (sym->type == symt_const_var)
        sym->data.c.type = pjt;
    else if (sym->type == symt_array)
        sym->data.a.type = pjt;
}

pjtype symbolGetPJType(symbol *sym)
{
    if (sym->type == symt_var)
        return sym->data.v.type;
    if (sym->type == symt_const_var)
        return sym->data.c.type;
    if (sym->type == symt_array)
        return sym->data.a.type;
    return pj_undef;
}

void symbolSetLocation(symbol *sym, unsigned int loc)
{
    if (sym->type == symt_var)
        sym->data.v.loc = loc;
    else if (sym->type == symt_array)
        sym->data.a.loc = loc;
    else if (sym->type == symt_proc)
        sym->data.p.loc = loc;
}

void symbolConstSetValue(symbol *sym, string *str)
{
    stringAppendString(sym->data.c.value, str);
}

string *symbolConstGetValue(symbol *sym)
{
    return sym->data.c.value;
}

void symbolArraySetBounds(symbol *sym, unsigned int low, unsigned int up)
{
    sym->data.a.lowBound = low;
    sym->data.a.upBound = up;
}

void symbolProcSetParams(symbol *sym, unsigned int n, pjtype *types, bool *opts)
{
    sym->data.p.numParams = n;
    sym->data.p.paramTypes = types;
    sym->data.p.optParams = opts;
}

void symbolProcSetReturnType(symbol *sym, pjtype pjt)
{
    sym->data.p.retType = pjt;
}

void symbolProcSetBuiltin(symbol *sym)
{
    sym->data.p.isBuiltin = true;
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
            //printBuiltin(sym->name, sym->data.b);
            break;
        default:
            break;
    }
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
    fprintf(stdout, "), Return Type: %s, Location: %d)\n",
            pjtypeString(proc.retType), proc.loc);
}

/*void printBuiltin(string *name, sym_builtin builtin)
{
}*/
