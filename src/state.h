#pragma once

// Shortened names for use in transition table
#define EN state_end
#define BE state_begin
#define ID state_id
#define R0 state_real0
#define R1 state_real1
#define R2 state_real2
#define R3 state_real3
#define R4 state_real4
#define R5 state_real5
#define L0 state_lit0
#define L1 state_lit1
#define C0 state_com0
#define C1 state_com1
#define C2 state_com2
#define C3 state_com3
#define LT state_lt
#define GT state_gt
#define DT state_dot
#define CL state_colon

typedef enum __state
{
    state_end,   /* 0: End state, found successful token */
    state_begin, /* 1: Start state */
    state_id,    /* 2: "a-z" id? fileid? */
    state_real0, /* 3: "0-9" int? real? */
    state_real1, /* 4: "." real? */
    state_real2, /* 5: "0-9" real? */
    state_real3, /* 6: "e" real? */
    state_real4, /* 7: "+-" real? */
    state_real5, /* 8: "0-9" real? */
    state_lit0,  /* 9: "'" literal? */
    state_lit1,  /* 10: "'" literal? */
    state_com0,  /* 11: "(" left parenthesis? comment? */
    state_com1,  /* 12: "*" comment? */
    state_com2,  /* 13: "lambda" comment? */
    state_com3,  /* 14: "*" comment? */
    state_lt,    /* 15: "<" less than (or equal to)? not equal? */
    state_gt,    /* 16: ">" greater than (or equal to)? */
    state_dot,   /* 17: "." dot? rdots? */
    state_colon, /* 18: ":" colon (equal)? */
    state_num
} state;

typedef enum __char_class
{
    ccls_ad_fz, /* a-d,f-z */
    ccls_e,     /* e */
    ccls_09,    /* 0-9 */
    ccls_pm,    /* +- */
    ccls_ast,   /* * */
    ccls_eq,    /* = */
    ccls_lt,    /* < */
    ccls_gt,    /* > */
    ccls_lpar,  /* ( */
    ccls_rpar,  /* ) */
    ccls_acc,   /* /[],; */
    ccls_dot,   /* . */
    ccls_colon, /* : */
    ccls_dsign, /* $ */
    ccls_quote, /* ' */
    ccls_carat, /* ^ */
    ccls_white, /* whitespace */
    ccls_eol,   /* \n */
    ccls_lmbd,  /* other */
    ccls_num
} char_class;
