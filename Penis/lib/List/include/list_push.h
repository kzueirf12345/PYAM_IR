#ifndef LIST_PUSH_H
#define LIST_PUSH_H

#include "list.h"

#include <stdlib.h>

enum ListError ListPushAfterIndex (list_t* const list, const void* const element, const size_t index);
enum ListError ListPushFront (list_t* const list, const void* const element);
enum ListError ListPushBack  (list_t* const list, const void* const element);

#endif // LIST_PUSH_H
