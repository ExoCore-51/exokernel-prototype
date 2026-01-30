/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - COMMAND LINE INTERFACE
 * =============================================================================
 * 
 * Interactive CLI for testing kernel and LibOS operations.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef CLI_H
#define CLI_H

#include "kernel.h"
#include "libos.h"

/* Maximum command line length */
#define CLI_MAX_LINE 256

/* Maximum number of arguments */
#define CLI_MAX_ARGS 10

/*
 * Initialize the CLI
 * Must be called after kernel_init()
 */
void cli_init(void);

/*
 * Run the interactive CLI loop
 * Blocks until user types 'exit'
 */
void cli_run(void);

/*
 * Process a single command
 * @param line: The command line to process
 * @return: 0 to continue, 1 to exit
 */
int cli_process_command(const char* line);

/*
 * Print CLI help
 */
void cli_print_help(void);

#endif /* CLI_H */
