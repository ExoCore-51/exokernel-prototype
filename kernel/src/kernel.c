/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - KERNEL CORE IMPLEMENTATION
 * =============================================================================
 * 
 * This file implements the main kernel lifecycle functions.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "kernel.h"
#include <stdio.h>
#include <string.h>

/* =============================================================================
 * GLOBAL STATE
 * =============================================================================
 */

/* The global kernel state - in a real kernel this would be in protected memory */
KernelState g_kernel;

/* =============================================================================
 * IMPLEMENTATION
 * =============================================================================
 */

/*
 * Print kernel version and banner
 */
void kernel_print_version(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        EXOKERNEL PROTOTYPE - GROUP 51                         ║\n");
    printf("║        Version %d.%d.%d                                          ║\n",
           KERNEL_VERSION_MAJOR, KERNEL_VERSION_MINOR, KERNEL_VERSION_PATCH);
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Secure Hardware Multiplexing | Minimal Abstractions          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/*
 * Initialize the kernel
 */
int kernel_init(void) {
    kernel_print_version();
    
    printf("[KERNEL] Initializing kernel...\n");
    
    /* Initialize the binding table */
    binding_table_init(&g_kernel.bindings);
    
    /* Set initial state */
    g_kernel.is_running = 1;
    g_kernel.syscall_count = 0;
    
    printf("[KERNEL] Kernel initialized successfully!\n");
    printf("[KERNEL] Ready to accept system calls from LibOS.\n\n");
    
    return 0;
}

/*
 * Process a system call
 */
SyscallResponse kernel_syscall(SyscallRequest request) {
    g_kernel.syscall_count++;
    return syscall_handler(&g_kernel.bindings, request);
}

/*
 * Main kernel loop (demonstration)
 * 
 * This simulates what a real kernel loop would do:
 * - Wait for interrupts/syscalls
 * - Process them
 * - Return control to applications
 */
void kernel_run(void) {
    printf("[KERNEL] Entering main loop...\n\n");
    
    /* 
     * In a real kernel, this would be an infinite loop waiting for
     * hardware interrupts. For our prototype, we just demonstrate
     * the concepts and then exit.
     */
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  DEMONSTRATION: Simulating LibOS Interactions\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    /* === DEMO 1: Create bindings for two applications === */
    printf("--- Demo 1: Creating Secure Bindings ---\n\n");
    
    SyscallRequest req1 = {
        .type = SYS_BIND,
        .app_id = 1,
        .page = 100,
        .permissions = PERM_READ | PERM_WRITE
    };
    kernel_syscall(req1);
    
    SyscallRequest req2 = {
        .type = SYS_BIND,
        .app_id = 1,
        .page = 101,
        .permissions = PERM_READ | PERM_EXECUTE
    };
    kernel_syscall(req2);
    
    SyscallRequest req3 = {
        .type = SYS_BIND,
        .app_id = 2,
        .page = 200,
        .permissions = PERM_ALL
    };
    kernel_syscall(req3);
    
    /* Print current binding table */
    binding_table_print(&g_kernel.bindings);
    
    /* === DEMO 2: Access control checks === */
    printf("--- Demo 2: Access Control Checks ---\n\n");
    
    /* App 1 tries to READ page 100 (should succeed) */
    SyscallRequest check1 = {
        .type = SYS_ACCESS,
        .app_id = 1,
        .page = 100,
        .permissions = PERM_READ
    };
    kernel_syscall(check1);
    
    /* App 1 tries to EXECUTE page 100 (should fail - no execute permission) */
    SyscallRequest check2 = {
        .type = SYS_ACCESS,
        .app_id = 1,
        .page = 100,
        .permissions = PERM_EXECUTE
    };
    kernel_syscall(check2);
    
    /* App 2 tries to access page 100 (should fail - no binding) */
    SyscallRequest check3 = {
        .type = SYS_ACCESS,
        .app_id = 2,
        .page = 100,
        .permissions = PERM_READ
    };
    kernel_syscall(check3);
    
    /* === DEMO 3: Remove a binding (visible revocation) === */
    printf("\n--- Demo 3: Visible Revocation ---\n\n");
    
    SyscallRequest unbind = {
        .type = SYS_UNBIND,
        .app_id = 1,
        .page = 100,
        .permissions = 0
    };
    kernel_syscall(unbind);
    
    /* Print updated binding table */
    binding_table_print(&g_kernel.bindings);
    
    /* === DEMO 4: Kernel info === */
    printf("--- Demo 4: Kernel Information ---\n\n");
    
    SyscallRequest info = {
        .type = SYS_INFO,
        .app_id = 0,
        .page = 0,
        .permissions = 0
    };
    kernel_syscall(info);
    
    printf("\n[KERNEL] Total syscalls processed: %u\n", g_kernel.syscall_count);
    printf("[KERNEL] Demo complete.\n\n");
}

/*
 * Shutdown the kernel
 */
void kernel_shutdown(void) {
    printf("[KERNEL] Initiating shutdown...\n");
    
    /* Clear the binding table */
    binding_table_init(&g_kernel.bindings);
    
    g_kernel.is_running = 0;
    
    printf("[KERNEL] Kernel shut down cleanly.\n");
    printf("[KERNEL] Total syscalls processed during session: %u\n", 
           g_kernel.syscall_count);
    printf("\nGoodbye!\n\n");
}
