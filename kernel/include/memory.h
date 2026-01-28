/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - PHYSICAL MEMORY MANAGER (KERNEL CORE)
 * =============================================================================
 *
 * Provides a minimal physical page allocator that the LibOS can use via
 * syscalls. This aligns with exokernel principles: the kernel multiplexes
 * hardware securely; higher-level abstractions (heaps, VMM) live in the LibOS.
 *
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

/* Total number of physical pages available in the prototype */
#define MAX_PAGES 1024

/* Special value indicating an invalid page */
#define INVALID_PAGE UINT32_MAX

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    uint8_t  owner[MAX_PAGES]; /* Owning app_id (0 = free) */
    uint8_t  allocated[MAX_PAGES]; /* 0 = free, 1 = allocated */
    uint32_t free_count; /* Count of free pages */
} MemoryState;

/* =============================================================================
 * API
 * =============================================================================
 */

/* Initialize memory state */
void memory_init(MemoryState* mem);

/* Allocate a single page for an app; returns page index or INVALID_PAGE */
uint32_t memory_alloc_page(MemoryState* mem, uint32_t app_id);

/* Free a page owned by app; returns true on success */
bool memory_free_page(MemoryState* mem, uint32_t app_id, uint32_t page);

/* Get info: number of free pages */
uint32_t memory_free_pages(const MemoryState* mem);

#endif /* MEMORY_H */
