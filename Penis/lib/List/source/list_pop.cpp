#include "../include/list_pop.h"

#include <stdlib.h>
#include <string.h>

#include "../include/list.h"
#include "../include/list_construction.h"

#include "../include/My_lib/Assert/my_assert.h"
#include "../include/My_lib/Logger/logging.h"

enum ListError ListPopAfterIndex (list_t* const list, void* const element, size_t index)
{
    ASSERT (list    != NULL, "Invalid argument list [%p] for pop of index\n", list);
    ASSERT (element != NULL, "Invalid argument element  [%p] for pop of index\n", element);

    LOG (kDebug,
                "Pop of index of the list got argument:\n"
                "| list = %p | element = %p | index = %lu |\n"
                "| Next = %lu | Previous = %lu |\n",
                list, element, index,
                list->order [index].next, list->order [index].previous);

    if ((list->counter == 0) || (list->order [index].previous == (size_t) -1))
    {
        return kCantPopList;
    }

    list->counter--;

    memcpy (element, (char*) list->data + index * list->elem_size, list->elem_size);

    size_t previous_elem_ = list->order [index].previous;
    size_t next_elem_ = list->order [index].next;

    list->order [previous_elem_].next = next_elem_;
    list->order [next_elem_].previous = previous_elem_;
//
//     if (list->counter + 1 > index)
//     {
//         memcpy ((char*) list->data + index * list->elem_size,
//                 (char*) list->data + (list->counter + 1) * list->elem_size,
//                 list->elem_size);
//
//         list->order [index] = list->order [list->counter + 1];
//
//         list->order [list->order [index].previous].next = index;
//         list->order [list->order [index].next].previous = index;
//
//         index = list->counter + 1;
//     }

    memset ((char*) list->data + index * list->elem_size, 0, list->elem_size);
    list->order [index].next = list->free;
    list->order [index].previous = (size_t) -1;

    list->free = index;

    return kDoneList;
}

enum ListError ListPopFront (list_t* const list, void* const element)
{
    ASSERT (list    != NULL, "Invalid argument list [%p] for pop front\n", list);
    ASSERT (element != NULL, "Invalid argument element  [%p] for pop front\n", element);

    LOG (kDebug,
                "Pop front of the list got argument:\n"
                "| list = %p | element = %p |\n"
                "| Head = %lu | Tail = %lu |\n",
                list, element,
                list->order [0].previous, list->order [0].next);

    enum ListError result = ListPopAfterIndex (list, element, list->order [0].previous);

    return result;
}

enum ListError ListPopBack (list_t* const list, void* const element)
{
    ASSERT (list    != NULL, "Invalid argument list [%p] for pop back\n", list);
    ASSERT (element != NULL, "Invalid argument element  [%p] for pop back\n", element);

    LOG (kDebug,
                "Pop back of the list got argument:\n"
                "| list = %p | element = %p |\n"
                "| Head = %lu | Tail = %lu |\n",
                list, element,
                list->order [0].previous, list->order [0].next);

    enum ListError result = ListPopAfterIndex (list, element, list->order [0].next);

    return result;
}
