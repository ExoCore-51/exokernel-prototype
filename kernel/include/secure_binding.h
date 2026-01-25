/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - SECURE BINDING TABLE
 * =============================================================================
 * 
 * This header defines the Secure Binding Table, the core data structure
 * of an exokernel. It maps application IDs to physical hardware resources
 * with specific permissions.
 * 
 * KEY CONCEPT - SECURE BINDINGS:
 * Unlike traditional kernels that abstract hardware, an exokernel gives
 * applications direct access to hardware through "secure bindings."
 * A binding is a one-time permission check that creates a direct link
 * between an application and a physical resource.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef SECURE_BINDING_H
#define SECURE_BINDING_H

#include <stdint.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

/* Maximum number of bindings the kernel can track */
#define MAX_BINDINGS 256

/* Maximum number of applications supported */
#define MAX_APPS 16

/* Permission flags - can be combined using bitwise OR */
#define PERM_NONE    0x00   /* No permissions */
#define PERM_READ    0x01   /* Read access */
#define PERM_WRITE   0x02   /* Write access */
#define PERM_EXECUTE 0x04   /* Execute access */
#define PERM_ALL     (PERM_READ | PERM_WRITE | PERM_EXECUTE)

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/*
 * BindingEntry: Represents a single secure binding
 * 
 * This structure maps an application to a physical resource (like a memory
 * page) and defines what operations are permitted.
 * 
 * Fields:
 *   - app_id:       Unique identifier for the application (0 = unused entry)
 *   - physical_page: The physical memory page number being bound
 *   - permissions:   Bitfield of PERM_READ, PERM_WRITE, PERM_EXECUTE
 *   - is_active:     Whether this binding is currently valid
 */
typedef struct {
    uint32_t app_id;         /* Application identifier */
    uint32_t physical_page;  /* Physical page number */
    uint8_t  permissions;    /* Permission flags */
    bool     is_active;      /* Is this binding active? */
} BindingEntry;

/*
 * BindingTable: The complete table of all secure bindings
 * 
 * This is the central data structure that the kernel uses to track
 * which applications have access to which resources.
 */
typedef struct {
    BindingEntry entries[MAX_BINDINGS];  /* Array of binding entries */
    uint32_t     count;                   /* Number of active bindings */
} BindingTable;

/* =============================================================================
 * FUNCTION DECLARATIONS
 * =============================================================================
 */

/*
 * Initialize the binding table
 * 
 * This MUST be called before any other binding operations.
 * Clears all entries and sets count to 0.
 * 
 * @param table: Pointer to the binding table to initialize
 */
void binding_table_init(BindingTable* table);

/*
 * Create a new secure binding
 * 
 * This grants an application access to a physical page with specific
 * permissions. The binding is verified at creation time, and then
 * the application can access the resource directly.
 * 
 * @param table:       Pointer to the binding table
 * @param app_id:      Application requesting the binding
 * @param page:        Physical page number to bind
 * @param permissions: Permission flags (PERM_READ, PERM_WRITE, PERM_EXECUTE)
 * 
 * @return: true if binding created successfully, false otherwise
 */
bool create_binding(BindingTable* table, uint32_t app_id, 
                    uint32_t page, uint8_t permissions);

/*
 * Remove an existing secure binding
 * 
 * Revokes an application's access to a physical page.
 * 
 * @param table:  Pointer to the binding table
 * @param app_id: Application whose binding to remove
 * @param page:   Physical page to unbind
 * 
 * @return: true if binding removed, false if not found
 */
bool remove_binding(BindingTable* table, uint32_t app_id, uint32_t page);

/*
 * Find a binding entry
 * 
 * Looks up the binding for a specific app and page combination.
 * 
 * @param table:  Pointer to the binding table
 * @param app_id: Application to look up
 * @param page:   Physical page to look up
 * 
 * @return: Pointer to BindingEntry if found, NULL otherwise
 */
BindingEntry* find_binding(BindingTable* table, uint32_t app_id, uint32_t page);

/*
 * Print the current state of the binding table (for debugging)
 * 
 * @param table: Pointer to the binding table
 */
void binding_table_print(BindingTable* table);

#endif /* SECURE_BINDING_H */
