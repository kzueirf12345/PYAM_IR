#include "read_ir.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../middleend/include/middleend.h"

#include "list.h"
#include "list_construction.h"
#include "list_push.h"

#include "../../../include/key_words.h"
#include "../../../include/dsl_write.h"

#include "Assert/my_assert.h"
#include "Logger/logging.h"
#include "My_stdio/my_stdio.h"

#define CHECK_RESULT            \
    if (result != kDoneIR)    \
    {                           \
        return result;          \
    }

typedef struct current_position
{
    const char* buffer;
    size_t      read_letters;
    char        func_name [kMaxWordLenIR];
    size_t      global_vars_cnt;
} current_position_t;

static enum IRError ReadIRFromBuffer (const char* const buffer, list_t* const list);

static enum IRError CallFuncIR      (current_position_t* const cur_pos, list_t* const list);
static enum IRError FuncBodyIR      (current_position_t* const cur_pos, list_t* const list);
static enum IRError CondJumpIR      (current_position_t* const cur_pos, list_t* const list);
static enum IRError AssignIR        (current_position_t* const cur_pos, list_t* const list);

static enum IRError AssignVarIR     (current_position_t* const cur_pos, list_t* const list);
static enum IRError AssignTmpIR     (current_position_t* const cur_pos, list_t* const list);
static enum IRError AssignArgIR     (current_position_t* const cur_pos, list_t* const list);

static enum IRError OperationIR     (current_position_t* const cur_pos, list_t* const list);
static enum IRError LabelIR         (current_position_t* const cur_pos, list_t* const list);
static enum IRError ReturnIR        (current_position_t* const cur_pos, list_t* const list);
static enum IRError SysCallIR       (current_position_t* const cur_pos, list_t* const list);
static enum IRError GlobalVarsNumIR (current_position_t* const cur_pos, list_t* const list);

static void SkipNumber (const char* const input_buf, size_t* const offset);

typedef enum IRError (*TranslateIRFunc_t) (current_position_t* const cur_pos, list_t* const list);

static const TranslateIRFunc_t kTranslationArray [kIR_KEY_WORD_NUMBER] =
{
    CallFuncIR,
    FuncBodyIR,
    CondJumpIR,
    AssignIR,
    OperationIR,
    LabelIR,
    ReturnIR,
    SysCallIR,
    GlobalVarsNumIR
};

enum IRError ReadIR (const char* const buffer, list_t* const list)
{
    ASSERT (buffer != NULL, "Invalid argument buffer\n");
    ASSERT (list   != NULL, "Invalid argument list\n");

    enum ListError result = ListCtor(list, kStartNumberOfListElem, sizeof(IRInstruction_t));
    if (result != kDoneList)
    {
        return kCantCreateListIR;
    }

    return ReadIRFromBuffer (buffer, list);
}

static enum IRError ReadIRFromBuffer (const char* const buffer, list_t* const list)
{
    ASSERT (buffer != NULL, "Invalid argument buffer\n");
    ASSERT (list   != NULL, "Invalid argument list\n");

    enum IRError result = kDoneIR;

    current_position_t cur_pos =
    {
        .buffer          = buffer,
        .read_letters    = 0,
        .func_name       = "global",
        .global_vars_cnt = 0,
    };

    cur_pos.read_letters = skip_space_symbols (buffer);

