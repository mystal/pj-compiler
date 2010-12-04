#include "hypomac.h"

const char *hypoOpStrings[hop_num-1] =
{
    "push", /* hop_push */
    "pushr", /* hop_pushr */
    "pushi", /* hop_pushi */
    "pushc", /* hop_pushc */
    "pop", /* hop_pop */
    "popc", /* hop_popc */
    "popr", /* hop_popr */
    "move", /* hop_move */
    "swap", /* hop_swap */
    "load", /* hop_load */
    "loadr", /* hop_loadr */
    "loada", /* hop_loada */
    "loadi", /* hop_loadi */
    "store", /* hop_store */
    "storeregs", /* hop_storeregs */
    "loadregs", /* hop_loadregs */
    "halt", /* hop_halt */
    "add", /* hop_add */
    "sub", /* hop_sub */
    "negate", /* hop_negate */
    "mult", /* hop_mult */
    "div", /* hop_div */
    "mod", /* hop_mod */
    "or", /* hop_or */
    "and", /* hop_and */
    "not", /* hop_not */
    "trunc", /* hop_trunc */
    "round", /* hop_round */
    "float", /* hop_float */
    "shift", /* hop_shift */
    "gt", /* hop_gt */
    "ge", /* hop_ge */
    "lt", /* hop_lt */
    "le", /* hop_le */
    "eq", /* hop_eq */
    "ne", /* hop_ne */
    "b", /* hop_b */
    "bct", /* hop_bct */
    "bcf", /* hop_bcf */
    "noop", /* hop_noop */
    "pack", /* hop_pack */
    "unpack", /* hop_unpack */
    "syscall", /* hop_syscall */
    "call", /* hop_call */
    "return" /* hop_return */
};

const char *hypoSysStrings[hsys_num-1] =
{
    "open", /* hsys_open */
    "close", /* hsys_close */
    "get", /* hsys_get */
    "put", /* hsys_put */
    "readln", /* hsys_readln */
    "writeln", /* hsys_writeln */
    "readbuff", /* hsys_readbuff */
    "writebuff", /* hsys_writebuff */
    "readint", /* hsys_readint */
    "readreal", /* hsys_readreal */
    "writeint", /* hsys_writeint */
    "writereal", /* hsys_writereal */
    "clock", /* hsys_clock */
    "eoln", /* hsys_eoln */
    "eof", /* hsys_eof */
    "dumpit", /* hsys_dumpit */
    "chr", /* hsys_chr */
    "ord", /* hsys_ord */
    "sqr", /* hsys_sqr */
    "dumpr", /* hsys_dumpr */
    "dumpi", /* hsys_dumpi */
    "dumpd", /* hsys_dumpd */
    "linelimit", /* hsys_linelimit */
    "instlim", /* hsys_instlim */
};

const char *spaceStrings[3] = {"ispace", "dspace", "register"};

const char *hypomacGetOpString(hypo_op op)
{
    return hypoOpStrings[op-1];
}

const char *hypomacGetSyscallString(hypo_syscall sys)
{
    return hypoSysStrings[sys-1];
}

const char *hypomacGetSpaceString(spaces sp)
{
    return spaceStrings[sp];
}
