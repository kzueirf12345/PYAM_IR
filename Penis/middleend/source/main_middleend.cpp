#include <stdlib.h>
#include <stdio.h>

#include "middleend.h"
#include "parse_flags_lang.h"
#include "error_handler.h"

#include "Logger/logging.h"

#define ERROR_HANDLER(error)                                                                            \
    if (error != kDoneIR)                                                                               \
    {                                                                                                   \
        fprintf (stderr, "Code error = {%d} with name \"%s\"\n",                                        \
                         error, EnumErrorToStr (error));                                                \
        SettingsDtor (&set);                                                                            \
        return EXIT_FAILURE;                                                                            \
    }

int main (const int argc, char* argv[])
{
    settings_of_program_t set = {};
    SettingsCtor (&set);
    ParseFlags (argc, argv, &set);
    if (set.stop_program)
    {
        SettingsDtor (&set);
        return EXIT_FAILURE;
    }

    set_log_file (set.stream_err);
    set_log_lvl (kDebug);

    enum IRError result = kDoneIR;

    result = OptimizeIR (set.stream_in, set.stream_out);
    ERROR_HANDLER (result);

    SettingsDtor (&set);

    return EXIT_SUCCESS;
}

#undef ERROR_HANDLER
