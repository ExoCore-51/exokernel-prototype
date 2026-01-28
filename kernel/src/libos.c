#include "libos.h"
#include "logging.h"

uint32_t libos_alloc_and_bind(uint32_t app_id, uint8_t permissions) {
    SyscallRequest alloc = { .type = SYS_ALLOC_PAGE, .app_id = app_id, .page = 0, .permissions = 0 };
    SyscallResponse r1 = kernel_syscall(alloc);
    if (r1.status != SYSCALL_SUCCESS) {
        log_json("libos", "alloc_and_bind", app_id, 0, permissions, "ERROR", "Allocation failed", r1.data, 0);
        return INVALID_PAGE;
    }
    uint32_t page = r1.data;
    if (!libos_bind(app_id, page, permissions)) {
        /* Best-effort free on bind failure */
        SyscallRequest fr = { .type = SYS_FREE_PAGE, .app_id = app_id, .page = page, .permissions = 0 };
        kernel_syscall(fr);
        log_json("libos", "alloc_and_bind", app_id, page, permissions, "ERROR", "Bind failed", page, 0);
        return INVALID_PAGE;
    }
    log_json("libos", "alloc_and_bind", app_id, page, permissions, "SUCCESS", "Allocated and bound page", page, 0);
    return page;
}

bool libos_free_and_unbind(uint32_t app_id, uint32_t page) {
    bool ok = libos_unbind(app_id, page);
    SyscallRequest fr = { .type = SYS_FREE_PAGE, .app_id = app_id, .page = page, .permissions = 0 };
    SyscallResponse r = kernel_syscall(fr);
    bool freed = (r.status == SYSCALL_SUCCESS);
    log_json("libos", "free_and_unbind", app_id, page, 0, freed ? "SUCCESS" : "ERROR", freed ? "Freed page" : "Free failed", page, 0);
    return ok && freed;
}

bool libos_bind(uint32_t app_id, uint32_t page, uint8_t permissions) {
    SyscallRequest req = { .type = SYS_BIND, .app_id = app_id, .page = page, .permissions = permissions };
    SyscallResponse resp = kernel_syscall(req);
    bool ok = (resp.status == SYSCALL_SUCCESS);
    log_json("libos", "bind", app_id, page, permissions, ok ? "SUCCESS" : "ERROR", ok ? "Bound" : "Bind failed", page, 0);
    return ok;
}

bool libos_unbind(uint32_t app_id, uint32_t page) {
    SyscallRequest req = { .type = SYS_UNBIND, .app_id = app_id, .page = page, .permissions = 0 };
    SyscallResponse resp = kernel_syscall(req);
    bool ok = (resp.status == SYSCALL_SUCCESS);
    log_json("libos", "unbind", app_id, page, 0, ok ? "SUCCESS" : "ERROR", ok ? "Unbound" : "Unbind failed", page, 0);
    return ok;
}

bool libos_check(uint32_t app_id, uint32_t page, uint8_t permissions) {
    SyscallRequest req = { .type = SYS_ACCESS, .app_id = app_id, .page = page, .permissions = permissions };
    SyscallResponse resp = kernel_syscall(req);
    bool ok = (resp.status == SYSCALL_SUCCESS);
    log_json("libos", "check_access", app_id, page, permissions, ok ? "SUCCESS" : "DENIED", ok ? "Access granted" : "Access denied", ok ? 1 : 0, 0);
    return ok;
}

void libos_run_app(uint32_t app_id, const char* name, libos_app_entry entry) {
    log_json("libos", "app_start", app_id, 0, 0, "OK", name ? name : "app", 0, 0);
    if (entry) entry();
    log_json("libos", "app_exit", app_id, 0, 0, "OK", name ? name : "app", 0, 0);
}
