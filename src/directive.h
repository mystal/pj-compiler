#pragma once

#include <stdbool.h>

typedef enum __directive
{
    dir_listing,
    dir_token_echo,
    dir_print_reduction,
    dir_flush_echo
} directive;

/* Flags for current directives */
extern bool directives[];
