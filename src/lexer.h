#pragma once

#include <stdbool.h>

#include "token.h"

/**
 * Initializes the lexer.
 **/
void lexerInit();

/**
 * Gets the next token and places it in the given pointer. Returns
 * true on success and false on failure.
 **/
bool getToken(token *);
