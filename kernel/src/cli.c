/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - COMMAND LINE INTERFACE
 * =============================================================================
 * 
 * Interactive CLI for testing kernel and LibOS operations.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

/*
 * Parse permission string (e.g., "rw", "rwx", "r") to permission flags
 */
static uint8_t parse_permissions(const char* str) {
    uint8_t perms = 0;
    if (!str) return 0;
    
    for (int i = 0; str[i]; i++) {
        switch (tolower(str[i])) {
            case 'r': perms |= PERM_READ; break;
            case 'w': perms |= PERM_WRITE; break;
            case 'x': perms |= PERM_EXECUTE; break;
        }
    }
    return perms;
}

/*
 * Format permission flags to string
 */
static const char* format_permissions(uint8_t perms) {
    static char buf[16];
    buf[0] = '\0';
    if (perms & PERM_READ) strcat(buf, "READ ");
    if (perms & PERM_WRITE) strcat(buf, "WRITE ");
    if (perms & PERM_EXECUTE) strcat(buf, "EXEC ");
    if (buf[0] == '\0') strcpy(buf, "NONE");
    return buf;
}

/*
 * Trim whitespace from string
 */
static char* trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

/* =============================================================================
 * CLI COMMANDS
 * =============================================================================
 */

static void cmd_help(void) {
    printf("\n");
    printf("Available Commands:\n");
    printf("-------------------\n");
    printf("  help                        - Show this help message\n");
    printf("  alloc <app_id>              - Allocate and bind a page (RW)\n");
    printf("  free <app_id> <page>        - Free and unbind a page\n");
    printf("  bind <app_id> <page> <perm> - Bind a page (perm: r/w/x)\n");
    printf("  unbind <app_id> <page>      - Unbind a page\n");
    printf("  check <app_id> <page> <perm>- Check access permission\n");
    printf("  run <app_id> [name]         - Start an app via LibOS\n");
    printf("  list                        - Show all active bindings\n");
    printf("  info                        - Show kernel information\n");
    printf("  status                      - Show kernel status summary\n");
    printf("  memory                      - Show memory statistics\n");
    printf("  version                     - Show kernel version\n");
    printf("  clear                       - Clear the screen\n");
    printf("  demo                        - Run a quick demo\n");
    printf("  exit                        - Shutdown and exit\n");
    printf("\n");
    printf("Examples:\n");
    printf("  alloc 10        - Allocate page for app 10\n");
    printf("  bind 10 5 rw    - Bind page 5 to app 10 with read/write\n");
    printf("  check 10 5 r    - Check if app 10 can read page 5\n");
    printf("  demo            - Show a quick demonstration\n");
    printf("\n");
}

static void cmd_alloc(uint32_t app_id) {
    uint32_t page = libos_alloc_and_bind(app_id, PERM_READ | PERM_WRITE);
    if (page != INVALID_PAGE) {
        printf("[OK] Allocated page %u for app %u (READ|WRITE)\n", page, app_id);
    } else {
        printf("[ERROR] Failed to allocate page for app %u\n", app_id);
    }
}

static void cmd_free(uint32_t app_id, uint32_t page) {
    if (libos_free_and_unbind(app_id, page)) {
        printf("[OK] Freed page %u for app %u\n", page, app_id);
    } else {
        printf("[ERROR] Failed to free page %u for app %u\n", page, app_id);
    }
}

static void cmd_bind(uint32_t app_id, uint32_t page, uint8_t perms) {
    if (libos_bind(app_id, page, perms)) {
        printf("[OK] Bound page %u to app %u with %s\n", page, app_id, format_permissions(perms));
    } else {
        printf("[ERROR] Failed to bind page %u to app %u\n", page, app_id);
    }
}

static void cmd_unbind(uint32_t app_id, uint32_t page) {
    if (libos_unbind(app_id, page)) {
        printf("[OK] Unbound page %u from app %u\n", page, app_id);
    } else {
        printf("[ERROR] Failed to unbind page %u from app %u\n", page, app_id);
    }
}

static void cmd_check(uint32_t app_id, uint32_t page, uint8_t perms) {
    if (libos_check(app_id, page, perms)) {
        printf("[OK] Access GRANTED: app %u can access page %u with %s\n", 
               app_id, page, format_permissions(perms));
    } else {
        printf("[DENIED] Access DENIED: app %u cannot access page %u with %s\n", 
               app_id, page, format_permissions(perms));
    }
}

