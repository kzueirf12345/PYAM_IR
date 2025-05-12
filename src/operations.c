#include "operations.h"
#include "../libpyam_ir.h"

#define CASE_ENUM_TO_STRING_(op_type_) case op_type_: return #op_type_
const char* ir_op_type_to_str(const enum IrOpType type)
{
    switch(type)
    {
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_SUM);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_SUB);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_MUL);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_DIV);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_POW);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_EQ);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_NEQ);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_LESS);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_LESSEQ);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_GREAT);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_GREATEQ);
        CASE_ENUM_TO_STRING_(IR_OP_TYPE_INVALID_OPERATION);
        default:
            return "UNKNOWN_IR_OP_TYPE";
    }
    return "UNKNOWN_IR_OP_TYPE";
}
#undef CASE_ENUM_TO_STRING_
