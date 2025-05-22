#include "unused_vars.h"

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
#include "list_pop.h"
#include "list_src.h"

#include "Assert/my_assert.h"
#include "Logger/logging.h"
#include "helpful.h"

static enum IRError GetGlobalVarsNum    (const list_t* const IR_list, size_t* const cnt_global_vars,
                                         size_t* const instruction_index);
static enum IRError FillGLobalVarsTable (list_t* const IR_list, VarUsageTable_t* const table,
                                         size_t* const instruction_index);

static enum IRError FillLocalVarsTablesAndKillVars (list_t* const IR_list, VarUsageTable_t* const global_table,
                                                    size_t* const instruction_index);

static enum IRError GetLocalVarsNum    (const list_t* const IR_list, size_t* const cnt_global_vars,
                                         size_t* const instruction_index);

static enum IRError GetTmpVarsNum      (const list_t* const IR_list, size_t* const cnt_tmp_vars,
                                        size_t* const instruction_index);

static enum IRError FillLocalVarsTable (list_t* const IR_list,
                                        VarUsageTable_t* const local_table,
                                        VarUsageTable_t* const tmp_table,
                                        VarUsageTable_t* const global_table,
                                        size_t* const instruction_index);

static enum IRError KillUnusedLocalVars (list_t* const IR_list, VarUsageTable_t* const local_table);

static size_t FindVarInTable (const VarUsageTable_t* const table, const size_t var_index);

enum IRError TableDtor (VarUsageTable_t* const table);

enum IRError KillUnusedVars (list_t* const IR_list)
{
    ASSERT (IR_list != NULL, "Invalid argument IR_list\n");

    LOG (kDebug, "Starting killing vars (he he he)\n");

    size_t instruction_index = TailIndex (IR_list);

    size_t cnt_global_vars = 0;
    enum IRError result = GetGlobalVarsNum (IR_list, &cnt_global_vars, &instruction_index);
    if (result != kDoneIR)
    {
        return result;
    }

    VarUsageTable_t global_table = {};
    global_table.counter = 0;

    global_table.table = (VarUsage_t*) calloc (cnt_global_vars, sizeof (VarUsage_t));
    if (global_table.table == NULL)
    {
        return kCantCreateGlobalVarsTable;
    }

    result = FillGLobalVarsTable (IR_list, &global_table, &instruction_index);
    if (result != kDoneIR)
    {
        TableDtor (&global_table);
        FREE_AND_NULL (global_table.table);
        return result;
    }

    LOG (kDebug, "Got Global vars\n");

    result = FillLocalVarsTablesAndKillVars (IR_list, &global_table, &instruction_index);
    if (result != kDoneIR)
    {
        TableDtor (&global_table);
        FREE_AND_NULL (global_table.table);
        return result;
    }

    LOG (kDebug, "Deal with local vars\n");

    result = KillUnusedLocalVars (IR_list, &global_table);

    LOG (kDebug, "Killed Global vars\n");

    TableDtor (&global_table);
    FREE_AND_NULL (global_table.table);

