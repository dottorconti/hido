@echo off
REM Check if dfu-util is installed and provide installation instructions

echo ============================================================
echo   HIDO - dfu-util Installation Check
echo ============================================================
echo.

where dfu-util >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] dfu-util is installed!
    echo.
    dfu-util --version
    echo.
    echo You're ready to update HIDO firmware.
    pause
    exit /b 0
)

echo [ERROR] dfu-util is NOT installed!
echo.
echo dfu-util is required to flash firmware via USB DFU.
echo.
echo === INSTALLATION OPTIONS ===
echo.
echo 1. Chocolatey (Recommended - requires admin):
echo    choco install dfu-util
echo.
echo 2. WinGet:
echo    winget install dfu-util
echo.
echo 3. Manual Download:
echo    - Visit: http://dfu-util.sourceforge.net/releases/
echo    - Download: dfu-util-0.11-binaries.tar.xz
echo    - Extract dfu-util.exe to this folder or add to PATH
echo.
echo 4. Portable (no installation):
echo    - Download dfu-util.exe
echo    - Place in same folder as this script
echo.
echo ============================================================
echo.
pause
