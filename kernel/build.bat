@echo off
set PATH=C:\w64devkit\bin;%PATH%
cd /d "%~dp0"
gcc -Wall -Wextra -I./include -o kernel_demo.exe src/secure_binding.c src/access_control.c src/syscall.c src/kernel.c src/main.c
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   BUILD SUCCESSFUL!
    echo ========================================
    echo   Run with: kernel_demo.exe
    echo.
) else (
    echo.
    echo BUILD FAILED
    echo.
)
pause
