/**
 * @file option_handler.c
 * @brief Option Handling for the Server Application
 *
 * This file contains functions for processing command-line options and printing
 * the help menu. It is responsible for setting up initial configurations based
 * on the options passed to the application.
 */
#define _GNU_SOURCE // for strnlen()

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // getopt

#include "number_converter.h"
#include "option_handler.h"
#include "utilities.h"

#define MIN_NUM_THREADS 2     // Minimum number of threads allowed
#define MAX_PORT_VALUE  65535 // Maximum allowable port number
#define MIN_PORT_VALUE  1025  // Minimum allowable port number

//
// -----------------------------UTILITY FUNCTIONS-----------------------------
//

/**
 * @brief Prints the help menu to the console.
 *
 * The function provides information on how to use the server application,
 * detailing available options and their usage.
 */
static void print_help_menu();

//
// ------------------------------REPORT FUNCTIONS------------------------------
//

/**
 * @brief Report any additional arguments passed to the program.
 *
 * This function is responsible for checking if there are any extra arguments
 * that `getopt` didn't process. If there are, it reports them to the stderr.
 *
 * @param optind Index of the next element to be processed in argv.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 *
 * @return E_SUCCESS if no additional invalid arguments are found, E_FAILURE
 * otherwise.
 */
static int report_extra_arguments(int optind, int argc, char ** argv);

/**
 * @brief Reports invalid or incomplete options to stderr.
 *
 * This function is called when an unknown option or an option requiring an
 * argument (like `-n` or `-p`) is passed without one. It prints an appropriate
 * error message to stderr.
 */
static void report_invalid_options();

//
// ------------------------------OPTION FUNCTIONS------------------------------
//

/**
 * @brief Process the '-n' command-line option.
 *
 * This function is responsible for processing the '-n' option passed in the
 * command line. The '-n' option specifies the number of threads in the thread
 * pool.
 *
 * @param optarg Pointer to the string containing the argument for the '-n'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_n_option(char * optarg, options_t * options_p);

/**
 * @brief Process the '-p' command-line option.
 *
 * This function is responsible for processing the '-p' option passed in the
 * command line. The '-p' option specifies the port number on which the server
 * should listen.
 *
 * @param optarg Pointer to the string containing the argument for the '-p'
 * option.
 * @param options_p Pointer to the options_t struct where the parsed value
 * should be stored.
 *
 * @return E_SUCCESS on successful processing, E_FAILURE otherwise.
 */
static int process_p_option(char * optarg, options_t * options_p);

// +---------------------------------------------------------------------------+
// |                            MAIN OPTION HANDLER                            |
// +---------------------------------------------------------------------------+

int process_options(int argc, char ** argv, options_t * options_p)
{
    int exit_code = E_FAILURE;
    int option    = 0;

    if ((NULL == argv) || (NULL == *argv) || (NULL == options_p))
    {
        print_error("process_options(): NULL argument passed.");
        goto END;
    }

    // Generally assignments inside conditional statements go against Barr-C and
    // CSD-T coding standards, however the following conditional was pulled
    // directly from the 'getopt()' man page, and follows a more idomatic
    // approach that is more recognizable to C programmers who are familiar with
    // 'getopt()'.
    while (-1 != (option = getopt(argc, argv, "n:p:h")))
    {
        switch (option)
        {
            case 'n':
                exit_code = process_n_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    print_error("Unable to process 'n' option.");
                    goto END;
                }
                break;

            case 'p':
                exit_code = process_p_option(optarg, options_p);
                if (E_SUCCESS != exit_code)
                {
                    print_error("Unable to process 'p' option.");
                    goto END;
                }
                break;

            case 'h':
                goto END;
                break;

            case '?':
                report_invalid_options();
                exit_code = E_FAILURE;
                goto END;
                break;

            default:
                fprintf(stderr, "Unknown option encountered.\n");
                exit_code = E_FAILURE;
                goto END;
        }
    }

    exit_code = report_extra_arguments(optind, argc, argv);
    if (E_SUCCESS != exit_code)
    {
        print_error("Invalid arguments passed.");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    if (E_FAILURE == exit_code)
    {
        print_help_menu();
    }
    return exit_code;
}

