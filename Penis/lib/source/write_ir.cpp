#include "write_ir.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "read_ir.h"

#include "../../../include/key_words.h"
#include "../../../include/operations.h"

#include "list.h"
#include "list_src.h"

#include "Assert/my_assert.h"
#include "Logger/logging.h"

static enum IRError CallFunc      (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError FuncBody      (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError CondJump      (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError Assign        (const IRInstruction_t* const instruction, FILE* const output_file,
                                   const size_t cnt_global_vars);

static enum IRError AssignVar     (const IRInstruction_t* const instruction, FILE* const output_file,
                                   const size_t cnt_global_vars);
static enum IRError AssignTmp     (const IRInstruction_t* const instruction, FILE* const output_file,
                                   const size_t cnt_global_vars);
static enum IRError AssignArg     (const IRInstruction_t* const instruction, FILE* const output_file);

static enum IRError Operation     (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError Label         (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError Return        (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError SysCall       (const IRInstruction_t* const instruction, FILE* const output_file);
static enum IRError GlobalVarsNum (const IRInstruction_t* const instruction, FILE* const output_file,
                                   size_t* const cnt_global_vars);

static size_t IndexSysCall (const char* const syscall_name);

enum IRError WriteIRPYAM (const list_t* const list, FILE* const output_file)
{
    ASSERT (list        != NULL, "Invalid argument list\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    enum IRError result = kDoneIR;

    IRInstruction_t instruction = {};
    size_t index = TailIndex (list);

    size_t cnt_global_vars = 0;

    while (index != 0)
    {
        ListElemValLoad (list, index, &instruction);
        switch (instruction.type)
        {
            case IR_FUNCTION_CALL_INDEX:
            {
                result = CallFunc (&instruction, output_file);
                break;
            }
            case IR_FUNCTION_BODY_INDEX:
            {
                result = FuncBody (&instruction, output_file);
                break;
            }
            case IR_CONDITIONAL_JUMP_INDEX:
            {
                result = CondJump (&instruction, output_file);
                break;
            }
            case IR_ASSIGNMENT_INDEX:
            {
                result = Assign (&instruction, output_file, cnt_global_vars);
                break;
            }
            case IR_LABEL_INDEX:
            {
                result = Label (&instruction, output_file);
                break;
            }
            case IR_SYSTEM_FUNCTION_CALL_INDEX:
            {
                result = SysCall (&instruction, output_file);
                break;
            }
            case IR_OPERATION_INDEX:
            {
                result = Operation (&instruction, output_file);
                break;
            }
            case IR_RETURN_INDEX:
            {
                result = Return (&instruction, output_file);
                break;
            }
            case IR_GLOBAL_VARS_NUM_INDEX:
            {
                result = GlobalVarsNum (&instruction, output_file, &cnt_global_vars);
                break;
            }
            case IR_INVALID_KEY_WORD:
            default:
            {
                return kInvalidKeyWordIR;
            }
            if (result != kDoneIR)
            {
                return result;
            }
        }
        index = NextIndex (list, index);
    }

    return kDoneIR;
}

static enum IRError CallFunc (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "CallFunc started analyzing\n");

    LOG (kDebug, "Calling function is \"%s\"\n", instruction->label);

    fprintf(
        output_file,
        "\n%s(tmp%zu, %s)\t # Calling function\n\n",
        kIR_KEY_WORD_ARRAY [IR_FUNCTION_CALL_INDEX],
        instruction->res_index,
        instruction->label
    );

    return kDoneIR;
}

static enum IRError FuncBody (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "FuncBody started analyzing\n");

    LOG (kDebug, "Start function \"%s\" body\n", instruction->label);

    fprintf(
        output_file,
        "\n%s(%s, %zu, %zu)\t # Function body\n",
        kIR_KEY_WORD_ARRAY [IR_FUNCTION_BODY_INDEX],
        instruction->label,
        instruction->value_1.number_size_t,
        instruction->value_2.number_size_t
    );

    return kDoneIR;
}

static enum IRError CondJump (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "CondJump started analyzing\n");

    if (instruction->type_1 != kNum)
    {
        fprintf(
            output_file,
            "%s(%s, tmp%zu)\t # Conditional jump\n",
            kIR_KEY_WORD_ARRAY [IR_CONDITIONAL_JUMP_INDEX],
            instruction->label,
            instruction->value_1.operand_1_index
        );
    }
    else
    {
        fprintf(
            output_file,
            "%s(%s, 1)\t # Jump\n",
            kIR_KEY_WORD_ARRAY [IR_CONDITIONAL_JUMP_INDEX],
            instruction->label
        );
    }

    return kDoneIR;
}

static enum IRError Assign (const IRInstruction_t* const instruction, FILE* const output_file,
                            const size_t cnt_global_vars)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Assigning started analyzing\n");

    switch (instruction->res_type)
    {
        case kGlobalVar:
        case kVar: return AssignVar (instruction, output_file, cnt_global_vars);
        case kTmp: return AssignTmp (instruction, output_file, cnt_global_vars);
        case kArg: return AssignArg (instruction, output_file);
        default:   return kInvalidPrefixIR;
    }
}

static enum IRError AssignVar (const IRInstruction_t* const instruction, FILE* const output_file,
                               const size_t cnt_global_vars)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Assigning to var started analyzing\n");

    switch (instruction->type_1)
    {
        case kTmp:
        {
            fprintf(
                output_file,
                "\t%s(var%lld, tmp%zu)\t # Variable\n",
                kIR_KEY_WORD_ARRAY [IR_ASSIGNMENT_INDEX],
                (instruction->res_type == kVar)
                                ? (long long) instruction->res_index
                                : - (long long) (cnt_global_vars - instruction->res_index),
                instruction->value_1.operand_1_index
            );

            return kDoneIR;
        }
        case kArg:
        {
            fprintf(
                output_file,
                "\t%s(var%lld, arg%zu)\t # Variable\n",
                kIR_KEY_WORD_ARRAY [IR_ASSIGNMENT_INDEX],
                (instruction->res_type == kVar)
                                ? (long long) instruction->res_index
                                : - (long long) (cnt_global_vars - instruction->res_index),
                instruction->value_1.operand_1_index
            );

            return kDoneIR;
        }
        default:
            return kInvalidPrefixIR;
    }

}

static enum IRError AssignTmp (const IRInstruction_t* const instruction, FILE* const output_file,
                               const size_t cnt_global_vars)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Assigning to tmp started analyzing\n");

    switch (instruction->type_1)
    {
        case kNum:
        {
            fprintf(
                output_file,
                "\t%s(tmp%zu, %lg )\t # Assigning Num to Tmp\n",
                kIR_KEY_WORD_ARRAY [IR_ASSIGNMENT_INDEX],
                instruction->res_index,
                instruction->value_1.number_double                                                                                    \
            );

            return kDoneIR;
        }
        case kGlobalVar:
        case kVar:
        {
            fprintf(
                output_file,
                "\t%s(tmp%zu, var%lld)\t # Assigning Var to Tmp\n",
                kIR_KEY_WORD_ARRAY [IR_ASSIGNMENT_INDEX],
                instruction->res_index,
                (instruction->type_1 == kVar)
                                ? (long long) instruction->value_1.operand_1_index
                                : - (long long) (cnt_global_vars - instruction->value_1.operand_1_index)
            );

            return kDoneIR;
        }
        default:
            return kInvalidAssigningIR;
    }
}

static enum IRError AssignArg (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Assigning to arg started analyzing\n");

    fprintf(
            output_file,
            "\t%s(arg%zu, tmp%zu)\t # Assigning Tmp to Arg\n",
            kIR_KEY_WORD_ARRAY [IR_ASSIGNMENT_INDEX],
            instruction->res_index,
            instruction->value_1.operand_1_index
    );

    return kDoneIR;
}

static enum IRError Operation (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Operation started analyzing\n");

    fprintf(
            output_file,
            "\t%s(tmp%zu, %d, tmp%zu, tmp%zu)\t # Operation\n\n",
            kIR_KEY_WORD_ARRAY [IR_OPERATION_INDEX],
            instruction->res_index,
            instruction->operation,
            instruction->value_1.operand_1_index,
            instruction->value_2.operand_2_index
    );

    return kDoneIR;
}

static enum IRError Label (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Label started analyzing\n");

    LOG (kDebug, "Label \"%s\" was analyzed\n", instruction->label);

    fprintf(
            output_file,
            "\n%s(%s)\t # Label\n",
            kIR_KEY_WORD_ARRAY [IR_LABEL_INDEX],
            instruction->label
    );

    return kDoneIR;
}

static enum IRError Return (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "Return started analyzing\n");

    fprintf(
            output_file,
            "\n\t%s(tmp%zu)\t # Return with ret val in tmp%zu\n",
            kIR_KEY_WORD_ARRAY [IR_RETURN_INDEX],
            instruction->res_index,
            instruction->res_index
    );

    return kDoneIR;
}

