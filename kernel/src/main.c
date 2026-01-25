/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - MAIN ENTRY POINT
 * =============================================================================
 * 
 * This is the entry point for the exokernel prototype.
 * It demonstrates the kernel's capabilities.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "kernel.h"
#include <stdio.h>

/*
 * Main function
 * 
 * In a real system, this would be called by the bootloader.
 * For our prototype, we run as a regular program.
 */
int main(int argc, char* argv[]) {
    
    /* Initialize the kernel */
    if (kernel_init() != 0) {
        printf("[ERROR] Failed to initialize kernel!\n");
        return 1;
    }
    
    /* Run the kernel (demonstration) */
    kernel_run();
    
    /* Shutdown */
    kernel_shutdown();
    
    return 0;
}
