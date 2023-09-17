/**
 * @file option_handler.h
 * @brief Header for Option Handling Functions
 *
 * This header file provides the interface for processing command-line options
 * and creating a structure to store these options for NetCalc.
 *
 */
#ifndef _OPTION_HANDLER_H
#define _OPTION_HANDLER_H

#include <stdbool.h>

#define MAX_PORT_SIZE 6 // Maximum size (in characters) for the port string

/**
 * @struct options
 * @brief Structure to store command-line options.
 *
 * This structure is used to store the options passed via the command-line
 * arguments. It keeps track of whether certain flags are activated and stores
 * their corresponding values if they are provided.
 */
typedef struct options
{
    bool    n_flag;  // Used to set the truth value for the n flag
    int32_t n_value; // Set the value of 'n'
    bool    p_flag;  // Used to set the truth value for the p flag
    char *  p_value; // Stores the port value as a string
} options_t;

/**
 * @brief Processes command-line options and populates an options_t structure.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array containing the command-line arguments.
 * @param options The address of a pointer to an options_t structure
 * where the options will be stored.
 * @return int - Returns E_SUCCESS on successful processing, otherwise
 * E_FAILURE.
 */
int process_options(int argc, char ** argv, options_t * options_p);

#endif /* _OPTION_HANDLER_H */
/*** end of file ***/
