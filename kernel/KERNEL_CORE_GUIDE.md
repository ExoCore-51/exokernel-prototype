# 🎓 Kernel Core - Beginner's Guide

## What is This?

**Welcome, teammates!** This document explains what the Kernel Core does in simple terms. No prior C knowledge required!

---

## 🤔 The Big Picture: What's an Exokernel?

### Traditional Operating System (Windows, Linux)
```
Your App → OS (lots of layers) → Hardware
         ↑
         The OS decides HOW you access hardware
         (you can't control it directly)
```

### Exokernel (What We're Building)
```
Your App → Exokernel (tiny) → Hardware
         ↑
         The kernel only checks IF you're allowed
         (then gets out of the way)
```

**Think of it like:**
- Traditional OS = A strict librarian who finds books for you
- Exokernel = A security guard who just checks your ID, then lets you find books yourself

---

## 🧱 What Did We Build?

We built **4 main pieces** that work together:

### 1. 📋 Secure Binding Table
**What it is:** A list that tracks which app can access which memory.

**Real-world analogy:** Hotel key cards
- Guest 1 → Room 101, Room 102 (can enter these rooms)
- Guest 2 → Room 200 (can only enter this room)

**In our code:**
```
App 1 → Page 100 (Read, Write)    ← "App 1 can read/write page 100"
App 1 → Page 101 (Read, Execute)  ← "App 1 can read/run page 101"
App 2 → Page 200 (All permissions)
```

---

### 2. 🔐 Access Control (check_access)
**What it is:** The security guard that checks permissions.

**How it works:**
```
App 1 wants to READ page 100
↓
check_access looks at the Binding Table
↓
"Does App 1 have READ permission for page 100?"
↓
YES → ACCESS GRANTED ✅
NO  → ACCESS DENIED ❌
```

**The actual logic (simplified):**
```c
if (app has the permission in the table) {
    return "ACCESS GRANTED";
} else {
    return "ACCESS DENIED";
}
```

---

### 3. 📞 System Calls (syscall)
**What it is:** The "phone line" between apps and the kernel.

Apps can't directly touch the kernel. They have to make a "call" (request).

**Available calls:**
| Call | What it does |
|------|--------------|
| `SYS_BIND` | "Hey kernel, give me access to this page" |
| `SYS_UNBIND` | "I'm done with this page, take it back" |
| `SYS_ACCESS` | "Can I read/write this page?" |
| `SYS_INFO` | "How many bindings exist?" |
| `SYS_SHUTDOWN` | "Shut down the kernel" |

---

### 4. 🎛️ Kernel Core (kernel.c)
**What it is:** The main brain that starts everything.

```
kernel_init()   → Turns on the kernel, sets up tables
kernel_run()    → Runs the demo (creates bindings, checks access)
kernel_shutdown() → Cleans up and says goodbye
```

---

## 🔨 Challenges We Faced

### Challenge 1: Installing GCC (C Compiler)
**The Problem:** Windows doesn't come with a C compiler.

**What is GCC?** 
GCC turns our C code (human-readable) into an .exe file (computer can run).

```
Our Code (.c files) → GCC Compiler → kernel_demo.exe
```

**How We Solved It:**
1. Downloaded **w64devkit** from GitHub
2. Extracted it to `C:\w64devkit`
3. Added `C:\w64devkit\bin` to system PATH
4. Created `build.bat` so we don't have to type long commands

---

### Challenge 2: Paths With Spaces
**The Problem:** The username folder has spaces: `Ellise Grant Boamah`

GCC got confused with spaces in file paths.

**How We Solved It:**
Created a batch file (`build.bat`) that:
1. Sets up the PATH correctly
2. Changes to the right directory
3. Runs GCC with proper settings

Now you just double-click `build.bat` and it works!

---

### Challenge 3: Understanding Permissions (Bitwise Operations)
**The Problem:** How do we store multiple permissions in one number?

**The Solution: Bitwise Flags**
```
PERM_READ    = 001 (binary) = 1
PERM_WRITE   = 010 (binary) = 2
PERM_EXECUTE = 100 (binary) = 4
```

Combine them:
```
READ + WRITE = 001 + 010 = 011 = 3
READ + EXECUTE = 001 + 100 = 101 = 5
ALL PERMISSIONS = 111 = 7
```

To check if READ is allowed:
```c
if (permissions & PERM_READ) {
    // READ is allowed!
}
```

---

## 🚀 How to Run It Yourself

### Step 1: Make sure GCC is installed
- Should be at `C:\w64devkit`
- If not, download from: https://github.com/skeeto/w64devkit/releases

### Step 2: Build the kernel
```
1. Open the kernel folder
2. Double-click build.bat
3. You should see "BUILD SUCCESSFUL!"
```

### Step 3: Run the demo
```
1. Double-click kernel_demo.exe
2. Watch the kernel create bindings and check access!
```

### Step 4: Run tests
```
1. Double-click test.bat
2. All 10 tests should pass ✅
```

---

## 📊 What You'll See When You Run It

```
╔═══════════════════════════════════════════════════════════════╗
║        EXOKERNEL PROTOTYPE - GROUP 51                         ║
║        Version 0.1.0                                          ║
╚═══════════════════════════════════════════════════════════════╝

[KERNEL] Initializing kernel...
[KERNEL] Binding table initialized (capacity: 256)

--- Demo 1: Creating Secure Bindings ---
[KERNEL] Created binding: App 1 -> Page 100 (perms: RW-)
[KERNEL] Created binding: App 1 -> Page 101 (perms: R-X)
[KERNEL] Created binding: App 2 -> Page 200 (perms: RWX)

--- Demo 2: Access Control Checks ---
[ACCESS] GRANTED: App 1 can READ page 100
[ACCESS] DENIED: App 1 lacks permission EXECUTE for page 100
[ACCESS] DENIED: App 2 has no binding for page 100

--- Demo 3: Visible Revocation ---
[KERNEL] Removed binding: App 1 no longer has access to page 100

[KERNEL] Total syscalls processed: 8
Goodbye!
```

---

## 🎯 Summary for Your Report

> **What we built:** The Kernel Core of an Exokernel operating system.
>
> **What it does:** Securely manages which applications can access which physical memory pages, using minimal abstractions.
>
> **Key components:**
> 1. **Secure Binding Table** - Maps apps to resources with permissions
> 2. **Access Control** - Validates every access request
> 3. **System Calls** - Interface for apps to request resources
> 4. **Kernel Core** - Coordinates everything
>
> **Technologies used:** C programming language, GCC compiler, Windows batch scripts

---

## ❓ Questions?

If anything is unclear, ask in the group chat! Understanding the Kernel Core is key to building the next components (Memory Management, LibOS, React Monitor).

---

*Document created by Group 51 - 2026*
