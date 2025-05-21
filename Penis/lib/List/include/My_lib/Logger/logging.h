#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <time.h>

#if (!(defined(NDEBUG)) && (defined(DEBUG)))
#define LOG(level, ...)                                                                                  \
    do                                                                                                   \
    {                                                                                                    \
        time_t seconds = time (NULL);                                                                    \
        Log ({__FILE__, __func__, __LINE__, localtime(&seconds)}, level, __VA_ARGS__);                   \
    } while (0)
#else
#define LOG(...)
#endif

enum LevelLog
{
    kDebug   = 1,
    kInfo    = 2,
    kWarning = 3,
    kError   = 4,
};

struct logging
{
    const char* const file;

    const char* const func;

    int line;

    struct tm *now;
};

void set_log_lvl (const enum LevelLog level);
void set_log_file (FILE* const file);
void Log (const struct logging parameter, const enum LevelLog level, const char * const format, ...);

#endif // LOGGING_H
