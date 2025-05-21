#ifndef DEAD_CODE_ELIM_H
#define DEAD_CODE_ELIM_H

#include "middleend.h"
#include "read_ir.h"
#include "list.h"

typedef struct CntrlFlowGraphNode
{
    IRInstruction_t instruction;
    size_t          dflt_branch_index;
    size_t          subbranch_index;
    bool            controlled;
} CntrlFlowGraphNode_t;

typedef struct LabelLocation
{
    char   label [kMaxWordLenIR];
    size_t instruction_index;
} LabelLocation_t;

enum IRError DeadCodeElimination (list_t* const IR_list);

#endif // DEAD_CODE_ELIM_H
