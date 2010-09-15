#pragma once

typedef enum __state
{
    state_end,   /* End state, found successful token */
    state_begin, /* Start state */
    state_id0,   /* "a-z" id? fileid? */
    state_real0, /* "0-9" int? real? */
    state_real1, /* "." real? */
    state_real2, /* "0-9" real? */
    state_real3, /* "e" real? */
    state_real4, /* "+-" real? */
    state_real5, /* "0-9" real? */
    state_lit0,  /* "'" literal? */
    state_lit1,  /* "'" literal? */
    state_com0,  /* "(" left parenthesis? comment? */
    state_com1,  /* "*" comment? */
    state_com2,  /*  */
    state_com3,  
    state_lt,    /* "<" less than (or equal to)? not equal? */
    state_gt,    /* ">" greater than (or equal to)? */
    state_dot,   /* "." dot? rdots? */
    state_colon, /* ":" colon (equal)? */
    state_num
} state;

/* a-d,f-z e 0-9 + - * / = < > [ ] . , : ; ^ ( ) whitespace EOL lambda */

typedef enum __char_class
{
    ccls_ad_fz,  /* a-d,f-z */
    ccls_e,      /* e */
    ccls_09,     /* 0-9 */
    ccls_plus,   /* + */
    ccls_minus,  /* - */
    ccls_ast,    /* * */
    ccls_div,    /* / */
    ccls_eq,     /* = */
    ccls_lt,     /* < */
    ccls_gt,     /* > */
    ccls_lbrack, /* [ */
    ccls_rbrack, /* ] */
    ccls_dot,    /* . */
    ccls_comma,
    ccls_colon,
    ccls_semi,
    ccls_carat,
    ccls_lparen,
    ccls_rparen,
    ccls_whitespace,
    ccls_eol,
    ccls_lambda,
    ccls_num
} char_class;
