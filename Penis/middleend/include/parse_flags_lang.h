#ifndef PARSE_FLAGS_LANG_H
#define PARSE_FLAGS_LANG_H

#include <stdio.h>


typedef struct settings_of_program
{
    FILE* stream_in;
    FILE* stream_out;
    FILE* stream_err;

    const char* input_file_name;
    const char* output_file_name;

    bool stop_program;
} settings_of_program_t;

void SettingsCtor (settings_of_program_t* const set);
void SettingsDtor (settings_of_program_t* const set);
void ParseFlags   (const int argc, char* const argv[], settings_of_program_t* const set);

#endif // PARSE_FLAGS_LANG_H
