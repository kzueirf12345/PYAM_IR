#ifndef PYAM_IR_DSL_WRITE_H
#define PYAM_IR_DSL_WRITE_H

#include <stdio.h>

#define IR_NUM_(assign_num_, num_)                                                                  \
        fprintf(                                                                                    \
            out,                                                                                    \
            "GIPSY(88_tmp%zu, %ld)\t# num\n",                                                       \
            assign_num_,                                                                            \
            num_                                                                                    \
        )

#define IR_VAR_(assign_num_, var_num_)                                                              \
        fprintf(                                                                                    \
            out,                                                                                    \
            "GIPSY(88_tmp%zu, 32_var%ld)\t# var\n",                                                 \
            assign_num_,                                                                            \
            var_num_                                                                                \
        )

#define IR_GIVE_ARG_(arg_num_, operand1_)                                                           \
        fprintf(                                                                                    \
            out,                                                                                    \
            "GIPSY(14_arg%zu, 88_tmp%zu)\t# give arg\n",                                            \
            arg_num_,                                                                               \
            operand1_                                                                               \
        )

#define IR_TAKE_ARG_(assign_num_, arg_num_)                                                         \
        fprintf(                                                                                    \
            out,                                                                                    \
            "GIPSY(32_var%ld, 14_arg%zu)\t# take arg\n",                                            \
            assign_num_,                                                                            \
            OP_TYPE_ARGS_COMMA,                                                                     \
            arg_num_                                                                                \
        )
        
#define IR_ASSIGN_(assign_num_, op_type_, operand1_, operand2_)                                     \
        fprintf(                                                                                    \
            out,                                                                                    \
            "GIPSY(88_tmp%zu, %d, 88_tmp%zu, 88_tmp%zu)\t# %s\n",                                   \
            assign_num_,                                                                            \
            op_type_,                                                                               \
            operand1_,                                                                              \
            operand2_,                                                                              \
            op_type_to_str(op_type_)                                                                \
        )

#define IR_ASSIGN_VAR_(assign_var_, op_type_, operand1_)                                            \
        fprintf(                                                                                    \
            out,                                                                                    \
            "GIPSY(32_var%ld, %d, 88_tmp%zu)\t# %s\n",                                              \
            assign_var_,                                                                            \
            op_type_,                                                                               \
            operand1_,                                                                              \
            op_type_to_str(op_type_)                                                                \
        )

#define IR_COND_JMP_(label_num_, cond_res_)                                                         \
        fprintf(                                                                                    \
            out,                                                                                    \
            "ENTER(label%zu, 88_tmp%zu)\t# cond jmp %s\n",                                          \
            label_num_,                                                                             \
            cond_res_                                                                               \
        )

#define IR_JMP_(label_num_)                                                                         \
        fprintf(                                                                                    \
            out,                                                                                    \
            "ENTER(label%zu)\t# not cond jmp\n",                                                    \
            label_num_                                                                              \
        )

#define IR_LABEL_(label_num_)                                                                       \
        fprintf(                                                                                    \
            out,                                                                                    \
            "FIFT(label%zu)\t# label\n",                                                            \
            label_num_                                                                              \
        )

#define IR_CALL_FUNC_(assign_num_, func_num_, cnt_args_)                                            \
        fprintf(                                                                                    \
            out,                                                                                    \
            "CALL_PENIS(88_tmp%zu, func_%zu_%zu, %zu)\t# call func\n",                              \
            assign_num_,                                                                            \
            func_num_,                                                                              \
            cnt_args_,                                                                              \
            cnt_args_                                                                               \
        )

#define IR_CALL_MAIN_(assign_num_)                                                                  \
        fprintf(                                                                                    \
            out,                                                                                    \
            "CALL_PENIS(88_tmp%zu, main, 0)\t# call main\n",                                        \
            assign_num_                                                                             \
        )

#define IR_IMPLEMENT_FUNC_(func_num_, cnt_args_)                                                    \
        fprintf(                                                                                    \
            out,                                                                                    \
            "\nPENIS(func_%zu_%zu, %zu)\t# implement func\n",                                       \
            func_num_,                                                                              \
            cnt_args_,                                                                              \
            cnt_args_                                                                               \
        )

#define IR_IMPLEMENT_MAIN_()                                                                        \
        fprintf(                                                                                    \
            out,                                                                                    \
            "\nPENIS(main, 0)\t# implement main\n"                                                  \
        )

#define IR_RET_(ret_val_)                                                                           \
        fprintf(                                                                                    \
            out,                                                                                    \
            "KILL_PENIS(88_tmp%zu)\t# ret\n",                                                       \
            ret_val_                                                                                \
        )

#endif /*PYAM_IR_DSL_WRITE_H*/