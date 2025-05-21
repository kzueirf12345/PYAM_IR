#ifndef LIST_ERROR_HANDLER_H
#define LIST_ERROR_HANDLER_H

#include <stdio.h>

#include "list.h"

static const char* const kColorNullElem = "yellow";
static const char* const kColorFree     = "green";
static const char* const kColorNext     = "blue";
static const char* const kColorPrev     = "red";
static const size_t kWeightInvisEdge    = 100;
static const size_t kWeightDefaultEdge  = 10;
static const size_t kWeightFreeEdge     = 10;

#define ERROR_HANDLER(result)                                   \
    if (result != kDoneList)                                    \
    {                                                           \
        const char* error = EnumToStr (result);                 \
        fprintf (stderr, " Code error = {%d} \n", result);      \
        fprintf (stderr, " Error is %s \n", error);             \
        fclose (error_file);                                    \
        ListDtor (&list);                                       \
        return EXIT_FAILURE;                                    \
    }

const char* EnumToStr (const enum ListError error);
enum ListError VerifyList (const list_t* const list);
enum ListError ListDump (const list_t* const list, void PrintData (void* const data, FILE* const stream));

#endif // LIST_ERROR_HANDLER_h
