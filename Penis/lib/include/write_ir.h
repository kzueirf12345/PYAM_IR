#ifndef WRITE_IR_H
#define WRITE_IR_H

#include <stdio.h>

#include "../../middleend/include/middleend.h"
#include "list.h"

enum IRError WriteIRPYAM (const list_t* const IR_list, FILE* const ir_file);

#endif // WRITE_IR_H
