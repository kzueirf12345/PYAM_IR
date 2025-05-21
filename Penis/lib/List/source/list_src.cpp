#include "../include/list_src.h"
#include "../include/list.h"

#include <stdlib.h>
#include <memory.h>

#include "../include/My_lib/Assert/my_assert.h"

size_t NextIndex (const list_t* const list, const size_t index)
{
    ASSERT (list != NULL, "Invalid argument list for NextIndex [%p]\n", list);

    return list->order [index].next;
}

size_t PrevIndex (const list_t* const list, const size_t index)
{
    ASSERT (list != NULL, "Invalid argument list for PrevIndex [%p]\n", list);

    return list->order [index].previous;
}

size_t HeadIndex (const list_t* const list)
{
    ASSERT (list != NULL, "Invalid argument list for HeadIndex [%p]\n", list);

    return list->order [0].previous;
}

size_t TailIndex (const list_t* const list)
{
    ASSERT (list != NULL, "Invalid argument list for TailIndex [%p]\n", list);

    return list->order [0].next;
}

enum ListError ListElemValLoad (const list_t* const list, const size_t index, void* const element)
{
    ASSERT (list    != NULL, "Invalid argument list for ListElemVal [%p]\n",    list   );
    ASSERT (element != NULL, "Invalid argument element for ListElemVal [%p]\n", element);

    memcpy (element, (char*) list->data + index * list->elem_size, list->elem_size);

    return kDoneList;
}

enum ListError ListElemValStor (const list_t* const list, const size_t index, void* const element)
{
    ASSERT (list    != NULL, "Invalid argument list for ListElemVal [%p]\n",    list   );
    ASSERT (element != NULL, "Invalid argument element for ListElemVal [%p]\n", element);

    memcpy ((char*) list->data + index * list->elem_size, element, list->elem_size);

    return kDoneList;
}
