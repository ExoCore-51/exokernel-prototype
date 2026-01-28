@echo off
REM Prefer existing GCC in PATH; fallback to common locations (avoid paren blocks)
where gcc >nul 2>nul
if errorlevel 1 if exist C:\mingw64\bin\gcc.exe set PATH=C:\mingw64\bin;%PATH%
where gcc >nul 2>nul
if errorlevel 1 if exist C:\msys64\mingw64\bin\gcc.exe set PATH=C:\msys64\mingw64\bin;%PATH%
where gcc >nul 2>nul
if errorlevel 1 if exist C:\msys64\ucrt64\bin\gcc.exe set PATH=C:\msys64\ucrt64\bin;%PATH%
where gcc >nul 2>nul
if errorlevel 1 if exist C:\w64devkit\bin\gcc.exe set PATH=C:\w64devkit\bin;%PATH%
cd /d "%~dp0"
gcc -Wall -Wextra -I./include -o test_kernel.exe ^
    src/secure_binding.c src/access_control.c src/syscall.c src/kernel.c ^
    src/memory.c src/logging.c src/libos.c tests/test_kernel.c
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Running unit tests...
    echo.
    test_kernel.exe
) else (
    echo.
    echo BUILD FAILED
    echo.
)
pause
