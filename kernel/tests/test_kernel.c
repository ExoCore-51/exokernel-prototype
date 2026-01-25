/*
 * =============================================================================
 * EXOKERNEL PROTOTYPE - UNIT TESTS
 * =============================================================================
 * 
 * Simple unit tests to verify the kernel core functionality.
 * 
 * Author: Group 51
 * Date: 2026
 * =============================================================================
 */

#include "kernel.h"
#include <stdio.h>
#include <assert.h>

/* Test counters */
static int tests_run = 0;
static int tests_passed = 0;

/* =============================================================================
 * TEST UTILITIES
 * =============================================================================
 */

#define TEST(name) void name(void)
#define RUN_TEST(test) do { \
    printf("Running %s... ", #test); \
    tests_run++; \
    test(); \
    tests_passed++; \
    printf("PASSED\n"); \
} while(0)

#define ASSERT(condition) do { \
    if (!(condition)) { \
        printf("FAILED\n"); \
        printf("  Assertion failed: %s\n", #condition); \
        printf("  Line %d in %s\n", __LINE__, __FILE__); \
        return; \
    } \
} while(0)

/* =============================================================================
 * TESTS: SECURE BINDING TABLE
 * =============================================================================
 */

TEST(test_binding_table_init) {
    BindingTable table;
    binding_table_init(&table);
    
    ASSERT(table.count == 0);
    
    /* Verify all entries are inactive */
    for (int i = 0; i < MAX_BINDINGS; i++) {
        ASSERT(table.entries[i].is_active == false);
    }
}

TEST(test_create_binding) {
    BindingTable table;
    binding_table_init(&table);
    
    /* Create a binding */
    bool result = create_binding(&table, 1, 100, PERM_READ | PERM_WRITE);
    
    ASSERT(result == true);
    ASSERT(table.count == 1);
    
    /* Verify the binding exists */
    BindingEntry* entry = find_binding(&table, 1, 100);
    ASSERT(entry != NULL);
    ASSERT(entry->app_id == 1);
    ASSERT(entry->physical_page == 100);
    ASSERT(entry->permissions == (PERM_READ | PERM_WRITE));
}

TEST(test_remove_binding) {
    BindingTable table;
    binding_table_init(&table);
    
    /* Create and then remove a binding */
    create_binding(&table, 1, 100, PERM_READ);
    ASSERT(table.count == 1);
    
    bool result = remove_binding(&table, 1, 100);
    ASSERT(result == true);
    ASSERT(table.count == 0);
    
    /* Verify binding is gone */
    BindingEntry* entry = find_binding(&table, 1, 100);
    ASSERT(entry == NULL);
}

TEST(test_multiple_bindings) {
    BindingTable table;
    binding_table_init(&table);
    
    /* Create multiple bindings */
    create_binding(&table, 1, 100, PERM_READ);
    create_binding(&table, 1, 101, PERM_WRITE);
    create_binding(&table, 2, 200, PERM_EXECUTE);
    
    ASSERT(table.count == 3);
    
    /* Verify each binding */
    ASSERT(find_binding(&table, 1, 100) != NULL);
    ASSERT(find_binding(&table, 1, 101) != NULL);
    ASSERT(find_binding(&table, 2, 200) != NULL);
    
    /* Non-existent bindings should return NULL */
    ASSERT(find_binding(&table, 1, 200) == NULL);
    ASSERT(find_binding(&table, 3, 100) == NULL);
}

/* =============================================================================
 * TESTS: ACCESS CONTROL
 * =============================================================================
 */

TEST(test_access_granted) {
    BindingTable table;
    binding_table_init(&table);
    
    create_binding(&table, 1, 100, PERM_READ | PERM_WRITE);
    
    /* Read should be granted */
    AccessResult result = check_access(&table, 1, 100, PERM_READ);
    ASSERT(result == ACCESS_GRANTED);
    
    /* Write should be granted */
    result = check_access(&table, 1, 100, PERM_WRITE);
    ASSERT(result == ACCESS_GRANTED);
}

TEST(test_access_denied) {
    BindingTable table;
    binding_table_init(&table);
    
    create_binding(&table, 1, 100, PERM_READ); /* Only read permission */
    
    /* Write should be denied */
    AccessResult result = check_access(&table, 1, 100, PERM_WRITE);
    ASSERT(result == ACCESS_DENIED);
    
    /* Execute should be denied */
    result = check_access(&table, 1, 100, PERM_EXECUTE);
    ASSERT(result == ACCESS_DENIED);
}

TEST(test_access_no_binding) {
    BindingTable table;
    binding_table_init(&table);
    
    /* No bindings exist - should fail */
    AccessResult result = check_access(&table, 1, 100, PERM_READ);
    ASSERT(result == ACCESS_NO_BINDING);
}

/* =============================================================================
 * TESTS: SYSTEM CALLS
 * =============================================================================
 */

TEST(test_syscall_bind) {
    BindingTable table;
    binding_table_init(&table);
    
    SyscallRequest req = {
        .type = SYS_BIND,
        .app_id = 1,
        .page = 100,
        .permissions = PERM_READ
    };
    
    SyscallResponse resp = syscall_handler(&table, req);
    ASSERT(resp.status == SYSCALL_SUCCESS);
    ASSERT(table.count == 1);
}

TEST(test_syscall_unbind) {
    BindingTable table;
    binding_table_init(&table);
    
    /* First bind */
    SyscallRequest bind_req = {
        .type = SYS_BIND,
        .app_id = 1,
        .page = 100,
        .permissions = PERM_READ
    };
    syscall_handler(&table, bind_req);
    
    /* Then unbind */
    SyscallRequest unbind_req = {
        .type = SYS_UNBIND,
        .app_id = 1,
        .page = 100,
        .permissions = 0
    };
    SyscallResponse resp = syscall_handler(&table, unbind_req);
    
    ASSERT(resp.status == SYSCALL_SUCCESS);
    ASSERT(table.count == 0);
}

TEST(test_syscall_access) {
    BindingTable table;
    binding_table_init(&table);
    
    /* Create a binding */
    create_binding(&table, 1, 100, PERM_READ);
    
    SyscallRequest req = {
        .type = SYS_ACCESS,
        .app_id = 1,
        .page = 100,
        .permissions = PERM_READ
    };
    
    SyscallResponse resp = syscall_handler(&table, req);
    ASSERT(resp.status == SYSCALL_SUCCESS);
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        EXOKERNEL PROTOTYPE - UNIT TESTS                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Binding Table Tests */
    printf("=== Secure Binding Table Tests ===\n");
    RUN_TEST(test_binding_table_init);
    RUN_TEST(test_create_binding);
    RUN_TEST(test_remove_binding);
    RUN_TEST(test_multiple_bindings);
    printf("\n");
    
    /* Access Control Tests */
    printf("=== Access Control Tests ===\n");
    RUN_TEST(test_access_granted);
    RUN_TEST(test_access_denied);
    RUN_TEST(test_access_no_binding);
    printf("\n");
    
    /* System Call Tests */
    printf("=== System Call Tests ===\n");
    RUN_TEST(test_syscall_bind);
    RUN_TEST(test_syscall_unbind);
    RUN_TEST(test_syscall_access);
    printf("\n");
    
    /* Summary */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  TEST RESULTS: %d/%d tests passed\n", tests_passed, tests_run);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    if (tests_passed == tests_run) {
        printf("  ✓ All tests passed!\n");
    } else {
        printf("  ✗ Some tests failed.\n");
    }
    printf("\n");
    
    return (tests_passed == tests_run) ? 0 : 1;
}
