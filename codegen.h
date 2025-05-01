/*eslint-disable*/

                 // num, name,           surname,    assign_val,      label, operation, operand1,              operand2
IR_OPERATION_HANDLE(1,   CALL_FUNC,      CALL_PENIS, TMP,             LABEL, NONE,      NUM,                   NONE      )
IR_OPERATION_HANDLE(2,   IMPLEMENT_FUNC, PENIS,      NONE,            LABEL, NONE,      NUM,                   NONE      )
IR_OPERATION_HANDLE(3,   CMP_JMP,        ENTER,      NONE,            LABEL, NONE,      NONE | TMP,            NONE      )
IR_OPERATION_HANDLE(4,   ASSIGN,         GIPSY,      TMP | VAR | ARG, NONE,  NONE | OP, NUM | TMP | VAR | ARG, NONE | TMP)
IR_OPERATION_HANDLE(5,   RET,            KILL_PENIS, NONE,            NONE,  NONE,      TMP,                   NONE      )
IR_OPERATION_HANDLE(6,   LABEL,          FIFT,       NONE,            LABEL, NONE,      NONE,                  NONE      )
IR_OPERATION_HANDLE(7,   SYS_FUNC,       NOTHING,    NONE,            NONE,  OP,        NONE | TMP,            NONE | TMP)