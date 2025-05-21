#include "my_stdio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../Logger/logging.h"
#include "../Assert/my_assert.h"

int my_strcmp (const char* const s1, const char* const s2)
{
    ASSERT (s1 != NULL, "Invalid pointer s1 for function %s\n", __FUNCTION__);
    ASSERT (s2 != NULL, "Invalid pointer s2 for function %s\n", __FUNCTION__);

    LOG (kDebug, "Function got arguments:\n"
                 "s1 = %s\ns2 = %s\n", s1, s2);

    size_t i = 0;
    while ((tolower (s1 [i]) - tolower (s2 [i])) == 0)
    {
        if (s1 [i] == '\0')
        {
            return 0;
        }
        i++;
    }

    return (tolower (s1 [i]) - tolower (s2 [i]));
}

size_t skip_space_symbols (const char* const string)
{
    ASSERT (string != NULL, "Invalid argument for [%s]: string = %p\n", __FUNCTION__, string);

    size_t new_offset = 0;
    char check = '\0';

    while ((sscanf (string + new_offset, "%c", &check) != EOF) && (isspace (string [new_offset])))
    {
        new_offset++;
    }

    return new_offset;
}

size_t size_of_file (FILE* const input)
{
    ASSERT (input != NULL, "Invalid argument input = %p\n", input);

    fseek (input, 0, SEEK_END);
    size_t size_code = (size_t) ftell (input);
    fseek (input, 0, SEEK_SET);

    return size_code;
}

void my_fflush (FILE* const input)
{
    ASSERT (input != NULL, "Invalid argument input = %p\n", input);

    while (getc (input) != EOF)
    {
        ;
    }
}

char* ReadFileToBuffer (FILE* const file)
{
    ASSERT (file != NULL, "Invalid argument file for ReadFileToBuffer\n");

    size_t file_size = size_of_file (file);

    char* buffer = (char*) calloc (file_size + 1, sizeof (char));
    if (buffer == NULL)
    {
        return NULL;
    }

    if (fread (buffer, sizeof (char), file_size, file) != file_size)
    {
        free (buffer);
        return NULL;
    }

    buffer [file_size] = '\0';

    return buffer;
}
