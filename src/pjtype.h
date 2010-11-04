#pragma once

typedef enum __pjtype
{
    pj_integer,
    pj_real,
    pj_alfa,
    pj_boolean,
    pj_char,
    pj_string,
    pj_text,
    pj_undef
} pjtype;

char *pjtypeString(pjtype);
