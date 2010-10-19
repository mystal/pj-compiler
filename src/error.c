#include "error.h"

char *errorStrings[err_num] =
{
    "unexpected token"
};

char *errorString(error e)
{
    return errorStrings[e];
}