    int can_read = 1;
    while (can_read)
    {
        cur_pos.read_letters += skip_space_symbols (buffer + cur_pos.read_letters);
        char key_word [kMaxWordLenIR] = "";

        if (*(buffer + cur_pos.read_letters) == kCommentSymbolIR)
        {
            const char* end_comment = strchr (buffer + cur_pos.read_letters, '\n');
            if (end_comment == NULL)
            {
                return kDoneIR;
            }
            cur_pos.read_letters = end_comment - buffer;
            cur_pos.read_letters += skip_space_symbols (buffer + cur_pos.read_letters);
        }

        can_read = sscanf (buffer + cur_pos.read_letters, "%[^ ^\t^\n^(]", key_word);
        if ((can_read == 0) || (strcmp (key_word, "") == 0))
        {
            break;
        }

        LOG (kDebug, "Key word = \"%s\"\n", key_word);

        cur_pos.read_letters += strlen (key_word);
        cur_pos.read_letters += skip_space_symbols (buffer + cur_pos.read_letters);
        if (*(buffer + cur_pos.read_letters) != kBracketOpenIR)
        {
            return kNoBracketsIR;
        }
        cur_pos.read_letters++;

        for (size_t index_kw = 0; index_kw <= kIR_KEY_WORD_NUMBER; index_kw++)
        {
            if (strcmp (key_word, kIR_KEY_WORD_ARRAY [index_kw]) == 0)
            {
                result = kTranslationArray [index_kw] (&cur_pos, list);
                CHECK_RESULT;
                break;
            }
        }
    }

    return kDoneIR;
}

static enum IRError CallFuncIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type = IR_FUNCTION_CALL_INDEX;
    list_elem.res_type = kTmp;
    list_elem.type_1 = list_elem.type_2 = kNone;

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &list_elem.res_index);
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%[^)^ ^\n^\t^\r]", list_elem.label);
    cur_pos->read_letters += strlen (list_elem.label);
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    LOG (kDebug, "Calling function is \"%s\"\n", list_elem.label);

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError FuncBodyIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type   = IR_FUNCTION_BODY_INDEX;
    list_elem.res_type = kNone;
    list_elem.type_1 = kNum;
    list_elem.type_2 = kNum;

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%[^,^ ^\n^\t^\r]", cur_pos->func_name);
    sscanf (cur_pos->func_name, "%s", list_elem.label);
    cur_pos->read_letters += strlen (cur_pos->func_name);
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.value_1.number_size_t));
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.value_2.number_size_t));
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    LOG (kDebug, "Start function \"%s\" body\n", cur_pos->func_name);

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError CondJumpIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type = IR_CONDITIONAL_JUMP_INDEX;
    list_elem.res_type = list_elem.type_1 = list_elem.type_2 = kNone;

    char label_name [kMaxWordLenIR] = "";
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%[^,^ ^\n^\t^\r]", label_name);
    cur_pos->read_letters += strlen (label_name);
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sprintf(list_elem.label, "%s_%s", cur_pos->func_name, label_name);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    bool cond_jump = (*(cur_pos->buffer + cur_pos->read_letters) != kTrueSymbolIR);
    if (cond_jump)
    {
        list_elem.type_1 = kTmp;
        sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.value_1.operand_1_index));
        cur_pos->read_letters += strlen (TMP_PREFIX);
        SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);
    }
    else
    {
        list_elem.type_1 = kNum;
        cur_pos->read_letters++;
    }

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError AssignIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    char prefix_name [kIRPrefixLen] = "";
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%3s", prefix_name);
    cur_pos->read_letters += kIRPrefixLen - 1;

    if (strcmp (prefix_name, VAR_PREFIX) == 0)
    {
        return AssignVarIR (cur_pos, list);
    }

    if (strcmp (prefix_name, TMP_PREFIX) == 0)
    {
        return AssignTmpIR (cur_pos, list);
    }

    if (strcmp (prefix_name, ARG_PREFIX) == 0)
    {
        return AssignArgIR (cur_pos, list);
    }

    return kInvalidPrefixIR;
}

