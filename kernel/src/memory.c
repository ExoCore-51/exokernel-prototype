/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - PHYSICAL MEMORY MANAGER IMPLEMENTATION
 * =============================================================================
 */

#include "memory.h"
#include "logging.h"
#include <string.h>

void memory_init(MemoryState* mem) {
    if (!mem) return;
    memset(mem->owner, 0, sizeof(mem->owner));
    memset(mem->allocated, 0, sizeof(mem->allocated));
    mem->free_count = MAX_PAGES;
    log_json("kernel", "memory_init", 0, 0, 0, "OK", "Memory initialized", 0, mem->free_count);
}

uint32_t memory_alloc_page(MemoryState* mem, uint32_t app_id) {
    if (!mem || app_id == 0) {
        log_json("kernel", "memory_alloc_page", app_id, INVALID_PAGE, 0, "ERROR", "Invalid params", 0, mem ? mem->free_count : 0);
        return INVALID_PAGE;
    }
    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        if (!mem->allocated[i]) {
            mem->allocated[i] = 1;
            mem->owner[i] = (uint8_t)app_id;
            mem->free_count--;
            log_json("kernel", "memory_alloc_page", app_id, i, 0, "SUCCESS", "Allocated page", 0, mem->free_count);
            return i;
        }
    }
    log_json("kernel", "memory_alloc_page", app_id, INVALID_PAGE, 0, "ERROR", "Out of memory", 0, mem->free_count);
    return INVALID_PAGE;
}

bool memory_free_page(MemoryState* mem, uint32_t app_id, uint32_t page) {
    if (!mem || page >= MAX_PAGES || app_id == 0) {
        log_json("kernel", "memory_free_page", app_id, page, 0, "ERROR", "Invalid params", 0, mem ? mem->free_count : 0);
        return false;
    }
    if (!mem->allocated[page] || mem->owner[page] != (uint8_t)app_id) {
        log_json("kernel", "memory_free_page", app_id, page, 0, "ERROR", "Not owner or not allocated", 0, mem->free_count);
        return false;
    }
    mem->allocated[page] = 0;
    mem->owner[page] = 0;
    mem->free_count++;
    log_json("kernel", "memory_free_page", app_id, page, 0, "SUCCESS", "Freed page", 0, mem->free_count);
    return true;
}

uint32_t memory_free_pages(const MemoryState* mem) {
    return mem ? mem->free_count : 0;
}
