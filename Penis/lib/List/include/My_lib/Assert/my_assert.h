#ifndef MY_ASSERT_H
#define MY_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#include "../Assert/print_error.h"
#include "../Logger/logging.h"

#ifdef DEBUG
#define ASSERT(comparison, ...)                                                     \
    do                                                                              \
    {                                                                               \
        if ((comparison) == 0)                                                      \
        {                                                                           \
            fprintf (stderr,"=================ASSERT=GOT=YOU=================\n"    \
                            "%s:%d (%s)\n", __FILE__, __LINE__, __FUNCTION__);      \
            print_err (stderr, __VA_ARGS__);                                        \
            fprintf (stderr,"================================================\n");  \
            abort();                                                                \
        };                                                                          \
    }                                                                               \
    while (0)
#else
#define ASSERT(...)
#endif // DEBUG

#endif // MY_ASSERT_H
