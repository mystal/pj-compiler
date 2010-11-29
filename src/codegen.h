#pragma once

#include "list.h"
#include "pjlang.h"
#include "string.h"
#include "symbol.h"
#include "symtable.h"

typedef enum __slr_sem_kind
{
    slr_sem_symbol,
    slr_sem_constant,
    slr_sem_operator,
    slr_sem_type,
    slr_sem_none
} slr_sem_kind;

typedef union __slr_sem
{
    struct
    {
        symbol *val;
        unsigned int level;
    } sym;
    struct
    {
        string *val;
        pjtype type;
    } constVal;
    pjop op;
    pjtype type;
} slr_sem;

typedef struct __slr_semantics
{
    slr_sem_kind kind;
    slr_sem sem;
} slr_semantics;

void codegenInit(void);

void codegenCleanup(void);

pjtype codegenExpr(unsigned int, list *, symtable *);

void codegenReportError(void);