// *****************************************************************************
//                          STATIC FUNCTION DEFINITIONS
// *****************************************************************************

static int process_n_option(char * optarg, options_t * options_p)
{
    int      exit_code     = E_FAILURE;
    number_t num_threads_p = { 0 };

    if ((NULL == optarg) || (NULL == options_p))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Check if the 'n' flag was already set
    if (true == options_p->n_flag)
    {
        print_error("process_options(): '-n' flag already true.");
        goto END;
    }

    // Convert string to integer
    exit_code = str_to_int32(optarg, &num_threads_p);
    if (E_SUCCESS != exit_code)
    {
        print_error("Unable to convert 'n_value' to number.");

        goto END;
    }

    if (MIN_NUM_THREADS > num_threads_p.signed_num)
    {
        print_error("process_options(): Number of threads must be 2 or more.");

        exit_code = E_FAILURE;
        goto END;
    }

    options_p->n_flag  = true;
    options_p->n_value = num_threads_p.signed_num;

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static int process_p_option(char * optarg, options_t * options_p)
{
    int      exit_code     = E_FAILURE;
    number_t port_number_p = { 0 };
    size_t   optarg_length = 0;

    if ((NULL == optarg) || (NULL == options_p))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    // Check if the 'p' flag was already set
    if (true == options_p->p_flag)
    {
        print_error("process_p_option(): '-p' flag already true.");
        goto END;
    }

    // Convert string to integer and check range
    exit_code = str_to_int32(optarg, &port_number_p);
    if (E_SUCCESS != exit_code)
    {
        print_error(
            "process_p_option(): Unable to convert 'p_value' to number.");
        goto END;
    }
    if ((MAX_PORT_VALUE < port_number_p.signed_num) ||
        (MIN_PORT_VALUE > port_number_p.signed_num))
    {
        print_error("process_p_option(): Port number out of range.");
        exit_code = E_FAILURE;
        goto END;
    }

    // Calculate string length
    optarg_length = strnlen(optarg, MAX_PORT_SIZE);
    if (MAX_PORT_SIZE < optarg_length)
    {
        print_error("process_p_option(): Port string is too long.");
        goto END;
    }

    options_p->p_flag  = true;
    options_p->p_value = optarg;

    exit_code = E_SUCCESS;

END:
    return exit_code;
}

static void report_invalid_options()
{
    if (optopt == 'n' || optopt == 'p')
    {
        fprintf(stderr, "Option '-%c' requires an argument.\n", optopt);
    }
    else
    {
        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
    }
}

static int report_extra_arguments(int optind, int argc, char ** argv)
{
    int exit_code = E_FAILURE;

    if ((NULL == argv) || (NULL == *argv))
    {
        print_error("NULL argument passed.");
        goto END;
    }

    if (optind < argc)
    {
        fprintf(stderr, "Invalid arguments encountered: ");
        while (optind < argc)
        {
            fprintf(stderr, "%s ", argv[optind++]);
        }
        fprintf(stderr, "\n");
        goto END;
    }

    exit_code = E_SUCCESS;
END:
    return exit_code;
}

static void print_help_menu()
{
    printf("Net Calc - Cyber Solutions Development - Tactical\n");
    printf("-------------------------------------------------\n");
    printf("Usage: ./netcalc [options]\n");
    printf("Options:\n");
    printf(
        "  -p PORT   Port to listen on; (MIN: 1025, MAX: 65535) defaults to "
        "31337.\n");
    printf(
        "  -n NUM    Number of threads in the pool; (MIN: 2) defaults to 4.\n");
    printf("  -h        Print this help menu and exit.\n");
    printf("\n");
    printf("Description:\n");
    printf(
        "  Net Calc is a server application that performs a variety of "
        "operations.\n");
    printf(
        "  It listens for incoming connections over network sockets, enqueues "
        "the data,\n");
    printf("  and processes the work in a queue with a threadpool.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  netcalc -p 8080 -n 8\n");
    printf("  netcalc -h\n");
    printf("\n");
    printf("For more information, see the documentation.\n");
}

/*** end of file ***/
