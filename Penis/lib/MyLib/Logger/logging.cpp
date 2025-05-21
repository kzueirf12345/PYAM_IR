#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "logging.h"
#include "../Assert/my_assert.h"

static enum LevelLog logging_lvl   = kWarning;
static FILE* logging_stream        = stderr;
static const int year_shift        = 1900;
static const int month_shift       = 1;

static void level_to_str (const enum LevelLog level);

static void level_to_str (const enum LevelLog level)
{
    switch (level)
    {
        case kDebug:
        {
            fprintf (logging_stream, "[%s] \n", "DEBUG");
            break;
        }
        case kInfo:
        {
            fprintf (logging_stream, "[%s] \n", "INFO");
            break;
        }
        case kWarning:
        {
            fprintf (logging_stream, "[%s] \n", "WARNING");
            break;
        }
        case kError:
        {
            fprintf (logging_stream, "[%s] \n", "ERROR");
            break;
        }
        default:
        {
            ASSERT(0, "Program got wrong level of logging in function log\n");
            break;
        }
    }
}


void Log (const struct logging settings, const enum LevelLog level, const char * const format, ...)
{
    ASSERT(format != NULL, "Invalid argument for Log %p\n", format);

    if (level < logging_lvl)
    {
        return;
    }

    level_to_str (level);
    fprintf (logging_stream, "%s:%d (%s) %d sec %d min %d hours %d days %d month %d year\n",
             settings.file, settings.line, settings.func,
             settings.now->tm_sec, settings.now->tm_min,
             settings.now->tm_hour, settings.now->tm_mday,
             settings.now->tm_mon + month_shift, settings.now->tm_year + year_shift);

    va_list param;
    va_start (param, format);
    vfprintf (logging_stream, format, param);
    va_end (param);

    fprintf (logging_stream, "\n\n");
}

void set_log_lvl (const enum LevelLog level)
{
    logging_lvl = level;
}

void set_log_file (FILE* const file)
{
    if (file != NULL)
    {
        logging_stream = file;
        setbuf (file, NULL);
    }
}
