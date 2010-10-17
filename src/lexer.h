#pragma once

#include "token.h"

/**
 * Initializes the lexical analyzer.
 **/
void lexerInit(void);

/**
 * Cleans up the lexical analyzer.
 **/
void lexerCleanup(void);

/**
 * Gets the next token. If an undefined token is returned, an error has
 * occurred, likely the end of the file has been reached.
 **/
token *lexerGetToken(void);
