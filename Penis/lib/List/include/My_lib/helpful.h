#ifndef HELPFUL_H
#define HELPFUL_H

#define CLOSE_AND_NULL(file)                        \
    if ((file) != NULL)                             \
    {                                               \
        fclose (file);                              \
        file = NULL;                                \
    }

#define FREE_AND_NULL(dynamic_ptr)                  \
    if ((dynamic_ptr) != NULL)                      \
    {                                               \
        free (dynamic_ptr);                         \
        dynamic_ptr = NULL;                         \
    }

#endif // HELPFUL_H
