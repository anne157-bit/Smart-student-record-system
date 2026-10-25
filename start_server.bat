@echo off
title Smart Student Record System (SSRS) — C++ Backend
color 0b
echo ============================================================
echo   Smart Student Record System (SSRS)
echo   C++ REST API Backend Server + Dynamic Web Interface
echo ============================================================
echo.

cd /d "%~dp0"

if not exist server.exe (
    echo [INFO] server.exe not found. Compiling server.cpp with g++...
    g++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32
    if errorlevel 1 (
        echo [ERROR] Compilation failed. Please ensure g++ is installed and in your PATH.
        pause
        exit /b 1
    )
    echo [OK] Compilation successful!
    echo.
)

echo [INFO] Launching Web Interface in your default browser...
start http://localhost:8080/

echo [INFO] Starting C++ Backend Server on port 8080...
echo [INFO] Press Ctrl+C in this window to stop the server anytime.
echo.
.\server.exe 8080
pause
