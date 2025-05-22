#ifndef UNUSED_VARS_H
#define UNUSED_VARS_H

#include "middleend.h"
#include "list.h"

static const size_t kStartListNumberElem = 10;

typedef struct VarUsage
{
    size_t var_index;
    size_t instruction_decl_index;
    size_t usage_counter;
} VarUsage_t;

typedef struct VarUsageTable
{
    VarUsage_t* table;
    size_t      counter;
} VarUsageTable_t;

enum IRError KillUnusedVars (list_t* const IR_list);

#endif // UNUSED_VARS_H
