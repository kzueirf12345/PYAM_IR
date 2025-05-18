#ifndef PYAM_IR_SRC_OPERATIONS_H
#define PYAM_IR_SRC_OPERATIONS_H

enum IrOperandType
{
    IR_OPERAND_TYPE_NONE        = 0b0000001,
    IR_OPERAND_TYPE_NUM         = 0b0000010,
    IR_OPERAND_TYPE_TMP         = 0b0000100,
    IR_OPERAND_TYPE_VAR         = 0b0001000,
    IR_OPERAND_TYPE_ARG         = 0b0010000,
    IR_OPERAND_TYPE_LABEL       = 0b0100000,
    IR_OPERAND_TYPE_OPERATION   = 0b1000000,
};

enum IrOpType
{
    IR_OP_TYPE_SUM              = 1,
    IR_OP_TYPE_SUB              = 2,
    IR_OP_TYPE_MUL              = 3,
    IR_OP_TYPE_DIV              = 4,
    IR_OP_TYPE_EQ               = 5,
    IR_OP_TYPE_NEQ              = 6,
    IR_OP_TYPE_LESS             = 7,
    IR_OP_TYPE_LESSEQ           = 8,
    IR_OP_TYPE_GREAT            = 9,
    IR_OP_TYPE_GREATEQ          = 10,
    IR_OP_TYPE_INVALID_OPERATION = -1
};

#define IR_OP_BLOCK_HANDLE(num_, name_, ...)                                                        \
        IR_OP_BLOCK_TYPE_##name_ = num_,

enum IrOpBlockType
{
#include "codegen.h"
    IR_OP_BLOCK_TYPE_INVALID       = -1
};

#undef IR_OP_BLOCK_HANDLE

////                                               PYAM_IR table (PYAMIRT) for better understanding                                                ////
////-----------------------------------------------------------------------------------------------------------------------------------------------////
// ||    Default name    ||  PYAM name  ||     Return value     ||       Label       ||  Operation  ||     First Operand     ||    Second Operand    ||
// ||  Call function     ||  RingRing   ||  Tmp                 ||  Function label   ||  None       ||  None                 ||  None                ||
// ||  Function body     ||  Gyat       ||  None                ||  Function label   ||  None       ||  Number of arguments  ||  Number of local vars||
// ||  Conditional jump  ||  Frog       ||  None                ||  Local label      ||  None       ||  Tmp or Num           ||  None                ||
// ||  Assignment        ||  Gnoming    ||  Tmp or Var or Arg   ||  None             ||  None       ||  Tmp or Var or Arg    ||  None                ||
// ||  Operation         ||  Digging    ||  Tmp                 ||  None             ||  Operation  ||  Tmp                  ||  Tmp                 ||
// ||  Label             ||  Viperr     ||  None                ||  Local label      ||  None       ||  None                 ||  None                ||
// ||  Return            ||  Cherepovec ||  Tmp                 ||  None             ||  None       ||  None                 ||  None                ||
// ||  Call sys function ||  Boob       ||  Tmp                 ||  Sys func name    ||  None       ||  None                 ||  None                ||
// ||  Global vars num   ||  Gg         ||  None                ||  None             ||  None       ||  Number of arguments  ||  None                ||
////-----------------------------------------------------------------------------------------------------------------------------------------------////
////                                                        End of PYAM_IR table (EOPYAMIRT)                                                       ////

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

const char* ir_op_type_to_str(const enum IrOpType type);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif
