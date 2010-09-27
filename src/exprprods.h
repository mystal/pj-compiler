#pragma once

#include "exprsymbol.h"

typedef struct __production
{
    nonterminal lhs;
    unsigned int rhsLen : 8;
} production;

extern const production productions[];

const char *prodString(unsigned int);
