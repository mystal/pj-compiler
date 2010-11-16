#pragma once

#include "symtable.h"
#include "token.h"

/**
 * Parses an expression. The given pointer must point to the first token in
 * the expression. Returns with the next non-expression token in the given
 * pointer.
 **/
void expr(token *, symtable *);
