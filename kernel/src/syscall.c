/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - SYSTEM CALL HANDLER IMPLEMENTATION
 * =============================================================================
 * 
 * Implements the system call dispatcher that routes LibOS requests
 * to the appropriate kernel functions.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "syscall.h"
#include <stdio.h>

/* =============================================================================
 * IMPLEMENTATION
 * =============================================================================
 */

/*
 * System Call Handler
 * 
 * This function acts as the dispatcher for all system calls.
 * Think of it as the "receptionist" of the kernel - it receives
 * requests and routes them to the appropriate handler.
 */
SyscallResponse syscall_handler(BindingTable* table, SyscallRequest request) {
    SyscallResponse response;
    
    printf("\n[SYSCALL] Received: %s from App %u\n",
           syscall_type_to_string(request.type), request.app_id);
    
    /* Validate common parameters */
    if (table == NULL) {
        response.status = SYSCALL_ERROR;
        response.data = 0;
        response.message = "Kernel not initialized";
        return response;
    }
    
    /* Dispatch based on syscall type */
    switch (request.type) {
        
        /* ============================================================
         * SYS_BIND: Create a new secure binding
         * ============================================================
         */
        case SYS_BIND: {
            bool success = create_binding(table, request.app_id, 
                                         request.page, request.permissions);
            if (success) {
                response.status = SYSCALL_SUCCESS;
                response.data = request.page;
                response.message = "Binding created successfully";
            } else {
                response.status = SYSCALL_ERROR;
                response.data = 0;
                response.message = "Failed to create binding";
            }
            break;
        }
        
        /* ============================================================
         * SYS_UNBIND: Remove an existing binding
         * ============================================================
         */
        case SYS_UNBIND: {
            bool success = remove_binding(table, request.app_id, request.page);
            if (success) {
                response.status = SYSCALL_SUCCESS;
                response.data = request.page;
                response.message = "Binding removed successfully";
            } else {
                response.status = SYSCALL_NOT_FOUND;
                response.data = 0;
                response.message = "Binding not found";
            }
            break;
        }
        
        /* ============================================================
         * SYS_ACCESS: Check access permissions
         * ============================================================
         */
        case SYS_ACCESS: {
            AccessResult result = check_access(table, request.app_id,
                                              request.page, request.permissions);
            if (result == ACCESS_GRANTED) {
                response.status = SYSCALL_SUCCESS;
                response.data = 1;
                response.message = "Access granted";
            } else if (result == ACCESS_NO_BINDING) {
                response.status = SYSCALL_NOT_FOUND;
                response.data = 0;
                response.message = "No binding exists";
            } else {
                response.status = SYSCALL_PERMISSION_DENIED;
                response.data = 0;
                response.message = "Access denied";
            }
            break;
        }
        
        /* ============================================================
         * SYS_INFO: Return kernel information
         * ============================================================
         */
        case SYS_INFO: {
            response.status = SYSCALL_SUCCESS;
            response.data = table->count;  /* Return number of active bindings */
            response.message = "Kernel info retrieved";
            printf("[SYSCALL] Active bindings: %u\n", table->count);
            break;
        }
        
        /* ============================================================
         * SYS_SHUTDOWN: Prepare for kernel shutdown
         * ============================================================
         */
        case SYS_SHUTDOWN: {
            response.status = SYSCALL_SUCCESS;
            response.data = 0;
            response.message = "Shutdown acknowledged";
            printf("[SYSCALL] Shutdown requested by App %u\n", request.app_id);
            break;
        }
        
        /* ============================================================
         * Unknown syscall
         * ============================================================
         */
        default: {
            response.status = SYSCALL_INVALID;
            response.data = 0;
            response.message = "Unknown system call";
            printf("[SYSCALL] ERROR: Unknown syscall type %d\n", request.type);
            break;
        }
    }
    
    printf("[SYSCALL] Response: %s - %s\n",
           syscall_status_to_string(response.status), response.message);
    
    return response;
}

/*
 * Convert syscall type to string
 */
const char* syscall_type_to_string(SyscallType type) {
    switch (type) {
        case SYS_BIND:     return "SYS_BIND";
        case SYS_UNBIND:   return "SYS_UNBIND";
        case SYS_ACCESS:   return "SYS_ACCESS";
        case SYS_INFO:     return "SYS_INFO";
        case SYS_SHUTDOWN: return "SYS_SHUTDOWN";
        default:           return "UNKNOWN";
    }
}

/*
 * Convert syscall status to string
 */
const char* syscall_status_to_string(SyscallStatus status) {
    switch (status) {
        case SYSCALL_SUCCESS:           return "SUCCESS";
        case SYSCALL_ERROR:             return "ERROR";
        case SYSCALL_PERMISSION_DENIED: return "PERMISSION_DENIED";
        case SYSCALL_NOT_FOUND:         return "NOT_FOUND";
        case SYSCALL_INVALID:           return "INVALID";
        default:                        return "UNKNOWN";
    }
}
