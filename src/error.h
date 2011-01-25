#pragma once

#include "token.h"

typedef enum __error_kind
{
    err_unex,
    err_unex_expr,
    err_exp_idint,
    err_exp_downto,
    err_exp_const,
    err_exp_stype,
    err_exp_end,
    err_exp_dot,
    err_badstmt,
    err_dup_sym,
    err_undef_sym,
    err_file_not_text,
    err_undecl_file,
    err_range_not_const,
    err_redef_builtin,
    err_op_type_mismatch,
    err_bad_fileptr,
    err_cond_not_bool,
    err_for_lcv_mod,
    err_for_lcv_not_int,
    err_for_lim_not_int,
    err_wrong_num_params,
    err_wrong_param_type,
    err_num
} error_kind;

/**
 * Prints the given error and the line it was found on.
 **/
void errorParse(error_kind, token *, token_kind);

/**
 * Prints errors related to the symbols and the symbol table. 
 **/
void errorST(error_kind, string *);

/**
 * Prints type errors, usually related to type checking.
 **/
void errorType(error_kind);
