#ifndef MY_STDIO_H
#define MY_STDIO_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "../Logger/logging.h"

int    my_strcmp          (const char* const s1, const char* const s2);
size_t skip_space_symbols (const char* const string);
size_t size_of_file       (FILE* const input);
void   my_fflush          (FILE* const input);
char*  ReadFileToBuffer   (FILE* const file);

#define FOPEN(file, name, mode)                                                     \
{                                                                                   \
    file = fopen (name, mode);                                                      \
    LOG (kDebug, "File with name \"%s\" and pointer [%p] was opened\n", name, file);\
}

#define FCLOSE(file)                                            \
{                                                               \
    LOG (kDebug, "File with pointer [%p] was closed\n", file);  \
    fclose (file);                                              \
    file = NULL;                                                \
}

#endif // MY_STDIO_H
