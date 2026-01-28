/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - ACCESS CONTROL IMPLEMENTATION
 * =============================================================================
 * 
 * Implements the security verification for hardware access requests.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "access_control.h"
#include "logging.h"
#include <stdio.h>

/* =============================================================================
 * IMPLEMENTATION
 * =============================================================================
 */

/*
 * Check if an application has permission for an operation
 * 
 * ALGORITHM:
 * 1. Validate inputs
 * 2. Look up the binding for this app/page combination
 * 3. If no binding exists -> ACCESS_NO_BINDING
 * 4. Check if the binding's permissions include the requested permission
 * 5. Return ACCESS_GRANTED or ACCESS_DENIED
 */
AccessResult check_access(BindingTable* table, uint32_t app_id, 
                          uint32_t page, uint8_t requested_permission) {
    
    /* Step 1: Validate inputs */
    if (table == NULL || app_id == 0 || requested_permission == PERM_NONE) {
        printf("[ACCESS] Invalid parameters for access check\n");
        log_json("kernel", "access_check", app_id, page, requested_permission, "ERROR", "Invalid parameters", 0, table ? table->count : 0);
        return ACCESS_INVALID_PARAMS;
    }
    
    /* Step 2: Look up the binding */
    BindingEntry* binding = find_binding(table, app_id, page);
    
    /* Step 3: Check if binding exists */
    if (binding == NULL) {
        printf("[ACCESS] DENIED: App %u has no binding for page %u\n",
               app_id, page);
        log_json("kernel", "access_check", app_id, page, requested_permission, "NOT_FOUND", "No binding", 0, table->count);
        return ACCESS_NO_BINDING;
    }
    
    /* Step 4 & 5: Check permissions using bitwise AND */
    /*
     * How this works:
     * If binding->permissions = 0x03 (READ | WRITE) = 0b00000011
     * And requested_permission = 0x01 (READ)        = 0b00000001
     * Then (0x03 & 0x01) = 0x01, which equals requested_permission
     * So access is GRANTED.
     * 
     * But if requested_permission = 0x04 (EXECUTE)  = 0b00000100
     * Then (0x03 & 0x04) = 0x00, which is NOT equal to 0x04
     * So access is DENIED.
     */
    if ((binding->permissions & requested_permission) == requested_permission) {
        printf("[ACCESS] GRANTED: App %u can %s page %u\n",
               app_id,
               (requested_permission == PERM_READ)    ? "READ" :
               (requested_permission == PERM_WRITE)   ? "WRITE" :
               (requested_permission == PERM_EXECUTE) ? "EXECUTE" : "ACCESS",
               page);
        log_json("kernel", "access_check", app_id, page, requested_permission, "SUCCESS", "Access granted", 1, table->count);
        return ACCESS_GRANTED;
    }
    
    printf("[ACCESS] DENIED: App %u lacks permission 0x%02X for page %u\n",
           app_id, requested_permission, page);
    log_json("kernel", "access_check", app_id, page, requested_permission, "DENIED", "Access denied", 0, table->count);
    return ACCESS_DENIED;
}

/*
 * Convert AccessResult to string
 */
const char* access_result_to_string(AccessResult result) {
    switch (result) {
        case ACCESS_GRANTED:       return "ACCESS_GRANTED";
        case ACCESS_DENIED:        return "ACCESS_DENIED";
        case ACCESS_NO_BINDING:    return "ACCESS_NO_BINDING";
        case ACCESS_INVALID_PARAMS: return "ACCESS_INVALID_PARAMS";
        default:                   return "UNKNOWN_RESULT";
    }
}
