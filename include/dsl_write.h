#ifndef PYAM_IR_SRC_DSL_WRITE_H
#define PYAM_IR_SRC_DSL_WRITE_H

#include <stdio.h>

#include "operations.h"
#include "key_words.h"

#define IR_OPERATION_(return_value_, op_type_, operand1_, operand2_)                                \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu, %d, tmp%zu, tmp%zu)\t# %s\n",                                             \
            kIR_KEY_WORD_ARRAY[IR_OPERATION_INDEX],                                                 \
            return_value_,                                                                          \
            op_type_,                                                                               \
            operand1_,                                                                              \
            operand2_,                                                                              \
            ir_op_type_to_str(op_type_)                                                             \
        )


#define IR_GIVE_ARG_(arg_num_, temp_variable_index_)                                                \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(arg%zu, tmp%zu)\n",                                                               \
            kIR_KEY_WORD_ARRAY[IR_ASSIGNMENT_INDEX],                                                \
            arg_num_,                                                                               \
            temp_variable_index_                                                                    \
        )

#define IR_TAKE_ARG_(var_index_, arg_num_)                                                          \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(var%lld, arg%zu)\n",                                                              \
            kIR_KEY_WORD_ARRAY[IR_ASSIGNMENT_INDEX],                                                \
            var_index_,                                                                             \
            arg_num_                                                                                \
        )

#define IR_ASSIGN_TMP_VAR_(tmp_index_, var_index_, comment)                                         \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu, var%lld)\t # %s\n",                                                       \
            kIR_KEY_WORD_ARRAY[IR_ASSIGNMENT_INDEX],                                                \
            tmp_index_,                                                                             \
            var_index_,                                                                             \
            comment                                                                                 \
        )

#define IR_ASSIGN_TMP_TMP_(tmp_index1_, tmp_index2_)                                                \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu, tmp%zu)\n",                                                               \
            kIR_KEY_WORD_ARRAY[IR_ASSIGNMENT_INDEX],                                                \
            tmp_index1_,                                                                            \
            tmp_index2_                                                                             \
        )

#define IR_ASSIGN_TMP_NUM_(tmp_index1_, num_)                                                       \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu, %lg)\n",                                                                  \
            kIR_KEY_WORD_ARRAY[IR_ASSIGNMENT_INDEX],                                                \
            tmp_index1_,                                                                            \
            num_                                                                                    \
        )

#define IR_ASSIGN_VAR_(variable_index_, temp_variable_index_, comment)                              \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(var%lld, tmp%zu)\t# Variable: %s\n",                                              \
            kIR_KEY_WORD_ARRAY[IR_ASSIGNMENT_INDEX],                                                \
            variable_index_,                                                                        \
            temp_variable_index_,                                                                   \
            comment                                                                                 \
        )

#define IR_COND_JMP_(label_num_, cond_res_, comment)                                                \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(label%zu, tmp%zu)\t# %s\n",                                                       \
            kIR_KEY_WORD_ARRAY[IR_CONDITIONAL_JUMP_INDEX],                                          \
            label_num_,                                                                             \
            cond_res_,                                                                              \
            comment                                                                                 \
        )

#define IR_JMP_(label_num_, comment)                                                                \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(label%zu, 1)\t# %s\n",                                                            \
            kIR_KEY_WORD_ARRAY[IR_CONDITIONAL_JUMP_INDEX],                                          \
            label_num_,                                                                             \
            comment                                                                                 \
        )

#define IR_LABEL_(label_num_, comment)                                                              \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(label%zu)\t# %s\n",                                                               \
            kIR_KEY_WORD_ARRAY[IR_LABEL_INDEX],                                                     \
            label_num_,                                                                             \
            comment                                                                                 \
        )

#define IR_CALL_FUNC_(return_value_, func_num_, cnt_args_, comment)                                 \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu, func_%zu_%zu)\t# %s\n",                                                   \
            kIR_KEY_WORD_ARRAY[IR_FUNCTION_CALL_INDEX],                                             \
            return_value_,                                                                          \
            func_num_,                                                                              \
            cnt_args_,                                                                              \
            comment                                                                                 \
        )

#define IR_CALL_MAIN_(return_value_)                                                                \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "%s(tmp%zu, main, 0)\t# call main\n",                                                   \
            kIR_KEY_WORD_ARRAY[IR_FUNCTION_CALL_INDEX],                                             \
            return_value_                                                                           \
        )

#define IR_FUNCTION_BODY_(func_num_, cnt_args_, cnt_local_vars_, comment)                           \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\n%s(func_%zu_%zu, %zu, %zu)\t# %s\n",                                                 \
            kIR_KEY_WORD_ARRAY[IR_FUNCTION_BODY_INDEX],                                             \
            func_num_,                                                                              \
            cnt_args_,                                                                              \
            cnt_args_,                                                                              \
            cnt_local_vars_,                                                                        \
            comment                                                                                 \
        )

#define IR_MAIN_BODY_(cnt_local_vars_)                                                              \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\n%s(main, 0, %zu)\t# implement main\n",                                               \
            kIR_KEY_WORD_ARRAY[IR_FUNCTION_BODY_INDEX],                                             \
            cnt_local_vars_                                                                         \
        )

#define IR_RET_(return_value_)                                                                      \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu)\n",                                                                       \
            kIR_KEY_WORD_ARRAY[IR_RETURN_INDEX],                                                    \
            return_value_                                                                           \
        )

#define IR_SYSCALL_(return_value_, syscall_name_, arg_cnt_)                                         \
        fprintf(                                                                                    \
            IR_file,                                                                                \
            "\t%s(tmp%zu, %s, %zu)\t# System function call: \"%s\"\n",                              \
            kIR_KEY_WORD_ARRAY[IR_SYSTEM_FUNCTION_CALL_INDEX],                                      \
            return_value_,                                                                          \
            syscall_name_,                                                                          \
            arg_cnt_,                                                                               \
            syscall_name_                                                                           \
        )

#endif /*PYAM_IR_SRC_DSL_WRITE_H*/
