/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - MAIN KERNEL HEADER
 * =============================================================================
 * 
 * This is the master header that includes all kernel subsystems and
 * provides the main kernel lifecycle functions.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef KERNEL_H
#define KERNEL_H

/* Include all kernel subsystems */
#include "secure_binding.h"
#include "access_control.h"
#include "syscall.h"
#include "memory.h"
#include "logging.h"

/* Kernel version */
#define KERNEL_VERSION_MAJOR 0
#define KERNEL_VERSION_MINOR 1
#define KERNEL_VERSION_PATCH 0

/* =============================================================================
 * KERNEL STATE
 * =============================================================================
 */

/*
 * KernelState: The complete state of the kernel
 * 
 * This structure holds all the data the kernel needs to operate.
 * In a real kernel, this would be much more complex, but for our
 * prototype, we focus on the core exokernel concepts.
 */
typedef struct {
    BindingTable bindings;  /* The secure binding table */
    int          is_running; /* Is the kernel running? */
    uint32_t     syscall_count; /* Number of syscalls processed */
    MemoryState  memory;     /* Physical memory manager state */
} KernelState;

/* Global kernel state (extern declaration) */
extern KernelState g_kernel;

/* =============================================================================
 * FUNCTION DECLARATIONS
 * =============================================================================
 */

/*
 * Initialize the kernel
 * 
 * Must be called before any other kernel operations.
 * Sets up all subsystems and prepares for operation.
 * 
 * @return: 0 on success, -1 on failure
 */
int kernel_init(void);

/*
 * Main kernel loop (simulated)
 * 
 * In a real kernel, this would be the main event loop.
 * For our prototype, it demonstrates kernel operation.
 */
void kernel_run(void);

/*
 * Shutdown the kernel
 * 
 * Performs cleanup and releases all resources.
 */
void kernel_shutdown(void);

/*
 * Process a system call (wrapper around syscall_handler)
 * 
 * @param request: The system call request
 * @return: System call response
 */
SyscallResponse kernel_syscall(SyscallRequest request);

/*
 * Print kernel version information
 */
void kernel_print_version(void);

#endif /* KERNEL_H */
