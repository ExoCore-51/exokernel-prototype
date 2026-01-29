/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - LIBRARY OS HELPERS
 * =============================================================================
 *
 * Convenience wrappers that an application (LibOS) uses to interact with the
 * exokernel: memory allocation/free, binding management, access checks, and
 * simple app run scaffolding.
 *
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef LIBOS_H
#define LIBOS_H

#include <stdint.h>
#include "kernel.h"

/* Allocate a page and bind with given permissions; returns page or INVALID_PAGE */
uint32_t libos_alloc_and_bind(uint32_t app_id, uint8_t permissions);

/* Free a page and unbind */
bool libos_free_and_unbind(uint32_t app_id, uint32_t page);

/* Explicit binding helpers */
bool libos_bind(uint32_t app_id, uint32_t page, uint8_t permissions);
bool libos_unbind(uint32_t app_id, uint32_t page);

/* Check access convenience */
bool libos_check(uint32_t app_id, uint32_t page, uint8_t permissions);

/* Minimal app runner scaffold */
typedef void (*libos_app_entry)(void);
void libos_run_app(uint32_t app_id, const char* name, libos_app_entry entry);

#endif /* LIBOS_H */
