/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - JSON LOGGING UTILITY
 * =============================================================================
 *
 * Ensures every kernel/libOS action emits a single-line JSON event for the
 * external monitor. Keep output minimal and machine-readable.
 *
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

/* Emit a JSON line with common fields */
void log_json(const char* component,
              const char* event,
              uint32_t app_id,
              uint32_t page,
              uint8_t permissions,
              const char* status,
              const char* message,
              uint32_t data,
              uint32_t count);

/* Convert permissions to RWX string */
const char* perms_to_string(uint8_t p);

#endif /* LOGGING_H */
