#include "middleend.h"

#include <stdio.h>

#include "read_ir.h"
#include "write_ir.h"

#include "dead_code_elim.h"
#include "subexpression.h"

#include "list.h"
#include "list_construction.h"

#include "Assert/my_assert.h"
#include "Logger/logging.h"
#include "My_stdio/my_stdio.h"
#include "helpful.h"

enum IRError OptimizeIR (FILE* const input_file, FILE* const output_file)
{
    ASSERT (input_file  != NULL, "Invalid argument input_file\n");
    ASSERT (output_file != NULL, "Invalid argument output_file\n");

    char* buffer = ReadFileToBuffer (input_file);
    if (buffer == NULL)
    {
        return kCantReadBufferIR;
    }

    LOG (kDebug, "The file was read to buffer\n");

    list_t IR_list = {};
    enum IRError result = ReadIR (buffer, &IR_list);
    FREE_AND_NULL (buffer);
    if (result != kDoneIR)
    {
        if (result != kCantCreateListIR)
        {
            ListDtor (&IR_list);
        }
        return result;
    }

    LOG (kDebug, "The IR was read to list\n");

    result = DeadCodeElimination (&IR_list);
    if (result != kDoneIR)
    {
        ListDtor (&IR_list);
        return result;
    }

    LOG (kDebug, "The IR was optimized by dead code elimination\n");

    result = WriteIRPYAM (&IR_list, output_file);
    ListDtor (&IR_list);
    return result;
}
