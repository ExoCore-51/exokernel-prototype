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
#include "libos.h"
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
    
    /* === LibOS demo: allocate/bind/use/free a page for App 10 === */
    libos_run_app(10, "demo_app", (libos_app_entry)0);
    uint32_t page = libos_alloc_and_bind(10, PERM_READ | PERM_WRITE);
    if (page != INVALID_PAGE) {
        /* Check access then clean up */
        libos_check(10, page, PERM_READ);
        libos_free_and_unbind(10, page);
    }
    
    /* Shutdown */
    kernel_shutdown();
    
    return 0;
}
