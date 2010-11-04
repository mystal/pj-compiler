#include "pjtype.h"

char *pjtypeStrings[pj_undef+1] =
{
    "integer", /* pj_integer */
    "real", /* pj_real */
    "alfa", /* pj_alfa */
    "boolean", /* pj_boolean */
    "char", /* pj_char */
    "string", /* pj_string */
    "text", /* pj_text */
    "none" /* pj_undef */
};

char *pjtypeString(pjtype type)
{
    return pjtypeStrings[type];
}
