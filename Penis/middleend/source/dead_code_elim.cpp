#include "dead_code_elim.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>

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

static enum IRError ConnectCntrlFlowGraph (const list_t* const IR_list, list_t* const cntrl_flow_list);
static enum IRError FillLabelLocationList (const list_t* const IR_list, list_t* const label_loc_list);
static enum IRError MarkNodes             (const list_t* const cntrl_flow_list, const size_t index);
static enum IRError KillNodes             (list_t* const IR_list, list_t* const cntrl_flow_list);

static size_t FindLabel (const list_t* const label_loc_list, const char* const label);

enum IRError DeadCodeElimination (list_t* const IR_list)
{
    ASSERT (IR_list != NULL, "Invalid argument IR_list\n");

    LOG (kDebug, "Starting dead code elimination (before creating list)\n");

    list_t cntrl_flow_list = {};
    enum ListError result_list = ListCtor (&cntrl_flow_list, IR_list->counter, sizeof (CntrlFlowGraphNode_t));
    if (result_list != kDoneList)
    {
        return kCantCtorCntrlFlowListIR;
    }

    LOG (kDebug, "Starting dead code elimination (after creating list)\n");

    enum IRError result_ir = ConnectCntrlFlowGraph (IR_list, &cntrl_flow_list);
    if (result_ir != kDoneIR)
    {
        ListDtor (&cntrl_flow_list);
        return result_ir;
    }

    LOG (kDebug, "The Control flow graph was created\n");

    result_ir = MarkNodes (&cntrl_flow_list, TailIndex (&cntrl_flow_list));
    if (result_ir != kDoneIR)
    {
        ListDtor (&cntrl_flow_list);
        return result_ir;
    }

    LOG (kDebug, "The nodes were marked\n");

    result_ir = KillNodes (IR_list, &cntrl_flow_list);
    ListDtor (&cntrl_flow_list);
    return result_ir;
}

static enum IRError ConnectCntrlFlowGraph (const list_t* const IR_list, list_t* const cntrl_flow_list)
{
    ASSERT (IR_list         != NULL, "Invalid argument IR_list\n");
    ASSERT (cntrl_flow_list != NULL, "Invalid argument cntrl_flow_list\n");

    CntrlFlowGraphNode_t node = {};
    node.controlled = false;
    size_t instruction_index = TailIndex (IR_list);

    LOG (kDebug, "Starting connecting control flow graph\n");

    list_t label_loc_list = {};
    if (ListCtor (&label_loc_list, IR_list->counter, sizeof (LabelLocation_t)) != kDoneList)
    {
        return kCantCreateListIR;
    }

    enum IRError result = FillLabelLocationList (IR_list, &label_loc_list);
    if (result != kDoneIR)
    {
        ListDtor (&label_loc_list);
        return result;
    }

    LOG (kDebug, "The table of labels was filled\n");

    while (instruction_index != 0)
    {
        ListElemValLoad (IR_list, instruction_index, &(node.instruction));
        instruction_index = NextIndex (IR_list, instruction_index);

        LOG (kDebug, "The next index = %lu\n", instruction_index);

        if ((node.instruction.type == IR_CONDITIONAL_JUMP_INDEX)
            || (node.instruction.type == IR_FUNCTION_CALL_INDEX))
        {
            node.subbranch_index = FindLabel (&label_loc_list, node.instruction.label);
            if (node.subbranch_index == ULLONG_MAX)
            {
                ListDtor (&label_loc_list);
                return kCantIdentifyLabelIR;
            }
        }

        node.dflt_branch_index = instruction_index;
        if (ListPushFront (cntrl_flow_list, &node) != kDoneList)
        {
            ListDtor (&label_loc_list);
            return kCantPushListElemIR;
        }
    }

    ListDtor (&label_loc_list);
    return kDoneIR;
}

