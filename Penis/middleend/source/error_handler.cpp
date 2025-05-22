#include "error_handler.h"
#include "middleend.h"

const char* EnumErrorToStr (const enum IRError error)
{
    #define CASE(error)             \
        case error:                 \
        {                           \
            return #error;          \
        }

    switch (error)
    {
        CASE (kDoneIR);

        CASE (kCantReadBufferIR);
        CASE (kCantCtorCntrlFlowListIR);

        CASE (kNoBracketsIR);
        CASE (kInvalidSyscallIR);
        CASE (kInvalidOperationIR);
        CASE (kNoSeparateSymbolIR);
        CASE (kInvalidPrefixIR);

        CASE (kCantCreateListIR);
        CASE (kCantPushListElemIR);
        CASE (kInvalidKeyWordIR);

        CASE (kInvalidAssigningIR);
        CASE (kCantIdentifyLabelIR);
        CASE (kInvalidOrderListIR);

        CASE (kCantCreateGlobalVarsTable);
        CASE (kUndefinedVarIR);
        CASE (kCantCreateLocalVarsTable);
        CASE (kInvalidIRStruct);

        CASE (kEmptyProgramIR);

        default:
            return "Invalid enum element";
    }

    #undef CASE
}
