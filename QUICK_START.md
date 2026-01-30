# Exokernel Prototype - Quick Start Guide

## Getting Started

### Step 1: Build the Kernel
```powershell
cd kernel
.\build.bat
```

### Step 2: Start the Monitor Server
Open a **new terminal** and run:
```powershell
cd monitor
node server.js
```
Keep this running! Open **http://localhost:3000** in your browser.

### Step 3: Run the Kernel CLI
In another terminal:
```powershell
cd kernel
.\kernel_demo.exe
```

---

## CLI Commands Reference

### Memory Management

| Command | Description | Example |
|---------|-------------|---------|
| `alloc <app_id>` | Allocate a page with RW permissions | `alloc 10` |
| `free <app_id> <page>` | Free a page | `free 10 0` |

**Example Session:**
```
exokernel> alloc 10
[OK] Allocated page 0 for app 10 (READ|WRITE)

exokernel> alloc 10
[OK] Allocated page 1 for app 10 (READ|WRITE)

exokernel> free 10 0
[OK] Freed page 0 for app 10
```

---

### Binding Management

| Command | Description | Example |
|---------|-------------|---------|
| `bind <app_id> <page> <perms>` | Bind a page with permissions | `bind 10 5 rwx` |
| `unbind <app_id> <page>` | Remove a binding | `unbind 10 5` |

**Permissions:** `r` = read, `w` = write, `x` = execute

**Example Session:**
```
exokernel> bind 10 5 rw
[OK] Bound page 5 to app 10 with READ WRITE

exokernel> bind 20 10 rwx
[OK] Bound page 10 to app 20 with READ WRITE EXEC

exokernel> unbind 10 5
[OK] Unbound page 5 from app 10
```

---

### Access Control

| Command | Description | Example |
|---------|-------------|---------|
| `check <app_id> <page> <perms>` | Check if access is allowed | `check 10 5 r` |

**Example Session:**
```
exokernel> alloc 10
[OK] Allocated page 0 for app 10 (READ|WRITE)

exokernel> check 10 0 r
[OK] Access GRANTED: app 10 can access page 0 with READ

exokernel> check 10 0 x
[DENIED] Access DENIED: app 10 cannot access page 0 with EXEC

exokernel> check 20 0 r
[DENIED] Access DENIED: app 20 cannot access page 0 (no binding)
```

---

### Information Commands

| Command | Description |
|---------|-------------|
| `list` | Show all active bindings |
| `info` | Show kernel information |
| `status` | Show kernel status summary |
| `memory` or `mem` | Show memory statistics |
| `version` or `ver` | Show kernel version |

**Example Output:**
```
exokernel> list
Active Bindings:
----------------
  App 10 -> Page 0 [READ WRITE ]
  App 10 -> Page 1 [READ WRITE ]
Total: 2 binding(s)

exokernel> status
=== KERNEL STATUS ===
  State:     [RUNNING]
  Syscalls:  15 processed
  Bindings:  2 active
  Memory:    1022 / 1024 pages free

exokernel> memory
=== MEMORY STATUS ===
  Total Pages:     1024
  Free Pages:      1022
  Used Pages:      2
  Usage:           0.2%
```

---

### Utility Commands

| Command | Description |
|---------|-------------|
| `help` or `?` | Show all commands |
| `demo` | Run a quick automated demo |
| `clear` or `cls` | Clear the screen |
| `exit` or `quit` | Shutdown and exit |

---

## Demo Workflow

Try this sequence to demonstrate all features:

```
# 1. Show initial state
list
info

# 2. Allocate pages for two apps
alloc 100
alloc 200

# 3. Check the bindings
list

# 4. Test access control
check 100 0 r      # Should be GRANTED
check 100 0 x      # Should be DENIED (no execute permission)
check 200 0 r      # Should be DENIED (different app)

# 5. Create custom binding with execute
bind 100 5 rwx

# 6. Verify execute works now
check 100 5 x      # Should be GRANTED

# 7. Revoke access
unbind 100 5
check 100 5 x      # Should be DENIED

# 8. Show memory usage
memory

# 9. Clean up
free 100 0
free 200 1
list

# 10. Exit
exit
```

---

## Monitor Dashboard

The monitor at **http://localhost:3000** shows:
- **Syscalls**: Total system calls processed
- **Active Bindings**: Current app→page mappings
- **Events Logged**: All kernel events
- **Free Pages**: Available memory

The dashboard auto-updates every 2 seconds as you use the CLI!

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                    YOUR TERMINAL                        │
│                                                         │
│  exokernel> alloc 10                                   │
│             ↓                                          │
│         [LibOS Layer]                                  │
│             ↓                                          │
│         [Kernel Core] ──→ kernel.log                   │
│                              ↓                         │
└─────────────────────────────────────────────────────────┘
                               ↓
┌─────────────────────────────────────────────────────────┐
│                MONITOR (Browser)                        │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐      │
│  │Syscalls │ │Bindings │ │ Events  │ │ Memory  │      │
│  │   15    │ │    2    │ │   23    │ │ 1022    │      │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘      │
└─────────────────────────────────────────────────────────┘
```

---

## Quick Commands Cheat Sheet

```
# Start everything
cd kernel && .\build.bat && .\kernel_demo.exe

# In another terminal
cd monitor && node server.js

# Browser
http://localhost:3000
```

---

*Exokernel Prototype - Group 51 - 2026*
