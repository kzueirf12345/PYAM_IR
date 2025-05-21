#ifndef LIST_CONSTRUCTION_H
#define LIST_CONSTRUCTION_H

#include "list.h"

#include <stdlib.h>

static const size_t kScaleList = 2;

enum ListError ListCtor   (list_t* const list, const size_t number_elem, const size_t elem_size);
enum ListError ListDtor   (list_t* const list);
enum ListError ListResize (list_t* const list, const bool flag_more);

enum ListError TryListResize (list_t* const list);

#endif // LIST_CONSTRUCTION_H
