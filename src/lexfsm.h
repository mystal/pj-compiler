#pragma once

#include "state.h"
#include "token.h"

/**
 * Perform the action for the given state and input character. The given
 * token will be filled with the next token available. The next state
 * will be returned.
 **/
state performAction(state, char, token *);
