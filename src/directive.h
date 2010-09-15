#pragma once

#include <stdbool.h>

typedef enum __directive
{
    dir_listing,
    dir_token_echo
} directive;

static bool directives[] =
{
    true, /* dir_listing */
    true  /* dir_token_echo */
};
