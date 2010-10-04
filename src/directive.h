#pragma once

#include <stdbool.h>

typedef enum __directive
{
    dir_listing,
    dir_token_echo,
    dir_print_reduction,
    dir_flush_echo,
    dir_num
} directive;

/* Flags for current directives */
extern bool directives[];

extern const char dirChars[];