    TryListResize (IR_list);

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

static enum IRError FillGLobalVarsTable (list_t* const IR_list, VarUsageTable_t* const table,
                                         size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (table             != NULL, "Invalid argument table\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    IRInstruction_t instruction = {};
    ListElemValLoad (IR_list, *instruction_index, &instruction);

    size_t cnt_tmp_vars = 0;
    enum IRError result = GetTmpVarsNum (IR_list, &cnt_tmp_vars, instruction_index);
    if (result != kDoneIR)
    {
        return result;
    }

    LOG (kDebug, "Number of tmp in global = %lu\n", cnt_tmp_vars);

    VarUsageTable_t tmp_table = {};
    tmp_table.counter = 0;

    tmp_table.table = (VarUsage_t*) calloc (cnt_tmp_vars, sizeof (VarUsage_t));
    if (tmp_table.table == NULL)
    {
        return kCantCreateLocalVarsTable;
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
                                                         .instruction_decl_index = {},
                                                         .usage_counter = 0};
                        table->counter++;
                        if (ListCtor (&(table->table [table->counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
                        {
                            return kCantCreateListIR;
                        }
                        ListPushFront (&(table->table [table->counter - 1].instruction_decl_index), instruction_index);
                    }
                    else
                    {
                        ListPushFront (&(table->table [index].instruction_decl_index), instruction_index);
                    }
                    if (instruction.type_1 == kTmp)
                    {
                        size_t index = FindVarInTable (&tmp_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        tmp_table.table [index].usage_counter++;
                    }
                    break;
                }
                case kTmp:
                {
                    tmp_table.table [tmp_table.counter] = {.var_index = instruction.res_index,
                                                           .instruction_decl_index = {},
                                                           .usage_counter = 0};
                    tmp_table.counter++;
                    if (ListCtor (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
                    {
                        return kCantCreateListIR;
                    }
                    ListPushFront (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), instruction_index);
                    if ((instruction.type_1 == kVar) || (instruction.type_1 == kGlobalVar))
                    {
                        size_t index = FindVarInTable (table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        table->table [index].usage_counter++;
                    }
                    break;
                }
                case kArg:
                {
                    if (instruction.type_1 == kTmp)
                    {
                        size_t index = FindVarInTable (&tmp_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        tmp_table.table [index].usage_counter++;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else if (instruction.type == IR_RETURN_INDEX)
        {
            LOG (kDebug, "Return was identified\n");

            if (instruction.type_1 == kTmp)
            {
                size_t index = FindVarInTable (&tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table.table [index].usage_counter++;
            }
        }
        else if (instruction.type == IR_OPERATION_INDEX)
        {
            LOG (kDebug, "Operation was identified\n");

            tmp_table.table [tmp_table.counter] = {.var_index = instruction.res_index,
                                                    .instruction_decl_index = {},
                                                    .usage_counter = 0};
            tmp_table.counter++;
            if (ListCtor (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
            {
                return kCantCreateListIR;
            }
            ListPushFront (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), instruction_index);
            if (instruction.type_1 == kTmp)
            {
                size_t index = FindVarInTable (&tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table.table [index].usage_counter++;
            }
            if (instruction.type_2 == kTmp)
            {
                size_t index = FindVarInTable (&tmp_table, instruction.value_2.operand_2_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table.table [index].usage_counter++;
            }
        }
        else if (instruction.type == IR_CONDITIONAL_JUMP_INDEX)
        {
            LOG (kDebug, "Conditional jump was identified\n");

            if (instruction.type_1 == kTmp)
            {
                size_t index = FindVarInTable (&tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table.table [index].usage_counter++;
            }
        }
        else if (instruction.type == IR_SYSTEM_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Syscall was identified\n");

            tmp_table.table [tmp_table.counter] = {.var_index = instruction.res_index,
                                                   .instruction_decl_index = {},
                                                   .usage_counter = 1};
            tmp_table.counter++;
            if (ListCtor (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
            {
                return kCantCreateListIR;
            }
            ListPushFront (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), instruction_index);
        }
        else if (instruction.type == IR_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Function call was identified\n");

            tmp_table.table [tmp_table.counter] = {.var_index = instruction.res_index,
                                                   .instruction_decl_index = {},
                                                   .usage_counter = 1};
            tmp_table.counter++;
            if (ListCtor (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
            {
                return kCantCreateListIR;
            }
            ListPushFront (&(tmp_table.table [tmp_table.counter - 1].instruction_decl_index), instruction_index);
        }

        *instruction_index = NextIndex (IR_list, *instruction_index);
        ListElemValLoad (IR_list, *instruction_index, &instruction);
    }

    KillUnusedLocalVars (IR_list, &tmp_table);

    TableDtor (&tmp_table);
    FREE_AND_NULL (tmp_table.table);

    return kDoneIR;
}

static size_t FindVarInTable (const VarUsageTable_t* const table, const size_t var_index)
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

static enum IRError FillLocalVarsTablesAndKillVars (list_t* const IR_list, VarUsageTable_t* const global_table,
                                                    size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (global_table      != NULL, "Invalid argument global_table\n");
    ASSERT (instruction_index != NULL, "Invalid argument instruction_index\n");

    while (*instruction_index != 0)
    {
        LOG (kDebug, "I am in function\n");

        size_t cnt_loc_vars = 0;
        enum IRError result = GetLocalVarsNum (IR_list, &cnt_loc_vars, instruction_index);
        if (result != kDoneIR)
        {
            return result;
        }

        VarUsageTable_t local_table = {};
        local_table.counter = 0;

        local_table.table = (VarUsage_t*) calloc (cnt_loc_vars, sizeof (VarUsage_t));
        if (local_table.table == NULL)
        {
            return kCantCreateLocalVarsTable;
        }

        size_t cnt_tmp_vars = 0;
        result = GetTmpVarsNum (IR_list, &cnt_tmp_vars, instruction_index);
        if (result != kDoneIR)
        {
            return result;
        }

        LOG (kDebug, "Tmp in function = %lu\n", cnt_tmp_vars);

        VarUsageTable_t tmp_table = {};
        tmp_table.counter = 0;

        tmp_table.table = (VarUsage_t*) calloc (cnt_tmp_vars, sizeof (VarUsage_t));
        if (tmp_table.table == NULL)
        {
            TableDtor (&local_table);
            FREE_AND_NULL (local_table.table);
            return kCantCreateLocalVarsTable;
        }

        result = FillLocalVarsTable (IR_list, &local_table, &tmp_table, global_table, instruction_index);
        if (result != kDoneIR)
        {
            TableDtor (&tmp_table);
            FREE_AND_NULL (tmp_table.table);
            TableDtor (&local_table);
            FREE_AND_NULL (local_table.table);
            return result;
        }

        LOG (kDebug, "Filled tables\n");

        KillUnusedLocalVars (IR_list, &local_table);
        KillUnusedLocalVars (IR_list, &tmp_table);

        LOG (kDebug, "Killed variables\n");

        TableDtor (&tmp_table);
        FREE_AND_NULL (tmp_table.table);
        TableDtor (&local_table);
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

static enum IRError FillLocalVarsTable (list_t* const IR_list,
                                        VarUsageTable_t* const local_table,
                                        VarUsageTable_t* const tmp_table,
                                        VarUsageTable_t* const global_table,
                                        size_t* const instruction_index)
{
    ASSERT (IR_list           != NULL, "Invalid argument IR_list\n");
    ASSERT (local_table       != NULL, "Invalid argument local_table\n");
    ASSERT (tmp_table         != NULL, "Invalid argument tmp_table\n");
    ASSERT (global_table      != NULL, "Invalid argument global_table\n");
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
                    size_t index = FindVarInTable (global_table, instruction.res_index);
                    if (index == ULLONG_MAX)
                    {
                        return kInvalidIRStruct;
                    }
                    else
                    {
                        ListPushFront (&(global_table->table [index].instruction_decl_index), instruction_index);
                    }
                    if (instruction.type_1 == kTmp)
                    {
                        size_t index = FindVarInTable (tmp_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        tmp_table->table [index].usage_counter++;
                    }
                    break;
                }
                case kVar:
                {
                    size_t index = FindVarInTable (local_table, instruction.res_index);
                    if (index == ULLONG_MAX)
                    {
                        local_table->table [local_table->counter] = {.var_index = instruction.res_index,
                                                                     .instruction_decl_index = {},
                                                                     .usage_counter = 0};
                        local_table->counter++;
                        if (ListCtor (&(local_table->table [local_table->counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
                        {
                            return kCantCreateListIR;
                        }
                        ListPushFront (&(local_table->table [local_table->counter - 1].instruction_decl_index), instruction_index);
                    }
                    else
                    {
                        ListPushFront (&(local_table->table [index].instruction_decl_index), instruction_index);
                    }
                    if (instruction.type_1 == kTmp)
                    {
                        size_t index = FindVarInTable (tmp_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        tmp_table->table [index].usage_counter++;
                    }
                    break;
                }
                case kTmp:
                {
                    tmp_table->table [tmp_table->counter] = {.var_index = instruction.res_index,
                                                             .instruction_decl_index = {},
                                                             .usage_counter = 0};
                    tmp_table->counter++;
                    if (ListCtor (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
                    {
                        return kCantCreateListIR;
                    }
                    ListPushFront (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), instruction_index);
                    if (instruction.type_1 == kGlobalVar)
                    {
                        size_t index = FindVarInTable (global_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        global_table->table [index].usage_counter++;
                    }
                    else if (instruction.type_1 == kVar)
                    {
                        size_t index = FindVarInTable (local_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        local_table->table [index].usage_counter++;
                    }
                    break;
                }
                case kArg:
                {
                    if (instruction.type_1 == kTmp)
                    {
                        size_t index = FindVarInTable (tmp_table, instruction.value_1.operand_1_index);
                        if (index == ULLONG_MAX)
                        {
                            return kUndefinedVarIR;
                        }
                        tmp_table->table [index].usage_counter++;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else if (instruction.type == IR_RETURN_INDEX)
        {
            LOG (kDebug, "Return was identified\n");

            if (instruction.res_type == kTmp)
            {
                size_t index = FindVarInTable (tmp_table, instruction.res_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table->table [index].usage_counter++;
            }
        }
        else if (instruction.type == IR_OPERATION_INDEX)
        {
            LOG (kDebug, "Operation was identified\n");

            tmp_table->table [tmp_table->counter] = {.var_index = instruction.res_index,
                                                    .instruction_decl_index = {},
                                                    .usage_counter = 0};
            tmp_table->counter++;
            if (ListCtor (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
            {
                return kCantCreateListIR;
            }
            ListPushFront (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), instruction_index);
            if (instruction.type_1 == kTmp)
            {
                size_t index = FindVarInTable (tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table->table [index].usage_counter++;
            }
            if (instruction.type_2 == kTmp)
            {
                size_t index = FindVarInTable (tmp_table, instruction.value_2.operand_2_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table->table [index].usage_counter++;
            }
        }
        else if (instruction.type == IR_CONDITIONAL_JUMP_INDEX)
        {
            LOG (kDebug, "Conditional jump was identified\n");

            if (instruction.type_1 == kTmp)
            {
                size_t index = FindVarInTable (tmp_table, instruction.value_1.operand_1_index);
                if (index == ULLONG_MAX)
                {
                    return kUndefinedVarIR;
                }
                tmp_table->table [index].usage_counter++;
            }
        }
        else if (instruction.type == IR_SYSTEM_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Syscall was identified\n");

            tmp_table->table [tmp_table->counter] = {.var_index = instruction.res_index,
                                                     .instruction_decl_index = {},
                                                     .usage_counter = 1};
            tmp_table->counter++;
            if (ListCtor (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
            {
                return kCantCreateListIR;
            }
            ListPushFront (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), instruction_index);
        }
        else if (instruction.type == IR_FUNCTION_CALL_INDEX)
        {
            LOG (kDebug, "Function call was identified\n");

            tmp_table->table [tmp_table->counter] = {.var_index = instruction.res_index,
                                                     .instruction_decl_index = {},
                                                     .usage_counter = 1};
            tmp_table->counter++;
            if (ListCtor (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), kStartNumberOfListElem, sizeof (size_t)) != kDoneList)
            {
                return kCantCreateListIR;
            }
            ListPushFront (&(tmp_table->table [tmp_table->counter - 1].instruction_decl_index), instruction_index);
        }

        *instruction_index = NextIndex (IR_list, *instruction_index);
        ListElemValLoad (IR_list, *instruction_index, &instruction);
    }

    return kDoneIR;
}

static enum IRError KillUnusedLocalVars (list_t* const IR_list, VarUsageTable_t* const local_table)
{
    ASSERT (IR_list     != NULL, "Invalid argument IR_list\n");
    ASSERT (local_table != NULL, "Invalid argument local_table\n");

    IRInstruction_t unused_instruction = {};

    for (size_t index = 0; index < local_table->counter; index++)
    {
        if (local_table->table [index].usage_counter == 0)
        {
            size_t index_del = 0;
            ListError result_list = kDoneList;
            result_list = ListPopFront(&(local_table->table [index].instruction_decl_index), &index_del);
            while (result_list == kDoneList)
            {
                ListPopAfterIndex (IR_list, &unused_instruction, index_del);
                result_list = ListPopFront(&(local_table->table [index].instruction_decl_index), &index_del);
            }
        }
    }

    return kDoneIR;
}

enum IRError TableDtor (VarUsageTable_t* const table)
{
    ASSERT (table != NULL, "Invalid argument table\n");

    for (size_t index = 0; index < table->counter; index++)
    {
        ListDtor (&(table->table [index].instruction_decl_index));
    }

    return kDoneIR;
}
