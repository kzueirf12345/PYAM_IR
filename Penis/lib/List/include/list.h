#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct order_list
{
    size_t next;

    size_t previous;
} order_list_t;

typedef struct list
{
    void* data;

    order_list_t* order;

    size_t counter;

    size_t size;

    size_t free;

    size_t elem_size;
} list_t;

enum ListError
{
    kDoneList                   = 0,

    kCantCtorList               = 1,
    kCantDtorList               = 2,
    kCantResizeList             = 3,

    kCantPushList               = 4,
    kCantPopList                = 5,

    kCantDumpList               = 6,

    kVerifyListNullPtr          = 7,
    kVerifyListInvalidZeroElem  = 8,
    kVerifyListInvalidSize      = 9,
    kVerifyListInvalidOrderList = 10,
    kVerifyListInvalidOrderFree = 11,
};

#endif // LIST_H
