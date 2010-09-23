#pragma once

#include "token.h"

/**
 * Gets the next token and places it in the given pointer. If an
 * undefined token is returned, an error has occurred, likely the
 * end of the file has been reached.
 **/
void lexerGetToken(token *);
