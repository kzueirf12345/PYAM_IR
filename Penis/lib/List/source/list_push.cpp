#include "../include/list_push.h"

#include <stdlib.h>
#include <string.h>

#include "../include/list.h"
#include "../include/list_construction.h"

#include "../include/My_lib/Assert/my_assert.h"
#include "../include/My_lib/Logger/logging.h"

enum ListError ListPushAfterIndex (list_t* const list, const void* const element, const size_t index)
{
    ASSERT (list    != NULL, "Invalid argument for list [%p] for push after index\n", list);
    ASSERT (element != NULL, "Invalid argument for element [%p] for push after index\n", element);

    LOG (kDebug,
                "Push after index of the list got argument:\n"
                "| list = %p | element = %p | index = %lu |\n"
                "| Next = %lu | Previous = %lu |\n",
                list, element, index,
                list->order [index].next, list->order [index].previous);

    if ((list->counter + 1 > list->size) || (list->order [index].previous == (size_t) -1))
    {
        return kCantPushList;
    }

    if ((list->counter > list->size * 3 / 4) || (list->free == list->order [list->free].next))
    {
        enum ListError error = ListResize (list, true);
        if (error != kDoneList)
        {
            return error;
        }
    }

    list->counter++;

    size_t new_element_index_ = list->free;

    list->free = list->order [list->free].next;

    memcpy ((char*) list->data  + new_element_index_ * list->elem_size, element, list->elem_size);
    list->order [new_element_index_].next = list->order [index].next;
    list->order [new_element_index_].previous = index;

    list->order [index].next = new_element_index_;
    list->order [list->order [new_element_index_].next].previous = new_element_index_;


    return kDoneList;
}

enum ListError ListPushFront (list_t* const list, const void* const element)
{
    ASSERT (list != NULL, "Invalid argument for list [%p] for push after head\n", list);

    LOG (kDebug,
                "Push after head of the list got argument:\n"
                "| list = %p | element = %lu |\n"
                "| Head = %lu | Tail = %lu |\n",
                list, element,
                list->order [0].previous, list->order [0].next);

    enum ListError result = ListPushAfterIndex (list, element, list->order [0].previous);

    return result;
}

enum ListError ListPushBack (list_t* const list, const void* const element)
{
    ASSERT (list != NULL, "Invalid argument for list [%p] for push before tail\n", list);

    LOG (kDebug,
                "Push before tail of the list got argument:\n"
                "| list = %p | element = %lu |\n"
                "| Head = %lu | Tail = %lu |\n",
                list, element,
                list->order [0].previous, list->order [0].next);

    enum ListError result = ListPushAfterIndex (list, element, 0);

    return result;
}

