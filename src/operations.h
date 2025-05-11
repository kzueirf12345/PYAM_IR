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

enum IrOpBlockType
{
    IR_OP_BLOCK_TYPE_CALL_FUNCTION = 1     , // Call function                                       :
    IR_OP_BLOCK_TYPE_FUNCTION_BODY         , // Function body description (serves as label as well) :
    IR_OP_BLOCK_TYPE_COND_JUMP             , // Conditional jump                                    :
    IR_OP_BLOCK_TYPE_ASSIGNMENT            , // Assignment                                          :
    IP_OP_BLOCK_TYPE_OPERATION             , // Operation                                           :
    IR_OP_BLOCK_TYPE_RETURN                , // Return                                              :
    IR_OP_BLOCK_TYPE_LABEL                 , // Label                                               :
    IR_OP_BLOCK_TYPE_SYSCALL               , // System function call                                :
};

////                                               PYAM_IR table (PYAMIRT) for better understanding                                                ////
////-----------------------------------------------------------------------------------------------------------------------------------------------////
// ||    Default name    ||  PYAM name  ||     Return value     ||       Label       ||  Operation  ||     First Operand     ||    Second Operand    ||
// ||  Call function     ||  RingRing   ||  Tmp                 ||  Function label   ||  None       ||  None                 ||  None                ||
// ||  Function body     ||  Gyat       ||  None                ||  Function label   ||  None       ||  Number of arguments  ||  None                ||
// ||  Conditional jump  ||  Frog       ||  None                ||  Local label      ||  None       ||  Tmp                  ||  None                ||
// ||  Assignment        ||  Gnoming    ||  Tmp or var          ||  None             ||  None       ||  Var or Tmp           ||  Var or Tmp or Num   ||
// ||  Operation         ||  Digging    ||  Tmp or var          ||  None             ||  Operation  ||  Var or Tmp or Num    ||  Var or Tmp or Num   ||
// ||  Label             ||  Viperr     ||  None                ||  Local label      ||  None       ||  None                 ||  None                ||
// ||  Return            ||  Cherepovec ||  Tmp or var or Num   ||  None             ||  None       ||  None                 ||  None                ||    
// || Call sys function  ||  Boob       ||  None                ||  Sys func name    ||  None       ||  None                 ||  None                ||
////-----------------------------------------------------------------------------------------------------------------------------------------------////
////                                                        End of PYAM_IR table (EOPYAMIRT)                                                       ////


const char* ir_op_type_to_str(const enum IrOpType type);


#endif 
