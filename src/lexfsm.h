#pragma once

#include "state.h"
#include "token.h"

void fsmInit();

state performAction(state, char, token *);
