@echo off
set PATH=C:\w64devkit\bin;%PATH%
cd /d "%~dp0"
gcc -Wall -Wextra -I./include -o test_kernel.exe src/secure_binding.c src/access_control.c src/syscall.c src/kernel.c tests/test_kernel.c
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
