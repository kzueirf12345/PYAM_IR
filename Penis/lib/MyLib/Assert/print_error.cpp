#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "print_error.h"

void print_err (FILE* const stream, const char * const format, ...)
{
    assert (stream != NULL);
    assert (format != NULL);

    va_list err;
    va_start (err, format);
    vfprintf (stream, format, err);
    va_end (err);
}
