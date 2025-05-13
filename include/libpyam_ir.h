#ifndef PYAM_IR_LIBPYAM_IR_H
#define PYAM_IR_LIBPYAM_IR_H

#include "key_words.h"
#include "dsl_write.h"
#include "operations.h"

#if defined (__cplusplus)
extern "C" {
#endif

const char* ir_op_type_to_str(const enum IrOpType type);

#if defined (__cplusplus)
}
#endif

#endif /*PYAM_IR_LIBPYAM_IR_H*/
