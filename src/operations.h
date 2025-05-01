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
    IR_OPERAND_TYPE_OPERATION   = 0b1000000
};

enum IrOpType
{
    IR_OP_TYPE_SUM              = 1,
    IR_OP_TYPE_SUB              = 2,
    IR_OP_TYPE_MUL              = 3,
    IR_OP_TYPE_DIV              = 4,
    IR_OP_TYPE_POW              = 5,
    IR_OP_TYPE_EQ               = 6,
    IR_OP_TYPE_NEQ              = 7,
    IR_OP_TYPE_LESS             = 8,
    IR_OP_TYPE_LESSEQ           = 9,
    IR_OP_TYPE_GREAT            = 10,
    IR_OP_TYPE_GREATEQ          = 11,
    IR_OP_TYPE_DECL_ASSIGNMENT  = 12,
    IR_OP_TYPE_ASSIGNMENT       = 13,
    IR_OP_TYPE_SUM_ASSIGNMENT   = 14,
    IR_OP_TYPE_SUB_ASSIGNMENT   = 15,
    IR_OP_TYPE_MUL_ASSIGNMENT   = 16,
    IR_OP_TYPE_DIV_ASSIGNMENT   = 17,
    IR_OP_TYPE_POW_ASSIGNMENT   = 18,
};

const char* ir_op_type_to_str(const enum IrOpType type);

#define IR_OPERATION_HANDLE(num_, name_, surname_, assign_, label_, op_, oper1_, oper2_, ...) \
        IR_OP_BLOCK_TYPE_##surname_ = num_,

enum IrOpBlockType
{
#include "./codegen.h"
};

#undef IR_OPERATION_HANDLE

#endif /*PYAM_IR_SRC_OPERATIONS_H*/
