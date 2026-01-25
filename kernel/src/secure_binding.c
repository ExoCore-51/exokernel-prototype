/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - SECURE BINDING TABLE IMPLEMENTATION
 * =============================================================================
 * 
 * This file implements the Secure Binding Table operations.
 * 
 * HOW IT WORKS:
 * 1. When an app requests access to a resource, we create a binding
 * 2. The binding stores: WHO (app_id) can do WHAT (permissions) to WHERE (page)
 * 3. Future access checks are fast because the binding already validated access
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "secure_binding.h"
#include <stdio.h>
#include <string.h>

/* =============================================================================
 * IMPLEMENTATION
 * =============================================================================
 */

/*
 * Initialize the binding table
 * 
 * Sets all entries to inactive and resets the counter.
 * This is a simple but effective approach for a prototype.
 */
void binding_table_init(BindingTable* table) {
    if (table == NULL) {
        return;
    }
    
    /* Clear the entire table using memset for efficiency */
    memset(table->entries, 0, sizeof(table->entries));
    table->count = 0;
    
    printf("[KERNEL] Binding table initialized (capacity: %d)\n", MAX_BINDINGS);
}

/*
 * Create a new secure binding
 * 
 * Step by step:
 * 1. Validate inputs
 * 2. Check if binding already exists (prevent duplicates)
 * 3. Find an empty slot in the table
 * 4. Create the binding entry
 */
bool create_binding(BindingTable* table, uint32_t app_id, 
                    uint32_t page, uint8_t permissions) {
    
    /* Step 1: Validate inputs */
    if (table == NULL || app_id == 0) {
        printf("[KERNEL] ERROR: Invalid parameters for create_binding\n");
        return false;
    }
    
    if (permissions == PERM_NONE) {
        printf("[KERNEL] ERROR: Cannot create binding with no permissions\n");
        return false;
    }
    
    /* Step 2: Check if this binding already exists */
    BindingEntry* existing = find_binding(table, app_id, page);
    if (existing != NULL) {
        printf("[KERNEL] WARNING: Binding already exists for app %u, page %u\n", 
               app_id, page);
        /* Update permissions instead of failing */
        existing->permissions = permissions;
        printf("[KERNEL] Updated permissions to 0x%02X\n", permissions);
        return true;
    }
    
    /* Step 3: Find an empty slot */
    if (table->count >= MAX_BINDINGS) {
        printf("[KERNEL] ERROR: Binding table is full!\n");
        return false;
    }
    
    /* Look for first inactive entry */
    for (int i = 0; i < MAX_BINDINGS; i++) {
        if (!table->entries[i].is_active) {
            /* Step 4: Create the binding entry */
            table->entries[i].app_id = app_id;
            table->entries[i].physical_page = page;
            table->entries[i].permissions = permissions;
            table->entries[i].is_active = true;
            table->count++;
            
            printf("[KERNEL] Created binding: App %u -> Page %u "
                   "(perms: %c%c%c)\n",
                   app_id, page,
                   (permissions & PERM_READ)    ? 'R' : '-',
                   (permissions & PERM_WRITE)   ? 'W' : '-',
                   (permissions & PERM_EXECUTE) ? 'X' : '-');
            
            return true;
        }
    }
    
    /* Should never reach here if count is accurate */
    printf("[KERNEL] ERROR: Could not find empty slot (inconsistent state)\n");
    return false;
}

/*
 * Remove an existing secure binding
 * 
 * This implements "visible revocation" - we notify the app that
 * its access is being revoked rather than silently removing it.
 */
bool remove_binding(BindingTable* table, uint32_t app_id, uint32_t page) {
    if (table == NULL) {
        return false;
    }
    
    BindingEntry* entry = find_binding(table, app_id, page);
    if (entry == NULL) {
        printf("[KERNEL] WARNING: No binding found for app %u, page %u\n",
               app_id, page);
        return false;
    }
    
    /* Mark as inactive (soft delete) */
    entry->is_active = false;
    entry->app_id = 0;
    entry->permissions = PERM_NONE;
    table->count--;
    
    printf("[KERNEL] Removed binding: App %u no longer has access to page %u\n",
           app_id, page);
    
    return true;
}

/*
 * Find a binding entry
 * 
 * Linear search through the table. For a production system, you'd use
 * a hash table or tree structure for O(1) or O(log n) lookups.
 */
BindingEntry* find_binding(BindingTable* table, uint32_t app_id, uint32_t page) {
    if (table == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < MAX_BINDINGS; i++) {
        if (table->entries[i].is_active &&
            table->entries[i].app_id == app_id &&
            table->entries[i].physical_page == page) {
            return &table->entries[i];
        }
    }
    
    return NULL;
}

/*
 * Print the binding table (for debugging)
 * 
 * Shows all active bindings in a formatted table.
 */
void binding_table_print(BindingTable* table) {
    if (table == NULL) {
        printf("[KERNEL] ERROR: NULL table\n");
        return;
    }
    
    printf("\n");
    printf("=== SECURE BINDING TABLE ===\n");
    printf("Active bindings: %u / %d\n", table->count, MAX_BINDINGS);
    printf("-----------------------------\n");
    printf("| App ID | Page   | Perms   |\n");
    printf("-----------------------------\n");
    
    for (int i = 0; i < MAX_BINDINGS; i++) {
        if (table->entries[i].is_active) {
            uint8_t p = table->entries[i].permissions;
            printf("| %-6u | %-6u | %c%c%c     |\n",
                   table->entries[i].app_id,
                   table->entries[i].physical_page,
                   (p & PERM_READ)    ? 'R' : '-',
                   (p & PERM_WRITE)   ? 'W' : '-',
                   (p & PERM_EXECUTE) ? 'X' : '-');
        }
    }
    
    printf("-----------------------------\n\n");
}
