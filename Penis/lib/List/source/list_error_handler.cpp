#include "../include/list_error_handler.h"
#include "../include/list.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/My_lib/Assert/my_assert.h"
#include "../include/My_lib/Logger/logging.h"
#include "../include/My_lib/helpful.h"

const char* EnumToStr (const enum ListError error)
{
    #define CASE_ENUM_(enum_elem)       \
        case enum_elem:                 \
        {                               \
            return #enum_elem;          \
        }

    switch (error)
    {
        CASE_ENUM_ (kDoneList);

        CASE_ENUM_ (kCantCtorList);
        CASE_ENUM_ (kCantDtorList);

        CASE_ENUM_ (kCantPushList);
        CASE_ENUM_ (kCantPopList);

        CASE_ENUM_ (kCantDumpList);

        CASE_ENUM_ (kVerifyListNullPtr);
        CASE_ENUM_ (kVerifyListInvalidZeroElem);
        CASE_ENUM_ (kVerifyListInvalidSize);
        CASE_ENUM_ (kVerifyListInvalidOrderList);
        CASE_ENUM_ (kVerifyListInvalidOrderFree);

        default:
            return "Undefined enum element";
    }

    return "Function didn't completed";

    #undef CASE_ENUM_
}

enum ListError VerifyList (const list_t* const list)
{
    if (list == NULL)
    {
        return kVerifyListNullPtr;
    }

    size_t zero = 0;

    if ((memcmp (list->data, &zero, list->elem_size) != 0)
        || ((list->order [0].next == 0) && (list->order [0].previous != 0))
        || ((list->order [0].next != 0) && (list->order [0].previous == 0))
        || ((list->order [0].next == 0) && (list->order [0].previous == 0) && (list->counter != 0)))
    {
        return kVerifyListInvalidZeroElem;
    }

    if (list->size < list->counter)
    {
        return kVerifyListInvalidSize;
    }

    size_t index_element = list->order [0].next;

    while (index_element != 0)
    {
        if ((list->order [index_element].previous == (size_t) - 1)
            || (list->order [index_element].next     != list->order [list->order [index_element].next].previous)
            || (list->order [index_element].previous != list->order [list->order [index_element].previous].next))
        {
            return kVerifyListInvalidOrderList;
        }

        index_element = list->order [index_element].next;
    }

    index_element = list->free;

    while (index_element != 0)
    {
        if (list->order [index_element].previous != (size_t) - 1)
        {
            return kVerifyListInvalidOrderFree;
        }

        index_element = list->order [index_element].next;
    }

    return kDoneList;
}

enum ListError ListDump (const list_t* const list, void PrintData (void* const data, FILE* const stream))
{
    static size_t counter_dump = 0;

    FILE* dump_file = fopen ("Dump_Files/Dump_file.dot", "w");
    if (dump_file == NULL)
    {
        return kCantDumpList;
    }

    FILE* html_dump_file = fopen ("Dump_Files/Dump.html", "a");
    if (html_dump_file == NULL)
    {
        CLOSE_AND_NULL (dump_file);
        return kCantDumpList;
    }

    if (ftell (html_dump_file) == 0)
    {
        fprintf (html_dump_file, "<pre>\n<HR>\n");
    }

    fprintf (dump_file, "digraph\n{\n"
    "\tfontname = \"Helvetica,Arial,sans-serif\";\n"
    "\tnode [fontname = \"Helvetica,Arial,sans-serif\"];\n"
    "\tgraph [rankdir = \"LR\"];\n"
	"\tranksep = 1.5;\n");

//------------------------------------------------------------------------------------------------------------

    fprintf (dump_file, "\n\t\"node-1\"\n\t[\n"
                        "\t\tlabel = \""
                        "<f0> free| "
                        "<f1> %lu\"\n"
                        "\t\tshape = \"record\"\n"
                        "\t\tcolor = \"%s\"\n"
                        "\t];\n",
                        list->free, kColorFree);

    fprintf (dump_file, "\n\t\"node0\"\n\t[\n"
                        "\t\tlabel = \""
                        "<f0> index = 0| "
                        "<f1> | "
                        "<f2> Tail = %lu| "
                        "<f3> Head = %lu\"\n"
                        "\t\tshape = \"record\"\n"
                        "\t\tcolor = \"%s\"\n"
                        "\t];\n\n",
                        list->order [0].next, list->order [0].previous, kColorNullElem);

//------------------------------------------------------------------------------------------------------------

    for (size_t index = 1; index < list->size; index++)
    {
        fprintf (dump_file, "\t\"node%lu\"\n\t[\n"
                            "\t\tlabel = \""
                            "<f0> index = %lu| "
                            "<f1> data = ",
                            index, index);

        PrintData ((char*) list->data + index * list->elem_size, dump_file);

        fprintf (dump_file, "| <f2> next = %lu| "
                            "<f3> previous = %lu\"\n"
                            "\t\tshape = \"record\"\n"
                            "\t];\n\n",
                            list->order [index].next, list->order [index].previous);
    }

//------------------------------------------------------------------------------------------------------------

    for (size_t index = 0; index < list->size - 1; index++)
    {
        fprintf (dump_file, "\t\"node%lu\":f0 -> \"node%lu\":f0 "
                            "[color = \"white\" weight = %lu];\n\n", index, index + 1, kWeightInvisEdge);
    }

    fprintf (dump_file, "\n");

    const char* color_edge_ = NULL;

    for (size_t index = 0; index < list->size; index++)
    {
        if (list->order [index].previous != (size_t) -1)
        {
            color_edge_ = kColorNext;

            fprintf (dump_file, "\t\"node%lu\":f3 -> \"node%lu\":f0 "
                                "[color = \"%s\" weight = %lu];\n\n",
                                index, list->order [index].previous,
                                kColorPrev, kWeightDefaultEdge);
        }
        else
        {
            color_edge_ = kColorFree;
        }

        fprintf (dump_file, "\t\"node%lu\":f2 -> \"node%lu\":f0 "
                            "[color = \"%s\" weight = %lu];\n\n",
                            index, list->order [index].next,
                            color_edge_, kWeightDefaultEdge);

    }

    fprintf (dump_file, "\n\t\"node-1\":f1 -> \"node%lu\":f0 [color = \"%s\" weight = %lu];\n",
                        list->free, kColorFree, kWeightFreeEdge);

    fprintf (dump_file, "}");

    CLOSE_AND_NULL (dump_file);

    char command [100] = "";

    sprintf (command, "dot -Tsvg Dump_Files/Dump_file.dot -o Dump_Files/Dump_file_%lu_.svg\n", counter_dump);

    if (system (command) == -1)
    {
        CLOSE_AND_NULL (html_dump_file);
        return kCantDumpList;
    }

    fprintf (html_dump_file, "<img src = \"Dump_file_%lu_.svg\" width = 2450>\n<HR>\n", counter_dump);

    counter_dump++;

    CLOSE_AND_NULL (html_dump_file);

    return kDoneList;
}