static enum IRError AssignVarIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type = IR_ASSIGNMENT_INDEX;
    list_elem.res_type = list_elem.type_1 = list_elem.type_2 = kNone;

    long long var_index = 0;
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%lld", &var_index);
    if (var_index < 0)
    {
        list_elem.res_type = kGlobalVar;
        list_elem.res_index = (size_t) (((long long) cur_pos->global_vars_cnt) + var_index);
    }
    else
    {
        list_elem.res_type = kVar;
        list_elem.res_index = (size_t) (var_index);
    }
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    char prefix_name [kIRPrefixLen] = "";
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%3s", prefix_name);
    cur_pos->read_letters += kIRPrefixLen - 1;

    if (strcmp (prefix_name, TMP_PREFIX) == 0)
    {
        list_elem.type_1 = kTmp;
        sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.value_1.operand_1_index));
        SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
        {
            return kNoBracketsIR;
        }
        cur_pos->read_letters++;
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        enum ListError result = ListPushFront(list, &list_elem);

        if (result != kDoneList)
        {
            return kCantPushListElemIR;
        }

        return kDoneIR;
    }

    if (strcmp (prefix_name, ARG_PREFIX) == 0)
    {
        list_elem.type_1 = kArg;
        sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.value_1.operand_1_index));
        SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
        {
            return kNoBracketsIR;
        }
        cur_pos->read_letters++;
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        enum ListError result = ListPushFront(list, &list_elem);

        if (result != kDoneList)
        {
            return kCantPushListElemIR;
        }

        return kDoneIR;
    }

    return kInvalidPrefixIR;
}

static enum IRError AssignTmpIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type   = IR_ASSIGNMENT_INDEX;
    list_elem.res_type = kTmp;
    list_elem.type_1 = list_elem.type_2 = kNone;

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.res_index));
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (isdigit (*(cur_pos->buffer + cur_pos->read_letters)))
    {
        list_elem.type_1 = kNum;
        sscanf (cur_pos->buffer + cur_pos->read_letters, "%lg", &(list_elem.value_1.number_double));
        SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
        {
            return kNoBracketsIR;
        }
        cur_pos->read_letters++;
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        enum ListError result = ListPushFront(list, &list_elem);

        if (result != kDoneList)
        {
            return kCantPushListElemIR;
        }

        return kDoneIR;
    }

    char prefix_name [kIRPrefixLen] = "";
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%3s", prefix_name);
    cur_pos->read_letters += kIRPrefixLen - 1;

    if (strcmp (prefix_name, VAR_PREFIX) == 0)
    {
        long long var_index = 0;
        sscanf (cur_pos->buffer + cur_pos->read_letters, "%lld", &var_index);
        SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
        {
            return kNoBracketsIR;
        }
        cur_pos->read_letters++;
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        if (var_index >= 0)
        {
            list_elem.type_1 = kVar;
            list_elem.value_1.operand_1_index = (size_t) var_index;
        }
        else
        {
            list_elem.type_1 = kGlobalVar;
            list_elem.value_1.operand_1_index = (cur_pos->global_vars_cnt + 1 - (size_t) (-var_index));
        }

        enum ListError result = ListPushFront(list, &list_elem);

        if (result != kDoneList)
        {
            return kCantPushListElemIR;
        }

        return kDoneIR;
    }

    if (strcmp (prefix_name, TMP_PREFIX) == 0)
    {
        list_elem.type_1 = kTmp;
        sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.value_1.operand_1_index));
        SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
        {
            return kNoBracketsIR;
        }
        cur_pos->read_letters++;
        cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

        enum ListError result = ListPushFront(list, &list_elem);

        if (result != kDoneList)
        {
            return kCantPushListElemIR;
        }

        return kDoneIR;
    }

    return kInvalidAssigningIR;
}

