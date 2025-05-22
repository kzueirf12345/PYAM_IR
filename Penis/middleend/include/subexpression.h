#ifndef SUBEXPRESSION_H
#define SUBEXPRESSION_H

#include <stdlib.h>

#include "middleend.h"
#include "read_ir.h"
#include "list.h"

static const size_t kStartListNumberElemForOperations = 100;
static const double kEpsilon = 0.00001;

typedef struct SSA
{
    size_t var_index;
    size_t assign_counter;
} SSA_t;

typedef struct SSATable
{
    SSA_t* table;
    size_t counter;
} SSATable_t;

typedef struct TMPValue
{
    size_t tmp_index;
    enum IROperandType type;
    union
    {
        double number;
        SSA_t  variable;
    } value;
} TMPValue_t;

typedef struct TMPValueTable
{
    TMPValue_t* table;
    size_t      counter;
} TMPValueTable_t;

typedef struct OperationInfo
{
    size_t          instruction_index;
    IRInstruction_t instruction;
    TMPValue_t      op_1;
    TMPValue_t      op_2;
} OperationInfo_t;

enum IRError FindSimilarExpression (list_t* const IR_list);

#endif // SUBEXPRESSION_H
