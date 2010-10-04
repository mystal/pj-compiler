#include "directive.h"

bool directives[dir_num] =
{
    true, /* dir_listing */
    false, /* dir_token_echo */
    true, /* dir_print_reduction */
    true /* dir_flush_echo */
};

const char dirChars[dir_num] =
{
    'l', /* dir_listing */
    't', /* dir_token_echo */
    'p', /* dir_print_reduction */
    'f' /* dir_flush_echo */
};
