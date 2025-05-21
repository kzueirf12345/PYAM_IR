#ifndef READ_IR_H
#define READ_IR_H

#include <stdlib.h>

#include "../../middleend/include/middleend.h"

#include "../../../include/key_words.h"
#include "../../../include/operations.h"

#include "list.h"

static const char kCommentSymbolIR = '#';
static const char kBracketOpenIR   = '(';
static const char kBracketCloseIR  = ')';
static const char kSepSymIR        = ',';
static const char kTrueSymbolIR    = '1';

static const size_t kMaxWordLenIR          = 200;
static const size_t kStartNumberOfListElem = 100;
static const size_t kIRPrefixLen           = 4;

enum IROperandType
{
    kTmp       = 0,
    kVar       = 1,
    kGlobalVar = 2,
    kArg       = 3,
    kNum       = 4,
};

typedef struct IRInstruction
{
    enum IR_KeyWord_Indexes type;
    enum IROperandType      res_type;
    size_t                  res_index;
    char                    label [2 * kMaxWordLenIR];
    enum IrOpType           operation;
    enum IROperandType      type_1;
    union
    {
        size_t              operand_1_index;
        size_t              number_size_t;
        double              number_double;
    } value_1;
    enum IROperandType      type_2;
    union
    {
        size_t              operand_2_index;
        size_t              number_size_t;
        double              number_double;
    } value_2;
} IRInstruction_t;

enum IRError ReadIR (const char* const buffer, list_t* const list);

#endif // READ_IR_H
