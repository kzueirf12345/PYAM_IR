#ifndef PYAM_IR_SRC_KEY_WORDS_H
#define PYAM_IR_SRC_KEY_WORDS_H

#include <stdlib.h>

const size_t kIR_KEY_WORD_NUMBER   = 8;
const size_t kIR_KEY_WORD_NAME_MAX = 16;

enum IR_KeyWord_Indexes
{
    IR_FUNCTION_CALL_INDEX = 0   ,
    IR_FUNCTION_BODY_INDEX       ,
    IR_CONDITIONAL_JUMP_INDEX    ,
    IR_ASSIGNMENT_INDEX          ,
    IR_OPERATION_INDEX           ,
    IR_LABEL_INDEX               ,
    IR_RETURN_INDEX              ,
    IR_SYSTEM_FUNCTION_CALL_INDEX,
    IR_INVALID_KEY_WORD = -1,
};

const char kIR_KEY_WORD_ARRAY[kIR_KEY_WORD_NUMBER][kIR_KEY_WORD_NAME_MAX] =
{
    {"RingRing"},
    {"Gyat"},
    {"Frog"},
    {"Gnoming"},
    {"Digging"},
    {"Viperr"},
    {"Cherepovec"},
    {"Bobb"}
};

const size_t kIR_OPERAND_IDENTIFIERS_NUMBER   = 3;
const size_t kIR_OPERAND_IDENTIFIERS_NAME_MAX = 16;
 
// I made both index and code enum in case we will add some non-numerical 
// prefixes to anything, string array will probably be deleted in the future

enum IR_Operands_Identifiers_Indexes
{
    IR_TMP_OPERAND_INDEX,
    IR_VAR_OPERAND_INDEX,
    IR_ARG_OPERAND_INDEX,
};

const size_t kIR_SYS_CALL_NUMBER   = 3;
const size_t kIR_SYS_CALL_NAME_MAX = 16;

enum IR_SysCall_Indexes
{
    SYSCALL_HLT_INDEX = 0   ,
    SYSCALL_IN_INDEX        ,
    SYSCALL_OUT_INDEX       ,
    INVALID_SYSCALL = -1
};

const int kIR_SYSCALL_HLT_ARGUMENTS_NUMBER = 0;
const int kIR_SYSCALL_IN_ARGUMENTS_NUMBER = 1;
const int kIR_SYSCALL_OUT_ARGUMENTS_NUMBER = 0;

const char kIR_SYS_CALL_ARRAY[kIR_SYS_CALL_NUMBER][kIR_SYS_CALL_NAME_MAX] =
{
    {"hlt"},
    {"in"},
    {"out"}
};

/*Если кто-то меняет этот файл, то слеудет произвести изменения и в файле с описанием IR'а - codegen.h*/
// Никакого кодгена больше нет, в данном случае он был лишним + бесполезным + портил читаемость

#endif /*PYAM_IR_SRC_KEY_WORDS_H*/
