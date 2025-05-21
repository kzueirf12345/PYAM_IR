#include "parse_flags_lang.h"

#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "Assert/my_assert.h"
#include "helpful.h"

static void PrintHelp (void);

void ParseFlags (const int argc, char* const argv[], settings_of_program_t* const set)
{
    ASSERT (argv != NULL, "Invalid argument: argv = %p\n", argv);
    ASSERT (set  != NULL, "Invalid argument: set = %p\n",  set);

    const struct option modifications [] =
    {
        {"input",    0, 0, 'i'},
        {"output",   0, 0, 'o'},
        {"log-file", 0, 0, 'l'},
        {"mode",     0, 0, 'm'},
        {"help",     0, 0, 'h'},
        {0,          0, 0,  0 }
    };

    int mode = 0;
    int long_index = 0;
    int count_iterate = 0;

    while (mode != -1)
    {
        count_iterate++;
        mode = getopt_long (argc, argv, "+i:o:l:m:h", modifications, &long_index);
        switch (mode)
        {
            case 'i':
            {
                set->stream_in = fopen (optarg, "r");
                if (!(set->stream_in))
                {
                    mode = -1;
                    set->stop_program = true;
                    fprintf (stderr, "There is no files with name %s\n", optarg);
                }
                set->input_file_name = optarg;
                count_iterate++;
                break;
            }
            case 'o':
            {
                set->stream_out = fopen (optarg, "w");
                if (!(set->stream_out))
                {
                    mode = -1;
                    set->stop_program = true;
                    fprintf (stderr, "There is no files with name %s\n", optarg);
                }
                set->output_file_name = optarg;
                count_iterate++;
                break;
            }
            case 'l':
            {
                set->stream_err = fopen (optarg, "w");
                if (!(set->stream_err))
                {
                    mode = -1;
                    set->stop_program = true;
                    fprintf (stderr, "There is no files with name %s\n", optarg);
                }
                count_iterate++;
                break;
            }
            case 'h':
            {
                PrintHelp ();
                mode = -1;
                set->stop_program = true;
                break;
            }
            case -1:
            {
                if (argc != count_iterate)
                {
                    fprintf (stderr, "There are odd symbols in the terminal.\n");
                    mode = -1;
                    set->stop_program = true;
                }
                break;
            }
            default:
            {
                fprintf (stderr, "Invalid name of command in the terminal.\n");
                mode = -1;
                set->stop_program = true;
                break;
            }
        }
    }
}

void SettingsCtor (settings_of_program_t* const set)
{
    ASSERT (set != NULL, "Invalid argument: set = %p\n", set);

    set->stop_program = false;

    set->stream_in  = stdin;
    set->input_file_name = NULL;
    set->stream_out = stdout;
    set->output_file_name = NULL;
    set->stream_err = stderr;
}

void SettingsDtor (settings_of_program_t* const set)
{
    ASSERT (set != NULL, "Invalid argument: set = %p\n", set);

    if (set->stream_in != stdin)
    {
        CLOSE_AND_NULL (set->stream_in);
    }
    set->input_file_name = NULL;

    if (set->stream_out != stdout)
    {
        CLOSE_AND_NULL (set->stream_out);
    }
    set->output_file_name = NULL;

    if (set->stream_err  != stderr)
    {
        CLOSE_AND_NULL (set->stream_err);
    }

    set->stop_program = true;
}

static void PrintHelp (void)
{
    fprintf (stdout, "------------------------------------------------------------------------------------------\n"
                     "| There is the list of parameters of that program:                                       |\n"
                     "| \"--input\" or \"-i\"                : You can change the stream of input                  |\n"
                     "|                                    by writing the name of file of input after that.    |\n"
                     "| \"--output\" or \"-o\"               : You can change the stream of output                 |\n"
                     "|                                    by writing the name of file for output after that.  |\n"
                     "| \"--log-file\" or \"-l\"             : You can change the stream of logging information    |\n"
                     "|                                    by writing the name of file for output after that.  |\n"
                     "| \"--help\" or \"-h\"                 : Write information about flags of that program.      |\n"
                     "------------------------------------------------------------------------------------------\n");
}

