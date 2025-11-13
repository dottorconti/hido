@echo off
REM HIDO Firmware Update Script for Windows
REM Uses dfu-util to flash firmware via USB DFU

setlocal

echo ============================================================
echo   HIDO Firmware Updater
echo   STM32 USB DFU Flash Tool
echo ============================================================
echo.

REM Check if dfu-util is installed
where dfu-util >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] dfu-util not found!
    echo.
    echo Please install dfu-util:
    echo   Download: http://dfu-util.sourceforge.net/
    echo   Or use:   choco install dfu-util
    echo.
    pause
    exit /b 1
)

echo [OK] dfu-util found

REM Find firmware file
set FIRMWARE=
if exist "build\hido.bin" set FIRMWARE=build\hido.bin
if exist "hido.bin" set FIRMWARE=hido.bin
if exist "firmware.bin" set FIRMWARE=firmware.bin

if "%~1" NEQ "" set FIRMWARE=%~1

if "%FIRMWARE%"=="" (
    echo [ERROR] No firmware file found!
    echo.
    echo Usage: dfu_update.bat [firmware.bin]
    echo.
    pause
    exit /b 1
)

if not exist "%FIRMWARE%" (
    echo [ERROR] Firmware file not found: %FIRMWARE%
    echo.
    pause
    exit /b 1
)

echo [OK] Firmware file: %FIRMWARE%
for %%A in ("%FIRMWARE%") do echo      Size: %%~zA bytes
echo.

REM Check for DFU device
echo Checking for STM32 DFU device...
dfu-util -l | findstr /C:"STM32" >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] STM32 DFU device detected
    goto :flash
)

echo [WARN] STM32 DFU device not found
echo.
echo Please put device in DFU mode:
echo   1. Disconnect USB
echo   2. Hold BOOT button (or short BOOT0 to 3.3V^)
echo   3. Connect USB while holding BOOT
echo   4. Release BOOT button
echo.
echo Press any key when ready...
pause >nul

:flash
echo.
echo ============================================================
echo Flashing firmware...
echo ============================================================
echo.

dfu-util -a 0 -s 0x08000000:leave -D "%FIRMWARE%"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ============================================================
    echo [OK] Firmware update successful!
    echo      Device should automatically restart...
    echo ============================================================
) else (
    echo.
    echo ============================================================
    echo [ERROR] Firmware update failed!
    echo ============================================================
)

echo.
pause