static enum IRError AssignArgIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type     = IR_ASSIGNMENT_INDEX;
    list_elem.res_type = kArg;
    list_elem.type_1   = kTmp;
    list_elem.type_2   = kNone;

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &(list_elem.res_index));
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.value_1.operand_1_index));
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError OperationIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type   = IR_OPERATION_INDEX;
    list_elem.res_type = kTmp;
    list_elem.type_1 = kTmp;
    list_elem.type_2 = kTmp;

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.res_index));
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);


    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    int operation = 0;
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%d", &(operation));
    list_elem.operation = (enum IrOpType) operation;
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.value_1.operand_1_index));
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.value_2.operand_2_index));
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError LabelIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type   = IR_LABEL_INDEX;
    list_elem.res_type = list_elem.type_1 = list_elem.type_2 = kNone;

    char label_name [kMaxWordLenIR] = "";
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%[^,^ ^\n^\t^\r^)]", label_name);
    cur_pos->read_letters += strlen (label_name);
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    LOG (kDebug, "Label \"%s\" was analyzed\n", label_name);

    sprintf (list_elem.label, "%s_%s", cur_pos->func_name, label_name);

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError ReturnIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type     = IR_RETURN_INDEX;
    list_elem.res_type = kTmp;
    list_elem.type_1 = list_elem.type_2 = kNone;

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.res_index));
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    const char* end_args = strchr (cur_pos->buffer + cur_pos->read_letters, kBracketCloseIR);
    if (end_args == NULL)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters = end_args - cur_pos->buffer;
    cur_pos->read_letters++;

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError SysCallIR (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type   = IR_SYSTEM_FUNCTION_CALL_INDEX;
    list_elem.res_type = kTmp;
    list_elem.type_1 = list_elem.type_2 = kNone;

    sscanf (cur_pos->buffer + cur_pos->read_letters, TMP_PREFIX "%lu", &(list_elem.res_index));
    cur_pos->read_letters += strlen (TMP_PREFIX);
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kSepSymIR)
    {
        return kNoSeparateSymbolIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    sscanf (cur_pos->buffer + cur_pos->read_letters, "%[^)^ ^\n^\t^\r^,]", list_elem.label);
    cur_pos->read_letters += strlen (list_elem.label);
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    const char* end_args = strchr (cur_pos->buffer + cur_pos->read_letters, kBracketCloseIR);
    if (end_args == NULL)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters = end_args - cur_pos->buffer;
    cur_pos->read_letters++;

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static enum IRError GlobalVarsNumIR  (current_position_t* const cur_pos, list_t* const list)
{
    ASSERT (cur_pos != NULL, "Invalid argument cur_pos\n");
    ASSERT (list    != NULL, "Invalid argument list\n");

    LOG (kDebug, "Current symbol       = {%c}\n"
                 "Already read letters = %lu\n",
                 *(cur_pos->buffer + cur_pos->read_letters), cur_pos->read_letters);

    IRInstruction_t list_elem = {};
    list_elem.type   = IR_GLOBAL_VARS_NUM_INDEX;
    list_elem.type_1 = kNum;
    list_elem.res_type = list_elem.type_2 = kNone;

    size_t cnt_glob_vars = 0;
    sscanf (cur_pos->buffer + cur_pos->read_letters, "%lu", &cnt_glob_vars);
    list_elem.value_1.number_size_t = cnt_glob_vars;
    SkipNumber (cur_pos->buffer, &(cur_pos->read_letters));
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    if (*(cur_pos->buffer + cur_pos->read_letters) != kBracketCloseIR)
    {
        return kNoBracketsIR;
    }
    cur_pos->read_letters++;
    cur_pos->read_letters += skip_space_symbols (cur_pos->buffer + cur_pos->read_letters);

    cur_pos->global_vars_cnt = cnt_glob_vars;

    enum ListError result = ListPushFront(list, &list_elem);

    if (result != kDoneList)
    {
        return kCantPushListElemIR;
    }

    return kDoneIR;
}

static void SkipNumber (const char* const input_buf, size_t* const offset)
{
    ASSERT (input_buf != NULL, "Invalid argument input_buf = %p\n", input_buf);
    ASSERT (offset    != NULL, "Invalid argument offset = %p\n",    offset);

    LOG (kDebug, "Input buffer    = %p\n"
                 "Offset          = %lu\n"
                 "Run time symbol = {%c}\n",
                 input_buf, *offset, input_buf [*offset]);

    if (input_buf [*offset] == '-')
    {
        (*offset)++;
    }

    while (isdigit (input_buf [*offset]))
    {
        (*offset)++;
    }

    if (input_buf [*offset] == '.')
    {
        (*offset)++;
    }

    while (isdigit (input_buf [*offset]))
    {
        (*offset)++;
    }
}
