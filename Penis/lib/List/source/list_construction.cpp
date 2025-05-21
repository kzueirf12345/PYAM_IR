#include "../include/list_construction.h"

#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>
#include <immintrin.h>

#include "../include/list.h"
#include "../include/list_src.h"

#include "../include/My_lib/Assert/my_assert.h"
#include "../include/My_lib/Logger/logging.h"
#include "../include/My_lib/helpful.h"

enum ListError ListCtor (list_t* const list, const size_t number_elem, const size_t elem_size)
{
    ASSERT (list != NULL, "Invalid argument for list [%p] for constructor\n", list);

    LOG (kDebug,
                "Constructor of the list got argument:\n"
                "| list = %p |\n", list);

    list->counter = 0;

    list->size = number_elem - (number_elem % sizeof (__m256)) + sizeof (__m256) - 1;

    list->elem_size = elem_size;

    list->free = 1;

    if (list->data == NULL)
    {
        list->data = aligned_alloc (sizeof (__m256), (list->size + 1) * list->elem_size);
        if (list->data == NULL)
        {
            return kCantCtorList;
        }
    }

    if (list->order == NULL)
    {
        list->order = (order_list_t*) calloc (list->size + 1, sizeof (order_list_t));
        if (list->order == NULL)
        {
            FREE_AND_NULL (list->data);
            return kCantCtorList;
        }
    }

    memset (list->data, 0, list->elem_size * list->size);

    for (size_t index_ctor_ = 1; index_ctor_ < number_elem; index_ctor_++)
    {
        list->order [index_ctor_].next = index_ctor_ + 1;
        list->order [index_ctor_].previous = (size_t) -1;
    }

    list->order [0].next = 0;
    list->order [0].previous = 0;
    list->order [number_elem - 1].next = number_elem - 1;

    return kDoneList;
}

enum ListError ListDtor (list_t* const list)
{
    ASSERT (list != NULL, "Invalid argument for list [%p] for destructor\n", list);

    LOG (kDebug,
                "Destructor of the list got argument:\n"
                "| list = %p |\n",
                list);

    FREE_AND_NULL (list->order);
    FREE_AND_NULL (list->data);

    list->counter = 0;
    list->size = 0;
    list->elem_size = 0;
    list->free = 0;

    return kDoneList;
}

enum ListError ListResize (list_t* const list, const bool flag_more)
{
    ASSERT (list != NULL, "Invalid argument for list [%p] for destructor\n", list);

    size_t new_size = (flag_more) ? list->size * kScaleList : list->size / kScaleList;

    new_size = new_size - (new_size % sizeof (__m256)) + sizeof (__m256) - 1;

    void* const new_data = aligned_alloc (sizeof (__m256), (new_size + 1) * list->elem_size);
    if (new_data == NULL)
    {
        return kCantResizeList;
    }

    order_list_t* const new_order = (order_list_t*) calloc (new_size + 1, sizeof (order_list_t));
    if (new_order == NULL)
    {
        free (new_data);
        return kCantResizeList;
    }

    size_t list_elem_index = TailIndex (list);
    size_t new_index = 1;

    while (list_elem_index != 0)
    {
        new_order [new_index] = {.next = new_index + 1, .previous = new_index - 1};
        memcpy ((char*) new_data   + new_index       * list->elem_size,
                (char*) list->data + list_elem_index * list->elem_size, list->elem_size);
        list_elem_index = NextIndex (list, list_elem_index);
        new_index++;
    }

    new_order [0] = {.next = 1, .previous = list->counter};
    new_order [new_index - 1].next = 0;

    for (size_t free_index = list->counter + 1; free_index < new_size; free_index++)
    {
        new_order [free_index].previous = (size_t) -1;
        new_order [free_index].next = (free_index + 1 < new_size) ? free_index + 1 : free_index;
    }

    list->free = list->counter + 1;

    FREE_AND_NULL (list->data);
    FREE_AND_NULL (list->order);

    list->size = new_size;
    list->data = new_data;
    list->order = new_order;

    return kDoneList;
}

enum ListError TryListResize (list_t* const list)
{
    ASSERT (list != NULL, "Invalid argument for list [%p] for destructor\n", list);

    if ((list->counter < list->size / 4) || (list->free == list->order [list->free].next))
    {
        enum ListError error = ListResize (list, false);
        if (error != kDoneList)
        {
            return error;
        }
    }

    if ((list->counter > list->size * 3 / 4) || (list->free == list->order [list->free].next))
    {
        enum ListError error = ListResize (list, true);
        if (error != kDoneList)
        {
            return error;
        }
    }

    return kDoneList;
}
