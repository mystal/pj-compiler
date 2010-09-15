#pragma once

#include <stdbool.h>

typedef enum __directive
{
    dir_listing,
    dir_token_echo
} directive;

extern bool directives[];