static enum IRError FillLabelLocationList (const list_t* const IR_list, list_t* const label_loc_list)
{
    ASSERT (IR_list        != NULL, "Invalid argument IR_list\n");
    ASSERT (label_loc_list != NULL, "Invalid argument label_loc_list\n");

    IRInstruction_t instruction = {};
    LabelLocation_t location = {};
    size_t instruction_index = TailIndex (IR_list);

    LOG (kDebug, "Starting filling table of labels\n");

    while (instruction_index != 0)
    {
        ListElemValLoad (IR_list, instruction_index, &(instruction));

        if ((instruction.type == IR_LABEL_INDEX) || (instruction.type == IR_FUNCTION_BODY_INDEX))
        {
            strcpy (location.label, instruction.label);
            location.instruction_index = instruction_index;

            if (ListPushFront (label_loc_list, &location) != kDoneList)
            {
                return kCantPushListElemIR;
            }
        }

        instruction_index = NextIndex (IR_list, instruction_index);
    }

    return kDoneIR;
}

static size_t FindLabel (const list_t* const label_loc_list, const char* const label)
{
    ASSERT (label_loc_list != NULL, "Invalid argument label_loc_list\n");
    ASSERT (label          != NULL, "Invalid argument label\n");

    LOG (kDebug, "Looking for label \"%s\"\n", label);

    LabelLocation_t location = {};
    size_t label_index = TailIndex (label_loc_list);

    while (label_index != 0)
    {
        ListElemValLoad (label_loc_list, label_index, &(location));

        if (strcmp (location.label, label) == 0)
        {
            return location.instruction_index;
        }

        label_index = NextIndex (label_loc_list, label_index);
    }

    return ULLONG_MAX;
}

static enum IRError MarkNodes (const list_t* const cntrl_flow_list, const size_t index)
{
    ASSERT (cntrl_flow_list != NULL, "Invalid argument cntrl_flow_list\n");

    if (index == 0)
    {
        return kDoneIR;
    }

    CntrlFlowGraphNode_t node = {};
    ListElemValLoad (cntrl_flow_list, index, &node);

    LOG (kDebug, "Starting marking nodes\n");

    if ((node.controlled) || (node.instruction.type == IR_RETURN_INDEX))
    {
        node.controlled = true;
        ListElemValStor (cntrl_flow_list, index, &node);
        return kDoneIR;
    }

    node.controlled = true;
    ListElemValStor (cntrl_flow_list, index, &node);

    if ((node.instruction.type == IR_CONDITIONAL_JUMP_INDEX)
        || (node.instruction.type == IR_FUNCTION_CALL_INDEX))
    {
        enum IRError result = MarkNodes (cntrl_flow_list, node.subbranch_index);
        if (result != kDoneIR)
        {
            return result;
        }

        if ((node.instruction.type == IR_FUNCTION_CALL_INDEX) || (node.instruction.type_1 != kNum))
        {
            enum IRError result = MarkNodes (cntrl_flow_list, node.dflt_branch_index);
            if (result != kDoneIR)
            {
                return result;
            }
        }
        return kDoneIR;
    }

    return MarkNodes (cntrl_flow_list, node.dflt_branch_index);
}

static enum IRError KillNodes (list_t* const IR_list, list_t* const cntrl_flow_list)
{
    ASSERT (IR_list         != NULL, "Invalid argument IR_list\n");
    ASSERT (cntrl_flow_list != NULL, "Invalid argument cntrl_flow_list\n");

    CntrlFlowGraphNode_t node = {};
    size_t instruction_index = TailIndex (IR_list);
    size_t previous_index = instruction_index;

    while (instruction_index != 0)
    {
        LOG (kDebug, "Index = %lu\n", instruction_index);

        ListElemValLoad (cntrl_flow_list, instruction_index, &node);
        instruction_index = NextIndex (IR_list, instruction_index);

        if (!node.controlled)
        {
            ListPopAfterIndex (IR_list, &(node.instruction), previous_index);
        }
        previous_index = instruction_index;
    }

    TryListResize (IR_list);

    return kDoneIR;
}
