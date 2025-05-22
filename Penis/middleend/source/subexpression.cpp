#include "subexpression.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>

#include "middleend.h"
#include "read_ir.h"

#include "../../../include/key_words.h"

#include "list.h"
#include "list_construction.h"
#include "list_push.h"
#include "list_src.h"

#include "Assert/my_assert.h"
#include "Logger/logging.h"
#include "helpful.h"

static enum IRError GetGlobalVarsNum    (const list_t* const IR_list, size_t* const cnt_global_vars,
                                         size_t* const instruction_index);

static enum IRError FillGLobalVarsTable (list_t* const IR_list, SSATable_t* const table,
                                         size_t* const instruction_index, size_t* const cnt_global_vars);

static enum IRError FindAndDeleteSimilarExpressionsGlobal (list_t* const IR_list, list_t* const operation_list,
                                                           size_t* const instruction_index, size_t* const cnt_global_vars);

static enum IRError FindSimilarExpressionsInFunc (list_t* const IR_list, SSATable_t* const global_table,
                                                  size_t* const instruction_index);

static enum IRError GetLocalVarsNum    (const list_t* const IR_list, size_t* const cnt_global_vars,
                                         size_t* const instruction_index);

static enum IRError GetTmpVarsNum      (const list_t* const IR_list, size_t* const cnt_tmp_vars,
                                        size_t* const instruction_index);

static enum IRError FindSimilarExpressionLocal (list_t* const IR_list,
                                                SSATable_t* const local_table,
                                                TMPValueTable_t* const tmp_table,
                                                SSATable_t* const global_table,
                                                list_t* const operation_list,
                                                size_t* const instruction_index);

static size_t FindVarInTable (const SSATable_t* const table, const size_t var_index);

static size_t FindTmpInTable (const TMPValueTable_t* const table, const size_t tmp_index);

enum IRError FindSimilarExpression (list_t* const IR_list)
{
    ASSERT (IR_list != NULL, "Invalid argument IR_list\n");

    LOG (kDebug, "Starting looking for similar expressions\n");

    size_t instruction_index = TailIndex (IR_list);

    size_t cnt_global_vars = 0;
    enum IRError result = GetGlobalVarsNum (IR_list, &cnt_global_vars, &instruction_index);
    if (result != kDoneIR)
    {
        return result;
    }

    SSATable_t global_table = {};
    global_table.counter = 0;

    global_table.table = (SSA_t*) calloc (cnt_global_vars, sizeof (SSA_t));
    if (global_table.table == NULL)
    {
        return kCantCreateGlobalVarsTable;
    }

    result = FillGLobalVarsTable (IR_list, &global_table, &instruction_index, &cnt_global_vars);
    if (result != kDoneIR)
    {
        FREE_AND_NULL (global_table.table);
        return result;
    }

    LOG (kDebug, "Got Global vars\n");

    result = FindSimilarExpressionsInFunc (IR_list, &global_table, &instruction_index);
    if (result != kDoneIR)
    {
        FREE_AND_NULL (global_table.table);
        return result;
    }

    LOG (kDebug, "Deal with local vars\n");

    FREE_AND_NULL (global_table.table);

    return result;
}

