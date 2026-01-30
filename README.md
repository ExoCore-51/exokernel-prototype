# Exokernel Prototype
# Group 51: Exokernel Implementation

This is the central hub for our Operating Systems project. We are building an Exokernel that focuses on **secure hardware multiplexing** and **minimalist abstractions**.

---

## 🏗️ Project Divisions

| Division | Primary Responsibility | Status |
|----------|------------------------|--------|
| **Kernel Core** | Low-level hardware multiplexing and secure resource allocation | ✅ Complete |
| **Memory Management** | Handling physical memory pages and protection domains | 🔜 Pending |
| **CLI / LibOS** | Creating the interface for users to interact with the kernel | 🔜 Pending |
| **React Monitor** | A web-based dashboard to visualize kernel status and logs | 🔜 Pending |

---

## 📖 Key Project Terms

> **Secure Bindings**: A one-time permission check that creates a direct hardware link for the app.
>
> **LibOS**: The Operating System features that run in the app's space, not the kernel's.
>
> **Visible Revocation**: When the kernel asks the app to release memory instead of forcing it.

---

## 🧠 Kernel Core (Complete)

The Kernel Core is the heart of our exokernel. It provides **secure hardware multiplexing** with minimal abstractions.

### Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     KERNEL CORE                              │
├─────────────────┬─────────────────┬───────────────────────────┤
│ Secure Binding  │ Access Control  │ System Call Interface    │
│ Table           │ (check_access)  │ (LibOS requests)         │
├─────────────────┴─────────────────┴───────────────────────────┤
│                   Resource Revocation                         │
└─────────────────────────────────────────────────────────────┘
                              ↓
              ┌───────────────────────────────┐
              │   Physical Hardware Resources  │
              │   (Memory Pages, CPU, etc.)    │
              └───────────────────────────────┘
```

### Project Structure

```
kernel/
├── include/               # Header files (interfaces)
│   ├── kernel.h           # Main kernel header
│   ├── secure_binding.h   # Binding table structures
│   ├── access_control.h   # Access validation
│   └── syscall.h          # System call interface
│
├── src/                   # Implementation files
│   ├── main.c             # Entry point
│   ├── kernel.c           # Kernel lifecycle
│   ├── secure_binding.c   # Binding operations
│   ├── access_control.c   # check_access implementation
│   └── syscall.c          # Syscall handler
│
├── tests/
│   └── test_kernel.c      # Unit tests (10 tests)
│
├── build.bat              # Build script for Windows
├── test.bat               # Test script for Windows
└── Makefile               # Build for Linux/Mac
```

### Module Breakdown

#### 1. Secure Binding Table (`secure_binding.h/.c`)

The core data structure of an exokernel. Maps applications to physical resources.

```c
typedef struct {
    uint32_t app_id;         // Application identifier
    uint32_t physical_page;  // Physical page number
    uint8_t  permissions;    // PERM_READ | PERM_WRITE | PERM_EXECUTE
    bool     is_active;      // Is this binding active?
} BindingEntry;
```

**Key Functions:**
- `binding_table_init()` - Initialize the table
- `create_binding(app_id, page, permissions)` - Grant access to a resource
- `remove_binding(app_id, page)` - Revoke access (visible revocation)
- `find_binding(app_id, page)` - Look up a binding

#### 2. Access Control (`access_control.h/.c`)

Validates permissions before allowing hardware access.

```c
AccessResult check_access(BindingTable* table, uint32_t app_id, 
                          uint32_t page, uint8_t requested_permission);
```

**Returns:**
- `ACCESS_GRANTED` - App has permission ✅
- `ACCESS_DENIED` - App lacks required permission ❌
- `ACCESS_NO_BINDING` - No binding exists for this app/resource

**How it works:**
```c
// Bitwise AND to check permissions
if ((binding->permissions & requested_permission) == requested_permission) {
    return ACCESS_GRANTED;
}
```

#### 3. System Call Interface (`syscall.h/.c`)

The interface that LibOS uses to communicate with the kernel.

**Supported System Calls:**
| Syscall | Purpose |
|---------|---------|
| `SYS_BIND` | Request a secure binding to a resource |
| `SYS_UNBIND` | Release a secure binding |
| `SYS_ACCESS` | Check if access is permitted |
| `SYS_INFO` | Get kernel information |
| `SYS_SHUTDOWN` | Request kernel shutdown |

**Example Request:**
```c
SyscallRequest req = {
    .type = SYS_BIND,
    .app_id = 1,
    .page = 100,
    .permissions = PERM_READ | PERM_WRITE
};
SyscallResponse resp = kernel_syscall(req);
```

#### 4. Kernel Core (`kernel.h/.c`)

Main kernel lifecycle management.

```c
int kernel_init(void);      // Initialize all subsystems
void kernel_run(void);      // Main kernel loop (demo)
void kernel_shutdown(void); // Clean shutdown
```

### How to Build & Run

#### Prerequisites
- GCC compiler (we use w64devkit on Windows)
- Located at `C:\w64devkit`

#### Build the Demo
```cmd
cd kernel
build.bat
```

#### Run the Demo
```cmd
kernel_demo.exe
```

#### Run Unit Tests
```cmd
test.bat
```

### Test Results

All **10 unit tests** pass:

| Category | Tests | Status |
|----------|-------|--------|
| Secure Binding Table | 4 | ✅ |
| Access Control | 3 | ✅ |
| System Calls | 3 | ✅ |

---

## 📝 Project Workflow & Rules

1. **The Dev Folder**: All teammates must push their work into the `dev/` folder. Do not push directly to the root directory.
2. **Branching**: Do not push directly to the `main` branch. Create a branch like `dev-yourname` first to keep the work safe.
3. **Merging**: I will be responsible for reviewing all code and documentation. Once everything is verified, I will merge it from the `dev/` folder into the main project.
4. **Research**: Upload your individual research findings to the `dev/findings/` folder within the repository.

---

## 📚 Resources

- [w64devkit](https://github.com/skeeto/w64devkit) - Windows C compiler

---

Created by Group 51 - 2026
