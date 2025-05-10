#ifndef PYAM_IR_SRC_DSL_WRITE_H
#define PYAM_IR_SRC_DSL_WRITE_H

#include <stdio.h>

#include "key_words.h"

#define IR_NUM_(assign_num_, num_)                                                                  \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(88_tmp%zu, %ld)\t# num\n",                                                        \
            kIRAssignKeyWord,                                                                       \
            assign_num_,                                                                            \
            num_                                                                                    \
        )

#define IR_VAR_(assign_num_, var_num_)                                                              \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(88_tmp%zu, 32_var%ld)\t# var\n",                                                  \
            kIRAssignKeyWord,                                                                       \
            assign_num_,                                                                            \
            var_num_                                                                                \
        )

#define IR_GIVE_ARG_(arg_num_, operand1_)                                                           \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(14_arg%zu, 88_tmp%zu)\t# give arg\n",                                             \
            kIRAssignKeyWord,                                                                       \
            arg_num_,                                                                               \
            operand1_                                                                               \
        )

#define IR_TAKE_ARG_(assign_num_, arg_num_)                                                         \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(32_var%ld, 14_arg%zu)\t# take arg\n",                                             \
            kIRAssignKeyWord,                                                                       \
            assign_num_,                                                                            \
            arg_num_                                                                                \
        )

#define IR_ASSIGN_(assign_num_, op_type_, operand1_, operand2_)                                     \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(88_tmp%zu, %d, 88_tmp%zu, 88_tmp%zu)\t# %s\n",                                    \
            kIRAssignKeyWord,                                                                       \
            assign_num_,                                                                            \
            op_type_,                                                                               \
            operand1_,                                                                              \
            operand2_,                                                                              \
            ir_op_type_to_str(op_type_)                                                             \
        )

#define IR_ASSIGN_VAR_(assign_var_, operand1_)                                                      \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(32_var%ld, 88_tmp%zu)\t# Put result to the variable\n",                           \
            kIRAssignKeyWord,                                                                       \
            assign_var_,                                                                            \
            operand1_                                                                               \
        )

#define IR_COND_JMP_(label_num_, cond_res_, comment)                                                \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "%s(label%zu, 88_tmp%zu)\t# " comment "\n",                                             \
            kIRJumpKeyWord,                                                                         \
            label_num_,                                                                             \
            cond_res_                                                                               \
        )

#define IR_JMP_(label_num_, comment)                                                                \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "%s(label%zu)\t# " comment "\n",                                                        \
            kIRJumpKeyWord,                                                                         \
            label_num_                                                                              \
        )

#define IR_LABEL_(label_num_, comment)                                                              \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "%s(label%zu)\t# " comment "\n",                                                        \
            kIRLabelKeyWord,                                                                        \
            label_num_                                                                              \
        )

#define IR_CALL_FUNC_(assign_num_, func_num_, cnt_args_)                                            \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\t%s(88_tmp%zu, func_%zu_%zu, %zu)\t# call func\n",                                    \
            kIRCallKeyWord,                                                                         \
            assign_num_,                                                                            \
            func_num_,                                                                              \
            cnt_args_,                                                                              \
            cnt_args_                                                                               \
        )

#define IR_CALL_MAIN_(assign_num_)                                                                  \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "%s(88_tmp%zu, main, 0)\t# call main\n",                                                \
            kIRCallKeyWord,                                                                         \
            assign_num_                                                                             \
        )

#define IR_IMPLEMENT_FUNC_(func_num_, cnt_args_)                                                    \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\n%s(func_%zu_%zu, %zu)\t# implement func\n",                                          \
            kIRFuncKeyWord,                                                                         \
            func_num_,                                                                              \
            cnt_args_,                                                                              \
            cnt_args_                                                                               \
        )

#define IR_IMPLEMENT_MAIN_()                                                                        \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "\n%s(main, 0)\t# implement main\n"                                                     \
            kIRCallKeyWord,                                                                         \
        )

#define IR_RET_(ret_val_)                                                                           \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "%s(88_tmp%zu)\t# ret\n",                                                               \
            kIRRetKeyWord,                                                                          \
            ret_val_                                                                                \
        )

#define IR_SYSCALL_(assign_val_, op_type_, operand1_)                                               \
        fprintf(                                                                                    \
            ir_file,                                                                                \
            "%s(88_tmp%zu, %d, 88_tmp%zu)\t# Syscall \"%s\"\n",                                     \
            kIRSystemKeyWord,                                                                       \
            assign_val_,                                                                            \
            op_type_,                                                                               \
            operand1_,                                                                              \
            ir_op_type_to_str(op_type_)                                                             \
        )

#endif /*PYAM_IR_SRC_DSL_WRITE_H*/