static enum IRError GetGlobalVarsNum (const list_t* const IR_list, size_t* const cnt_global_vars,
                                      size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (cnt_global_vars   != NULL, "Invalid argument cnt_global_vars\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    if (*instruction_index == 0)
    {
        return kEmptyProgramIR;
    }

    IRInstruction_t instruction = {};
    ListElemValLoad (IR_list, *instruction_index, &instruction);

    if (instruction.type != IR_GLOBAL_VARS_NUM_INDEX)
    {
        return kInvalidOrderListIR;
    }

    *cnt_global_vars = instruction.value_1.number_size_t;

    *instruction_index = NextIndex (IR_list, *instruction_index);

    return kDoneIR;
}

static enum IRError FillGLobalVarsTable (list_t* const IR_list, SSATable_t* const table,
                                         size_t* const instruction_index, size_t* const cnt_global_vars)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (table             != NULL, "Invalid argument table\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");
    ASSERT (cnt_global_vars   != NULL, "Invalid argument cnt_global_vars\n");

    IRInstruction_t instruction = {};
    ListElemValLoad (IR_list, *instruction_index, &instruction);

    size_t cnt_tmp_vars = 0;
    enum IRError result = GetTmpVarsNum (IR_list, &cnt_tmp_vars, instruction_index);
    if (result != kDoneIR)
    {
        return result;
    }

    LOG (kDebug, "Number of tmp in global = %lu\n", cnt_tmp_vars);

    TMPValueTable_t tmp_table = {};
    tmp_table.counter = 0;

    tmp_table.table = (TMPValue_t*) calloc (cnt_tmp_vars, sizeof (TMPValue_t));
    if (tmp_table.table == NULL)
    {
        return kCantCreateLocalVarsTable;
    }

    list_t operation_list = {};
    if (ListCtor (&operation_list, kStartNumberOfListElem, sizeof(OperationInfo_t)) != kDoneList)
    {
        FREE_AND_NULL (tmp_table.table);
        return kCantCreateListIR;
    }

    while ((instruction.type != IR_FUNCTION_BODY_INDEX) && (*instruction_index != 0))
    {
        if (instruction.type == IR_ASSIGNMENT_INDEX)
        {
            LOG (kDebug, "Assigning was identified\n");

            switch (instruction.res_type)
            {
                case kGlobalVar:
                case kVar:
                {
                    size_t index = FindVarInTable (table, instruction.value_1.operand_1_index);
                    if (index == ULLONG_MAX)
                    {
                        table->table [table->counter] = {.var_index = instruction.res_index,
                                                         .assign_counter = 1};
                        table->counter++;
                    }
                    else
                    {
                        table->table [index].assign_counter++;
                    }
                    break;
                }
                case kTmp:
                {
                    tmp_table.table [tmp_table.counter] = {};
                    tmp_table.table [tmp_table.counter].tmp_index = instruction.res_index;
                    tmp_table.table [tmp_table.counter].type = instruction.type_1;
                    tmp_table.counter++;
                    if ((instruction.type_1 == kVar) || (instruction.type_1 == kGlobalVar))
                    {
                        size_t index = FindVarInTable (table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }

                        tmp_table.table [tmp_table.counter - 1].value.variable = table->table [index];
                    }
                    else
                    {
                        tmp_table.table [tmp_table.counter - 1].value.number = instruction.value_1.number_double;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else if (instruction.type == IR_OPERATION_INDEX)
        {
            LOG (kDebug, "Operation was identified\n");

            OperationInfo_t operation = {};
            operation.instruction_index = *instruction_index;
            operation.instruction = instruction;

            tmp_table.table [tmp_table.counter] = {};
            tmp_table.table [tmp_table.counter].type = kNone;

            tmp_table.counter++;
            if (instruction.type_1 == kTmp)
            {
                size_t index = FindTmpInTable (&tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                operation.op_1 = tmp_table.table [index];
            }
            if (instruction.type_2 == kTmp)
            {
                size_t index = FindTmpInTable (&tmp_table, instruction.value_2.operand_2_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                operation.op_2 = tmp_table.table [index];
            }

            ListPushFront (&operation_list, &operation);
        }
        else if (instruction.type == IR_SYSTEM_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Syscall was identified\n");

            tmp_table.table [tmp_table.counter] = {};
            tmp_table.table [tmp_table.counter].type = kNone;

            tmp_table.counter++;
        }
        else if (instruction.type == IR_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Function call was identified\n");

            tmp_table.table [tmp_table.counter] = {};
            tmp_table.table [tmp_table.counter].type = kNone;

            tmp_table.counter++;
        }

        *instruction_index = NextIndex (IR_list, *instruction_index);
        ListElemValLoad (IR_list, *instruction_index, &instruction);
    }

    FindAndDeleteSimilarExpressionsGlobal (IR_list, &operation_list, instruction_index, cnt_global_vars);

    ListElemValLoad (IR_list, TailIndex (IR_list), &instruction);
    instruction.value_1.number_size_t = *cnt_global_vars;
    ListElemValStor (IR_list, TailIndex (IR_list), &instruction);

    FREE_AND_NULL (tmp_table.table);

    return kDoneIR;
}

static enum IRError FindAndDeleteSimilarExpressionsGlobal (list_t* const IR_list, list_t* const operation_list,
                                                           size_t* const instruction_index, size_t* const cnt_global_vars)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (operation_list    != NULL, "Invalid argument operation_list\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");
    ASSERT (cnt_global_vars   != NULL, "Invalid argument cnt_global_vars\n");

    size_t op_index = TailIndex (operation_list);

    OperationInfo_t operation = {};
    OperationInfo_t cmp_operation = {};

    size_t max_tmp_index = 0;
    GetTmpVarsNum(IR_list, &max_tmp_index, instruction_index);

    while (op_index != 0)
    {
        ListElemValLoad (operation_list, op_index, &operation);
        size_t cmp_op_index = NextIndex (operation_list, op_index);

        while (cmp_op_index != 0)
        {
            ListElemValLoad (operation_list, cmp_op_index, &cmp_operation);

            if ((operation.instruction.operation == cmp_operation.instruction.operation)
                && (operation.op_1.type == cmp_operation.op_1.type)
                && (operation.op_1.type != kNone)
                && (((operation.op_1.value.number - cmp_operation.op_1.value.number < kEpsilon)
                        && (operation.op_1.type == kNum))
                    || ((operation.op_1.value.variable.var_index == cmp_operation.op_1.value.variable.var_index)
                        && (operation.op_1.value.variable.assign_counter == cmp_operation.op_1.value.variable.assign_counter)
                        && (operation.op_1.type != kNum)))
                && (((operation.op_2.value.number - cmp_operation.op_2.value.number < kEpsilon)
                        && (operation.op_2.type == kNum))
                    || ((operation.op_2.value.variable.var_index == cmp_operation.op_2.value.variable.var_index)
                        && (operation.op_2.value.variable.assign_counter == cmp_operation.op_2.value.variable.assign_counter)
                        && (operation.op_2.type != kNum))))
            {
                IRInstruction_t instruction = {};

                instruction.type = IR_ASSIGNMENT_INDEX;
                instruction.res_type = kVar;
                instruction.res_index = *cnt_global_vars;
                instruction.type_1 = kTmp;
                instruction.value_1.operand_1_index = max_tmp_index;
                ListPushAfterIndex (IR_list, &instruction, operation.instruction_index);

                instruction.res_type = kTmp;
                instruction.res_index = operation.instruction.res_index;
                instruction.type_1 = kVar;
                instruction.value_1.operand_1_index = *cnt_global_vars;
                ListPushAfterIndex (IR_list, &instruction, NextIndex (IR_list, operation.instruction_index));

                instruction = operation.instruction;
                instruction.res_index = max_tmp_index;
                ListElemValStor (IR_list, operation.instruction_index, &instruction);

                instruction.res_type = kTmp;
                instruction.res_index = cmp_operation.instruction.res_index;
                instruction.type_1 = kVar;
                instruction.value_1.operand_1_index = *cnt_global_vars;
                ListElemValStor (IR_list, cmp_operation.instruction_index, &instruction);

                (*cnt_global_vars)++;
                max_tmp_index++;
            }
        }
    }

    return kDoneIR;
}

static size_t FindVarInTable (const SSATable_t* const table, const size_t var_index)
{
    ASSERT (table != NULL, "Invalid argument table\n");

    for (size_t index = 0; index < table->counter; index++)
    {
        if (table->table [index].var_index == var_index)
        {
            return index;
        }
    }

    return ULLONG_MAX;
}

static size_t FindTmpInTable (const TMPValueTable_t* const table, const size_t tmp_index)
{
    ASSERT (table != NULL, "Invalid argument table\n");

    for (size_t index = 0; index < table->counter; index++)
    {
        if (table->table [index].tmp_index == tmp_index)
        {
            return index;
        }
    }

    return ULLONG_MAX;
}

static enum IRError FindSimilarExpressionsInFunc (list_t* const IR_list, SSATable_t* const global_table,
                                                  size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (global_table      != NULL, "Invalid argument global_table\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    while (*instruction_index != 0)
    {
        LOG (kDebug, "I am in function\n");

        size_t cnt_loc_vars = 0;
        size_t func_body_index = *instruction_index;
        enum IRError result = GetLocalVarsNum (IR_list, &cnt_loc_vars, instruction_index);
        if (result != kDoneIR)
        {
            return result;
        }

        SSATable_t local_table = {};
        local_table.counter = 0;

        local_table.table = (SSA_t*) calloc (cnt_loc_vars, sizeof (SSA_t));
        if (local_table.table == NULL)
        {
            return kCantCreateLocalVarsTable;
        }

        size_t cnt_tmp_vars = 0;
        result = GetTmpVarsNum (IR_list, &cnt_tmp_vars, instruction_index);
        if (result != kDoneIR)
        {
            FREE_AND_NULL (local_table.table);
            return result;
        }

        LOG (kDebug, "Tmp in function = %lu\n", cnt_tmp_vars);

        TMPValueTable_t tmp_table = {};
        tmp_table.counter = 0;

        tmp_table.table = (TMPValue_t*) calloc (cnt_tmp_vars, sizeof (TMPValue_t));
        if (tmp_table.table == NULL)
        {
            FREE_AND_NULL (local_table.table);
            return kCantCreateLocalVarsTable;
        }

        list_t operation_list = {};
        if (ListCtor (&operation_list, kStartListNumberElemForOperations, sizeof(OperationInfo_t)) != kDoneList)
        {
            FREE_AND_NULL (local_table.table);
            FREE_AND_NULL (tmp_table.table);
            return kCantCreateListIR;
        }

        result = FindSimilarExpressionLocal (IR_list, &local_table, &tmp_table, global_table, &operation_list, instruction_index);
        if (result != kDoneIR)
        {
            FREE_AND_NULL (tmp_table.table);
            FREE_AND_NULL (local_table.table);
            ListDtor (&operation_list);
            return result;
        }

        LOG (kDebug, "Filled tables\n");

        FindAndDeleteSimilarExpressionsGlobal (IR_list, &operation_list, instruction_index, &cnt_loc_vars);

        IRInstruction_t instruction = {};
        ListElemValLoad (IR_list, func_body_index, &instruction);
        instruction.value_2.number_size_t = cnt_loc_vars;
        ListElemValStor (IR_list, func_body_index, &instruction);

        LOG (kDebug, "Killed additional expressions\n");

        FREE_AND_NULL (tmp_table.table);
        FREE_AND_NULL (local_table.table);
    }

    return kDoneIR;
}

static enum IRError GetLocalVarsNum (const list_t* const IR_list, size_t* const cnt_loc_vars,
                                     size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (cnt_loc_vars      != NULL, "Invalid argument cnt_loc_vars\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    IRInstruction_t instruction = {};
    ListElemValLoad (IR_list, *instruction_index, &instruction);

    if (instruction.type != IR_FUNCTION_BODY_INDEX)
    {
        return kInvalidOrderListIR;
    }

    *cnt_loc_vars = instruction.value_2.number_size_t;

    *instruction_index = NextIndex (IR_list, *instruction_index);

    return kDoneIR;
}

static enum IRError GetTmpVarsNum (const list_t* const IR_list, size_t* const cnt_tmp_vars,
                                   size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (cnt_tmp_vars      != NULL, "Invalid argument cnt_tmp_vars\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    size_t max_tmp_counter = 0;

    IRInstruction_t instruction = {};

    size_t index = *instruction_index;

    ListElemValLoad (IR_list, index, &instruction);
    while ((index != 0) && (instruction.type != IR_FUNCTION_BODY_INDEX))
    {
        if ((instruction.res_type == kTmp) && (instruction.res_index > max_tmp_counter))
        {
            max_tmp_counter = instruction.res_index;
        }
        if ((instruction.type_1 == kTmp) && (instruction.value_1.operand_1_index > max_tmp_counter))
        {
            max_tmp_counter = instruction.value_1.operand_1_index;
        }
        if ((instruction.type_2 == kTmp) && (instruction.value_2.operand_2_index > max_tmp_counter))
        {
            max_tmp_counter = instruction.value_2.operand_2_index;
        }

        index = NextIndex (IR_list, index);
        ListElemValLoad (IR_list, index, &instruction);
    }

    *cnt_tmp_vars = max_tmp_counter + 1;
    return kDoneIR;
}

static enum IRError FindSimilarExpressionLocal (list_t* const IR_list,
                                                SSATable_t* const local_table,
                                                TMPValueTable_t* const tmp_table,
                                                SSATable_t* const global_table,
                                                list_t* const operation_list,
                                                size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (local_table       != NULL, "Invalid argument local_table\n");
    ASSERT (tmp_table         != NULL, "Invalid argument tmp_table\n");
    ASSERT (global_table      != NULL, "Invalid argument global_table\n");
    ASSERT (operation_list    != NULL, "Invalid argument operation_list\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    IRInstruction_t instruction = {};
    ListElemValLoad (IR_list, *instruction_index, &instruction);

    while ((*instruction_index != 0) && (instruction.type != IR_FUNCTION_BODY_INDEX))
    {
        if (instruction.type == IR_ASSIGNMENT_INDEX)
        {
            LOG (kDebug, "Assigning was identified\n");

            switch (instruction.res_type)
            {
                case kGlobalVar:
                {
                    size_t index = FindVarInTable (global_table, instruction.value_1.operand_1_index);
                    if (index == ULLONG_MAX)
                    {
                        return kInvalidOrderListIR;
                    }
                    else
                    {
                        global_table->table [index].assign_counter++;
                    }
                    break;
                }
                case kVar:
                {
                    size_t index = FindVarInTable (local_table, instruction.value_1.operand_1_index);
                    if (index == ULLONG_MAX)
                    {
                        local_table->table [local_table->counter] = {.var_index = instruction.res_index,
                                                                     .assign_counter = 1};
                        local_table->counter++;
                    }
                    else
                    {
                        local_table->table [index].assign_counter++;
                    }
                    break;
                }
                case kTmp:
                {
                    tmp_table->table [tmp_table->counter] = {};
                    tmp_table->table [tmp_table->counter].tmp_index = instruction.res_index;
                    tmp_table->table [tmp_table->counter].type = instruction.type_1;
                    tmp_table->counter++;
                    if (instruction.type_1 == kVar)
                    {
                        size_t index = FindVarInTable (local_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }

                        tmp_table->table [tmp_table->counter - 1].value.variable = local_table->table [index];
                    }
                    else if (instruction.type_1 == kGlobalVar)
                    {
                        size_t index = FindVarInTable (global_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }

                        tmp_table->table [tmp_table->counter - 1].value.variable = global_table->table [index];
                    }
                    else
                    {
                        tmp_table->table [tmp_table->counter - 1].value.number = instruction.value_1.number_double;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else if (instruction.type == IR_OPERATION_INDEX)
        {
            LOG (kDebug, "Operation was identified\n");

            OperationInfo_t operation = {};
            operation.instruction_index = *instruction_index;
            operation.instruction = instruction;

            tmp_table->table [tmp_table->counter] = {};
            tmp_table->table [tmp_table->counter].type = kNone;

            tmp_table->counter++;
            if (instruction.type_1 == kTmp)
            {
                size_t index = FindTmpInTable (tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                operation.op_1 = tmp_table->table [index];
            }
            if (instruction.type_2 == kTmp)
            {
                size_t index = FindTmpInTable (tmp_table, instruction.value_2.operand_2_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                operation.op_2 = tmp_table->table [index];
            }

            ListPushFront (operation_list, &operation);
        }
        else if (instruction.type == IR_SYSTEM_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Syscall was identified\n");

            tmp_table->table [tmp_table->counter] = {};
            tmp_table->table [tmp_table->counter].type = kNone;

            tmp_table->counter++;
        }
        else if (instruction.type == IR_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Function call was identified\n");

            tmp_table->table [tmp_table->counter] = {};
            tmp_table->table [tmp_table->counter].type = kNone;

            tmp_table->counter++;
        }

        *instruction_index = NextIndex (IR_list, *instruction_index);
        ListElemValLoad (IR_list, *instruction_index, &instruction);
    }

    return kDoneIR;
}