static void cmd_run(uint32_t app_id, const char* name) {
    printf("[OK] Starting app %u (%s)...\n", app_id, name ? name : "unnamed");
    libos_run_app(app_id, name, NULL);
    printf("[OK] App %u finished\n", app_id);
}

static void cmd_list(void) {
    printf("\n");
    printf("Active Bindings:\n");
    printf("----------------\n");
    
    int found = 0;
    for (int i = 0; i < MAX_BINDINGS; i++) {
        if (g_kernel.bindings.entries[i].is_active) {
            BindingEntry* e = &g_kernel.bindings.entries[i];
            printf("  App %u -> Page %u [%s]\n", 
                   e->app_id, e->physical_page, format_permissions(e->permissions));
            found++;
        }
    }
    
    if (found == 0) {
        printf("  (no active bindings)\n");
    }
    printf("\nTotal: %d binding(s)\n\n", found);
}

static void cmd_info(void) {
    printf("\n");
    printf("Kernel Information:\n");
    printf("-------------------\n");
    printf("  Version:       %d.%d.%d\n", 
           KERNEL_VERSION_MAJOR, KERNEL_VERSION_MINOR, KERNEL_VERSION_PATCH);
    printf("  Status:        %s\n", g_kernel.is_running ? "RUNNING" : "STOPPED");
    printf("  Syscalls:      %u\n", g_kernel.syscall_count);
    printf("  Bindings:      %u / %d\n", g_kernel.bindings.count, MAX_BINDINGS);
    printf("  Free Pages:    %u / %d\n", 
           g_kernel.memory.free_count, MAX_PAGES);
    printf("\n");
}

static void cmd_status(void) {
    printf("\n");
    printf("=== KERNEL STATUS ===");
    printf("\n");
    printf("  State:     %s\n", g_kernel.is_running ? "[RUNNING]" : "[STOPPED]");
    printf("  Syscalls:  %u processed\n", g_kernel.syscall_count);
    printf("  Bindings:  %u active\n", g_kernel.bindings.count);
    printf("  Memory:    %u / %d pages free\n", g_kernel.memory.free_count, MAX_PAGES);
    printf("\n");
}

static void cmd_memory(void) {
    printf("\n");
    printf("=== MEMORY STATUS ===");
    printf("\n");
    printf("  Total Pages:     %d\n", MAX_PAGES);
    printf("  Free Pages:      %u\n", g_kernel.memory.free_count);
    printf("  Used Pages:      %u\n", MAX_PAGES - g_kernel.memory.free_count);
    printf("  Usage:           %.1f%%\n", 
           100.0 * (MAX_PAGES - g_kernel.memory.free_count) / MAX_PAGES);
    printf("\n");
}

static void cmd_version(void) {
    printf("\n");
    printf("Exokernel Prototype v%d.%d.%d\n", 
           KERNEL_VERSION_MAJOR, KERNEL_VERSION_MINOR, KERNEL_VERSION_PATCH);
    printf("Group 51 - 2026\n");
    printf("\n");
}

static void cmd_clear(void) {
    /* ANSI escape code to clear screen */
    printf("\033[2J\033[H");
}

static void cmd_demo(void) {
    printf("\n");
    printf("=== QUICK DEMO ===");
    printf("\n");
    printf("Step 1: Allocating pages for apps 100 and 200...\n");
    
    uint32_t page1 = libos_alloc_and_bind(100, PERM_READ | PERM_WRITE);
    printf("  -> App 100 got page %u (RW)\n", page1);
    
    uint32_t page2 = libos_alloc_and_bind(200, PERM_READ | PERM_WRITE | PERM_EXECUTE);
    printf("  -> App 200 got page %u (RWX)\n", page2);
    
    printf("\nStep 2: Testing access control...\n");
    printf("  -> App 100 READ page %u: %s\n", page1, 
           libos_check(100, page1, PERM_READ) ? "GRANTED" : "DENIED");
    printf("  -> App 200 READ page %u: %s\n", page1, 
           libos_check(200, page1, PERM_READ) ? "GRANTED" : "DENIED (no binding)");
    
    printf("\nStep 3: Revoking access...\n");
    libos_free_and_unbind(100, page1);
    printf("  -> App 100 page %u freed\n", page1);
    
    printf("\nStep 4: Current bindings...\n");
    cmd_list();
    
    printf("Demo complete!\n\n");
}

