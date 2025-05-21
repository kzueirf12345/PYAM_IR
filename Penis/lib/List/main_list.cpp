#include "include/list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "My_lib/Logger/logging.h"
#include "My_lib/Assert/my_assert.h"

void PrintData (void* const element, FILE* const output);

int main ()
{
    FILE* const error_file = fopen ("My_lib/Logger/error.txt", "w");
    if (error_file == NULL)
    {
        fprintf (stderr, "Can't start logging\n");
        return EXIT_FAILURE;
    }
    set_log_file (error_file);
    set_log_lvl (DEBUG);

    list_t list = {};

    ListError result = kDoneList;

    size_t element = 100;

    result = ListCtor (&list, 10, sizeof (element));

    ERROR_HANDLER (result);

    result = ListPushFront (&list, &element);

    ERROR_HANDLER (result);

    element = 777;

    result = ListPushBack (&list, &element);

    ERROR_HANDLER (result);

    result = ListPopAfterIndex (&list, &element, 1);

    ERROR_HANDLER (result);

    fprintf (stderr, "Element = %lu\n", element);

    result = ListDump (&list, PrintData);

    ERROR_HANDLER (result);

    element = 100;

    result = ListPushFront (&list, &element);

    ERROR_HANDLER (result);

    result = ListPushFront (&list, &element);

    ERROR_HANDLER (result);

    result = ListPushFront (&list, &element);

    ERROR_HANDLER (result);

    result = ListDump (&list, PrintData);

    ERROR_HANDLER (result);

    result = ListPopAfterIndex (&list, &element, 2);

    ERROR_HANDLER (result);

    fprintf (stderr, "Element = %lu\n", element);

    result = ListDtor (&list);

    ERROR_HANDLER (result);

    fclose (error_file);

    return EXIT_SUCCESS;
}

void PrintData (void* const element, FILE* const output)
{
    ASSERT (element != NULL, "Invalid argument element = %p\n", element);
    ASSERT (output  != NULL, "Invalid argument output = %p\n", output);

    fprintf (output, "%lu", *((size_t*) element));

    return;
}

#undef ERROR_HANDLER
