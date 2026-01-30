#include "logging.h"
#include <stdio.h>
#include <time.h>

static char perms_buf[4];
static FILE* log_file = NULL;

const char* perms_to_string(uint8_t p) {
    perms_buf[0] = (p & 0x01) ? 'R' : '-';
    perms_buf[1] = (p & 0x02) ? 'W' : '-';
    perms_buf[2] = (p & 0x04) ? 'X' : '-';
    perms_buf[3] = '\0';
    return perms_buf;
}

void log_init(void) {
    log_file = fopen("kernel.log", "w");
}

void log_close(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_json(const char* component,
              const char* event,
              uint32_t app_id,
              uint32_t page,
              uint8_t permissions,
              const char* status,
              const char* message,
              uint32_t data,
              uint32_t count) {
    time_t now = time(NULL);
    
    /* Format the JSON line */
    char json[512];
    snprintf(json, sizeof(json),
        "{\"ts\":%ld,\"component\":\"%s\",\"event\":\"%s\",\"app_id\":%u,\"page\":%u,\"perms\":\"%s\",\"status\":\"%s\",\"message\":\"%s\",\"data\":%u,\"count\":%u}\n",
        (long)now,
        component ? component : "",
        event ? event : "",
        app_id,
        page,
        perms_to_string(permissions),
        status ? status : "",
        message ? message : "",
        data,
        count);
    
    /* Output to console */
    printf("%s", json);
    
    /* Output to file */
    if (log_file) {
        fprintf(log_file, "%s", json);
        fflush(log_file);
    }
}