/* =============================================================================
 * CLI CORE
 * =============================================================================
 */

void cli_init(void) {
    /* Currently nothing to initialize */
}

void cli_print_help(void) {
    cmd_help();
}

int cli_process_command(const char* line) {
    char buf[CLI_MAX_LINE];
    char* args[CLI_MAX_ARGS];
    int argc = 0;
    
    /* Copy and trim */
    strncpy(buf, line, CLI_MAX_LINE - 1);
    buf[CLI_MAX_LINE - 1] = '\0';
    char* trimmed = trim(buf);
    
    /* Empty line */
    if (*trimmed == '\0') return 0;
    
    /* Tokenize */
    char* token = strtok(trimmed, " \t");
    while (token && argc < CLI_MAX_ARGS) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    
    if (argc == 0) return 0;
    
    /* Process commands */
    const char* cmd = args[0];
    
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        cmd_help();
    }
    else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
        printf("Shutting down...\n");
        return 1;
    }
    else if (strcmp(cmd, "alloc") == 0) {
        if (argc < 2) {
            printf("Usage: alloc <app_id>\n");
        } else {
            cmd_alloc((uint32_t)atoi(args[1]));
        }
    }
    else if (strcmp(cmd, "free") == 0) {
        if (argc < 3) {
            printf("Usage: free <app_id> <page>\n");
        } else {
            cmd_free((uint32_t)atoi(args[1]), (uint32_t)atoi(args[2]));
        }
    }
    else if (strcmp(cmd, "bind") == 0) {
        if (argc < 4) {
            printf("Usage: bind <app_id> <page> <permissions>\n");
            printf("  permissions: r=read, w=write, x=execute (e.g., rw, rwx)\n");
        } else {
            cmd_bind((uint32_t)atoi(args[1]), (uint32_t)atoi(args[2]), 
                     parse_permissions(args[3]));
        }
    }
    else if (strcmp(cmd, "unbind") == 0) {
        if (argc < 3) {
            printf("Usage: unbind <app_id> <page>\n");
        } else {
            cmd_unbind((uint32_t)atoi(args[1]), (uint32_t)atoi(args[2]));
        }
    }
    else if (strcmp(cmd, "check") == 0) {
        if (argc < 4) {
            printf("Usage: check <app_id> <page> <permissions>\n");
        } else {
            cmd_check((uint32_t)atoi(args[1]), (uint32_t)atoi(args[2]), 
                      parse_permissions(args[3]));
        }
    }
    else if (strcmp(cmd, "run") == 0) {
        if (argc < 2) {
            printf("Usage: run <app_id> [name]\n");
        } else {
            cmd_run((uint32_t)atoi(args[1]), argc > 2 ? args[2] : NULL);
        }
    }
    else if (strcmp(cmd, "list") == 0) {
        cmd_list();
    }
    else if (strcmp(cmd, "info") == 0) {
        cmd_info();
    }
    else if (strcmp(cmd, "status") == 0) {
        cmd_status();
    }
    else if (strcmp(cmd, "memory") == 0 || strcmp(cmd, "mem") == 0) {
        cmd_memory();
    }
    else if (strcmp(cmd, "version") == 0 || strcmp(cmd, "ver") == 0) {
        cmd_version();
    }
    else if (strcmp(cmd, "clear") == 0 || strcmp(cmd, "cls") == 0) {
        cmd_clear();
    }
    else if (strcmp(cmd, "demo") == 0) {
        cmd_demo();
    }
    else {
        printf("Unknown command: %s (type 'help' for available commands)\n", cmd);
    }
    
    return 0;
}

void cli_run(void) {
    char line[CLI_MAX_LINE];
    
    printf("\n");
    printf("========================================\n");
    printf("     EXOKERNEL PROTOTYPE CLI\n");
    printf("========================================\n");
    printf("Type 'help' for available commands.\n");
    printf("\n");
    
    cli_init();
    
    while (1) {
        printf("exokernel> ");
        fflush(stdout);
        
        if (fgets(line, CLI_MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        if (cli_process_command(line)) {
            break;
        }
    }
}
