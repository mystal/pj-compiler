#pragma once

typedef enum __hypo_op
{
    hop_push = 1,
    hop_pushr,
    hop_pushi,
    hop_pushc,
    hop_pop,
    hop_popc,
    hop_popr,
    hop_move,
    hop_swap,
    hop_load,
    hop_loadr,
    hop_loada,
    hop_loadi,
    hop_store,
    hop_storeregs,
    hop_loadregs,
    hop_halt,
    hop_add,
    hop_sub,
    hop_negate,
    hop_mult,
    hop_div,
    hop_mod,
    hop_or,
    hop_and,
    hop_not,
    hop_trunc,
    hop_round,
    hop_float,
    hop_shift,
    hop_gt,
    hop_ge,
    hop_lt,
    hop_le,
    hop_eq,
    hop_ne,
    hop_b,
    hop_bct,
    hop_bcf,
    hop_noop,
    hop_pack,
    hop_unpack,
    hop_syscall,
    hop_call,
    hop_return,
    hop_num
} hypo_op;

typedef enum __hypo_syscall
{
    hsys_open = 1,
    hsys_close,
    hsys_get,
    hsys_put,
    hsys_readln,
    hsys_writeln,
    hsys_readbuff,
    hsys_writebuff,
    hsys_readint,
    hsys_readreal,
    hsys_writeint,
    hsys_writereal,
    hsys_clock,
    hsys_eoln,
    hsys_eof,
    hsys_dumpit,
    hsys_chr,
    hsys_ord,
    hsys_sqr,
    hsys_dumpr,
    hsys_dumpi,
    hsys_dumpd,
    hsys_linelimit,
    hsys_instlim,
    hsys_num
} hypo_syscall;

typedef char alfa[10];
typedef enum __hypo_type {h_int, h_real, h_char, h_alfa, h_bool} hypo_type;
typedef enum __spaces { ispace, dspace, regs } spaces;

typedef struct __varval
{
	hypo_type type;
	union {
		int intval;
		float realval;
		char charval;
		alfa alfaval;   /* array of 10 characters */
		char boolval;   /* set to NUMERIC 0 or 1 */
	} ircab_val;
} varval;

typedef struct __instruction {
	int opcode;
	varval oper1;
	int oper2;
} instruction;

typedef struct __reg
{
	int regnum, regval;
} reg;

typedef struct __loadrec
{
	int loc;
	spaces space;
	union {
		instruction ivalue;
		varval dvalue;
		reg rvalue;
	} idr_value;
} loadrec;

const char *hypomacGetOpString(hypo_op);

const char *hypomacGetSyscallString(hypo_syscall);

const char *hypomacGetSpaceString(spaces);
