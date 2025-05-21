#ifndef LIST_POP_H
#define LIST_POP_H

#include "list.h"

#include <stdlib.h>

enum ListError ListPopAfterIndex (list_t* const list, void* const element, size_t index);
enum ListError ListPopFront (list_t* const list, void* const element);
enum ListError ListPopBack  (list_t* const list, void* const element);

#endif // LIST_POP_H
