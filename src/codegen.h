/*eslint-disable*/

#define OP_T_(type) IR_OPERAND_TYPE_##type
                 // num, name,           surname,    assign_val,                           label,        operation,                      operand1,                                           operand2
IR_OPERATION_HANDLE(1,   CALL_FUNC,      CALL_PENIS, OP_T_(TMP),                           OP_T_(LABEL), OP_T_(NONE),                    OP_T_(NUM),                                         OP_T_(NONE)             )
IR_OPERATION_HANDLE(2,   IMPLEMENT_FUNC, PENIS,      OP_T_(NONE),                          OP_T_(LABEL), OP_T_(NONE),                    OP_T_(NUM),                                         OP_T_(NONE)             )
IR_OPERATION_HANDLE(3,   CMP_JMP,        ENTER,      OP_T_(NONE),                          OP_T_(LABEL), OP_T_(NONE),                    OP_T_(NONE) | OP_T_(TMP),                           OP_T_(NONE)             )
IR_OPERATION_HANDLE(4,   ASSIGN,         GIPSY,      OP_T_(TMP) | OP_T_(VAR) | OP_T_(ARG), OP_T_(NONE),  OP_T_(NONE) | OP_T_(OPERATION), OP_T_(NUM)  | OP_T_(TMP) | OP_T_(VAR) | OP_T_(ARG), OP_T_(NONE) | OP_T_(TMP))
IR_OPERATION_HANDLE(5,   RET,            KILL_PENIS, OP_T_(NONE),                          OP_T_(NONE),  OP_T_(NONE),                    OP_T_(TMP),                                         OP_T_(NONE)             )
IR_OPERATION_HANDLE(6,   LABEL,          FIFT,       OP_T_(NONE),                          OP_T_(LABEL), OP_T_(NONE),                    OP_T_(NONE),                                        OP_T_(NONE)             )
IR_OPERATION_HANDLE(7,   SYS_FUNC,       NOTHING,    OP_T_(NONE),                          OP_T_(NONE),  OP_T_(OPERATION),               OP_T_(NONE) | OP_T_(TMP),                           OP_T_(NONE) | OP_T_(TMP))

#undef OP_T_

/*eslint-disable*/