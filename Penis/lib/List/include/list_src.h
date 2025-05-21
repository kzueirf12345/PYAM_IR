#ifndef LIST_SRC_H
#define LIST_SRC_H

#include "list.h"

#include <stdlib.h>

size_t NextIndex (const list_t* const list, const size_t index);
size_t PrevIndex (const list_t* const list, const size_t index);
size_t HeadIndex (const list_t* const list);
size_t TailIndex (const list_t* const list);

enum ListError ListElemValLoad (const list_t* const list, const size_t index, void* const element);
enum ListError ListElemValStor (const list_t* const list, const size_t index, void* const element);

#endif // LIST_SRC_H
