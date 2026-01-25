/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - ACCESS CONTROL
 * =============================================================================
 * 
 * This module provides the check_access function, which validates whether
 * an application has permission to perform a specific operation on a resource.
 * 
 * KEY CONCEPT:
 * In an exokernel, access control is minimal but SECURE. We don't provide
 * abstractions - we just verify that the app has the right to access
 * the hardware directly.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <stdint.h>
#include "secure_binding.h"

/* =============================================================================
 * RESULT CODES
 * =============================================================================
 */

/*
 * AccessResult: Possible outcomes of an access check
 */
typedef enum {
    ACCESS_GRANTED,         /* Permission granted - proceed with operation */
    ACCESS_DENIED,          /* Permission denied - no binding or wrong perms */
    ACCESS_NO_BINDING,      /* No binding exists for this app/resource pair */
    ACCESS_INVALID_PARAMS   /* Invalid parameters passed to check_access */
} AccessResult;

/* =============================================================================
 * FUNCTION DECLARATIONS
 * =============================================================================
 */

/*
 * Check if an application has permission for an operation
 * 
 * This is the CORE security function of the exokernel. Before any
 * application can access hardware, this function verifies that:
 * 1. A binding exists for the app and resource
 * 2. The binding includes the requested permission
 * 
 * @param table:                Pointer to the binding table
 * @param app_id:               Application requesting access
 * @param page:                 Physical page being accessed
 * @param requested_permission: The operation being attempted (PERM_READ, etc.)
 * 
 * @return: AccessResult indicating whether access is allowed
 */
AccessResult check_access(BindingTable* table, uint32_t app_id, 
                          uint32_t page, uint8_t requested_permission);

/*
 * Convert AccessResult to a human-readable string
 * 
 * @param result: The AccessResult to convert
 * @return: String representation of the result
 */
const char* access_result_to_string(AccessResult result);

#endif /* ACCESS_CONTROL_H */