static enum IRError SysCall (const IRInstruction_t* const instruction, FILE* const output_file)
{
    ASSERT (instruction != NULL, "Invalid argument instruction\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    LOG (kDebug, "SysCall started analyzing\n");

    fprintf(
            output_file,
            "\n\t%s(tmp%zu, %s, %d)\t # System function call: \"%s\"\n",
            kIR_KEY_WORD_ARRAY [IR_SYSTEM_FUNCTION_CALL_INDEX],
            instruction->res_index,
            instruction->label,
            kIR_SYS_CALL_ARRAY [IndexSysCall (instruction->label)].NumberOfArguments,
            instruction->label
    );

    return kDoneIR;
}

static size_t IndexSysCall (const char* const syscall_name)
{
    ASSERT(syscall_name != NULL, "Invalid argument syscall_name\n");

    for (size_t syscall_index = 0; syscall_index < kIR_SYS_CALL_NUMBER; syscall_index++)
    {
        if (strcmp (kIR_SYS_CALL_ARRAY [syscall_index].Name, syscall_name) == 0)
        {
            return syscall_index;
        }
    }
    return ULLONG_MAX;
}

static enum IRError GlobalVarsNum  (const IRInstruction_t* const instruction, FILE* const output_file,
                                   size_t* const cnt_global_vars)
{
    ASSERT (instruction     != NULL, "Invalid argument instruction\n");
    ASSERT (output_file     != NULL, "Invalid argument output_file\n");
    ASSERT (cnt_global_vars != NULL, "Invalid argument cnt_global_vars\n");

    LOG (kDebug, "GlobalVarsNum started analyzing\n");

    fprintf(
            output_file,
            "%s(%lu)\t # Number of global vars\n",
            kIR_KEY_WORD_ARRAY[IR_GLOBAL_VARS_NUM_INDEX],
            instruction->value_1.number_size_t
        );

    *cnt_global_vars = instruction->value_1.number_size_t;

    return kDoneIR;
}
