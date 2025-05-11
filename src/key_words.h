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

const char kIR_KEY_WORD_ARRAY[kIR_KEY_WORD_NUMBER][kIR_KEY_WORD_NAME_MAX]=
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

const size_t kIR_SYS_CALL_NUMBER   = 3;
const size_t kIR_SYS_CALL_NAME_MAX = 16;

enum IR_SysCall_Indexes
{
    HLT_INDEX = 0   ,
    IN_INDEX        ,
    OUT_INDEX       ,
    INVALID_SYSCALL = -1
};

const char kIR_SYS_CALL_ARRAY[kIR_SYS_CALL_NUMBER][kIR_SYS_CALL_NAME_MAX]=
{
    {"hlt"},
    {"in"},
    {"out"}
};

/*Если кто-то меняет этот файл, то слеудет произвести изменения и в файле с описанием IR'а - codegen.h*/
// Никакого кодгена больше нет, в данном случае он был лишним + бесполезным + портил читаемость

#endif /*PYAM_IR_SRC_KEY_WORDS_H*/
