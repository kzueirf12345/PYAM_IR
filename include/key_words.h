#ifndef PYAM_IR_SRC_KEY_WORDS_H
#define PYAM_IR_SRC_KEY_WORDS_H

#include <stdlib.h>
#include <stdbool.h>

#define kIR_KEY_WORD_NUMBER    9
#define kIR_KEY_WORD_NAME_MAX  16

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
    IR_GLOBAL_VARS_NUM_INDEX     ,
    IR_INVALID_KEY_WORD = -1,
};

static const char kIR_KEY_WORD_ARRAY[kIR_KEY_WORD_NUMBER][kIR_KEY_WORD_NAME_MAX] =
{
    {"RingRing"},
    {"Gyat"},
    {"Frog"},
    {"Gnoming"},
    {"Digging"},
    {"Viperr"},
    {"Cherepovec"},
    {"Bobb"},
    {"Gg"},
};

#define kIR_OPERAND_IDENTIFIERS_NUMBER   3
#define kIR_OPERAND_IDENTIFIERS_NAME_MAX 16

#define kIR_SYS_CALL_NUMBER   5
#define kIR_SYS_CALL_NAME_MAX 16

enum IR_SysCall_Indexes
{
    SYSCALL_HLT_INDEX = 0   ,
    SYSCALL_IN_INDEX        ,
    SYSCALL_OUT_INDEX       ,
    SYSCALL_POW_INDEX       ,
    SYSCALL_SQRT_INDEX      ,
    INVALID_SYSCALL = -1
};

typedef struct PYAM_IR_SystemCall
{
    char Name[kIR_SYS_CALL_NAME_MAX];
    int NumberOfArguments;
    bool HaveRetVal;
} PYAM_IR_SystemCall_t;


static const int kIR_SYSCALL_HLT_ARGUMENTS_NUMBER = 1;
static const int kIR_SYSCALL_IN_ARGUMENTS_NUMBER = 0;
static const int kIR_SYSCALL_OUT_ARGUMENTS_NUMBER = 1;
static const int kIR_SYSCALL_POW_ARGUMENTS_NUMBER = 2;
static const int kIR_SYSCALL_SQRT_ARGUMENTS_NUMBER = 1;

static const PYAM_IR_SystemCall_t kIR_SYS_CALL_ARRAY[kIR_SYS_CALL_NUMBER] =
{
    {"hlt", kIR_SYSCALL_HLT_ARGUMENTS_NUMBER, false},
    {"in", kIR_SYSCALL_IN_ARGUMENTS_NUMBER, true},
    {"out", kIR_SYSCALL_OUT_ARGUMENTS_NUMBER, false},
    {"pow", kIR_SYSCALL_POW_ARGUMENTS_NUMBER, true},
    {"sqrt", kIR_SYSCALL_SQRT_ARGUMENTS_NUMBER, true},
};

#endif /*PYAM_IR_SRC_KEY_WORDS_H*/
