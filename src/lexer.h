#pragma once

#include <stdbool.h>

#include "token.h"

void lexerInit();

bool getToken(token *);
