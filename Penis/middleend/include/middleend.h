#ifndef MIDDLEEND_H
#define MIDDLEEND_H

#include <stdio.h>

enum IRError
{
    kDoneIR                     = 0,

    kCantReadBufferIR           = 1,
    kCantCtorCntrlFlowListIR    = 2,

    kNoBracketsIR               = 3,
    kInvalidSyscallIR           = 4,
    kInvalidOperationIR         = 5,
    kNoSeparateSymbolIR         = 6,
    kInvalidPrefixIR            = 7,

    kCantCreateListIR           = 8,
    kCantPushListElemIR         = 9,
    kInvalidKeyWordIR           = 10,

    kInvalidAssigningIR         = 11,
    kCantIdentifyLabelIR        = 12,
    kInvalidOrderListIR         = 13,

    kCantCreateGlobalVarsTable  = 14,
    kUndefinedVarIR             = 15,
    kCantCreateLocalVarsTable   = 16,
    kInvalidIRStruct            = 17,
    kEmptyProgramIR             = 18,
};

enum IRError OptimizeIR (FILE* const input_file, FILE* const output_file);

#endif // MIDDLEEND_H
