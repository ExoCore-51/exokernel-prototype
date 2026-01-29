/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - SYSTEM CALL INTERFACE
 * =============================================================================
 * 
 * This module defines the system call interface that Library Operating
 * Systems (LibOS) use to communicate with the exokernel.
 * 
 * KEY CONCEPT - SYSTEM CALLS IN EXOKERNEL:
 * Traditional OS: System calls provide high-level abstractions (open file, etc.)
 * Exokernel: System calls only manage secure bindings and resource allocation
 * 
 * The LibOS builds its own abstractions on top of these minimal syscalls.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "secure_binding.h"
#include "access_control.h"

/* =============================================================================
 * SYSTEM CALL TYPES
 * =============================================================================
 */

/*
 * SyscallType: The types of system calls supported by the exokernel
 * 
 * These are MINIMAL - just enough to manage hardware access securely.
 */
typedef enum {
    SYS_BIND,       /* Request a secure binding to a resource */
    SYS_UNBIND,     /* Release a secure binding */
    SYS_ACCESS,     /* Check if access is permitted (for LibOS validation) */
    SYS_INFO,       /* Get kernel information (debugging) */
    SYS_SHUTDOWN,   /* Request kernel shutdown */
    SYS_ALLOC_PAGE, /* Allocate a physical memory page */
    SYS_FREE_PAGE   /* Free a previously allocated page */
} SyscallType;

/*
 * SyscallStatus: Result status of a system call
 */
typedef enum {
    SYSCALL_SUCCESS,        /* Operation completed successfully */
    SYSCALL_ERROR,          /* General error */
    SYSCALL_PERMISSION_DENIED,  /* Caller lacks permission */
    SYSCALL_NOT_FOUND,      /* Resource or binding not found */
    SYSCALL_INVALID         /* Invalid syscall or parameters */
} SyscallStatus;

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/*
 * SyscallRequest: Incoming request from a LibOS
 * 
 * This structure encapsulates all the information needed to process
 * a system call from an application/LibOS.
 */
typedef struct {
    SyscallType type;       /* What operation is requested */
    uint32_t    app_id;     /* Who is making the request */
    uint32_t    page;       /* Which resource (physical page) */
    uint8_t     permissions;/* What permissions (for BIND calls) */
} SyscallRequest;

/*
 * SyscallResponse: Response from the kernel
 * 
 * Contains the result of the system call operation.
 */
typedef struct {
    SyscallStatus status;   /* Success or error code */
    uint32_t      data;     /* Optional return data */
    const char*   message;  /* Human-readable message */
} SyscallResponse;

/* =============================================================================
 * FUNCTION DECLARATIONS
 * =============================================================================
 */

/*
 * Handle a system call from a LibOS/application
 * 
 * This is the MAIN ENTRY POINT for all kernel operations from user space.
 * 
 * @param table:   Pointer to the kernel's binding table
 * @param request: The system call request
 * 
 * @return: SyscallResponse with the result
 */
SyscallResponse syscall_handler(BindingTable* table, SyscallRequest request);

/*
 * Convert SyscallType to string (for debugging)
 */
const char* syscall_type_to_string(SyscallType type);

/*
 * Convert SyscallStatus to string (for debugging)
 */
const char* syscall_status_to_string(SyscallStatus status);

#endif /* SYSCALL_H */
