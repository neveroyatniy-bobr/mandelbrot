#include "cmd_args_handler.hpp"

#include <SFML/Graphics.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "screen_state.hpp"
#include "native.hpp"
#include "arrays.hpp"
#include "intrinsics.hpp"

static void PrintHelp (void);

void HandleCmdArgs(const int argc, char* const argv[], int (**UpdatePixels)(ScreenState, sfColor*)) {
    const struct option modifications [] = {
        {"version",      required_argument, 0, 'v'},
        {"help",               no_argument, 0, 'h'},
        {0,                              0, 0,  0 }
    };

    int mode = 0;
    int long_index = 0;
    int count_iterate = 0;

    while (mode != INVALID_MODE)
    {
        count_iterate++;
        mode = getopt_long (argc, argv, "+v:h", modifications, &long_index);
        switch (mode) {
            case 'v': {
                int mandelbrot_version_num = atoi (optarg);
                switch (mandelbrot_version_num) {
                    case 1: {
                        *UpdatePixels = NativeUpdatePixels;
                        break;
                    }
                    case 2: {
                        *UpdatePixels = ArraysUpdatePixels;
                        break;
                    }
                    case 3: {
                        *UpdatePixels = IntrinsicsUpdatePixels;
                        break;
                    }
                    default:
                        *UpdatePixels = IntrinsicsUpdatePixels;
                        break;
                }

                if (errno == ERANGE)
                {
                    fprintf(stderr, "Can't convert optarg to a mandelbrot version num.\n");
                    mode = INVALID_MODE;
                }
                count_iterate++;
                break;
            }
            case 'h': {
                PrintHelp();
                mode = INVALID_MODE;
                break;
            }
            case INVALID_MODE: {
                if (argc != count_iterate)
                {
                    fprintf (stderr, "There are odd symbols in the terminal.\n");
                    mode = INVALID_MODE;
                }
                break;
            }
            default:
            {
                fprintf (stderr, "Invalid name of command in the terminal.\n");
                mode = INVALID_MODE;
                break;
            }
        }
    }
}

static void PrintHelp (void) {
    fprintf (stdout, "----------------------------------------------------------------------------\n"
                     "| \"--verson\" or \"-v\" : You can change the version of mandelbrot            |\n"
                     "| \"--help\" or \"-h\"   : Write information about flags of that program.      |\n"
                     "----------------------------------------------------------------------------\n");
}