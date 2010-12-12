#pragma once

#include "hypomac.h"
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

/* Semantics for the SLR expression parser */
typedef struct __slr_semantics
{
    slr_sem_kind kind;
    slr_sem sem;
} slr_semantics;

/**
 * Initialize the code generation module.
 **/
void codegenInit(void);

/**
 * Clean up the code generation module.
 **/
void codegenCleanup(void);

/**
 * Generate code for the given instruction and its operators at the next
 * location in ispace.
 **/
void codegenInstruction(hypo_op, varval, int);

/**
 * Generate code for the given instruction and its operators at the given
 * location in ispace.
 **/
void codegenInstructionAt(hypo_op, varval, int, unsigned int);

/**
 * Returns the address of the given symbol at the given location.
 **/
int codegenIdAddress(symbol *, unsigned int);

/**
 * Generates code to get the address of the given symbol array at the
 * given level.
 **/
void codegenArrayAddress(symbol *, unsigned int);

/**
 * Generates code to call the given user-defined procedure at the given level.
 **/
void codegenProcedureCall(symbol *, unsigned int);

/**
 * Generates code to perform the given bulitin procedure on the given filename,
 * using the provided type as the second argument, if needed.
 **/
void codegenBuiltinProcedure(pjbuiltin, string *, pjtype);

/**
 * Generate code for the given reduction, provided the current semantics.
 **/
pjtype codegenExpr(unsigned int, list *, symtable *);

/**
 * Increments the next location.
 **/
void codegenIncrementNextLocation(void);

/**
 * Returns the next location in ispace.
 **/
unsigned int codegenGetNextLocation(void);

/**
 * Report an error to the code generation module. Stops any further code
 * generation from occurring.
 **/
void codegenReportError(void);
