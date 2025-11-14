@echo off
echo ========================================
echo   HIDO - Flash via DFU (Simple Mode)
echo ========================================
echo.

REM Check if firmware exists
if not exist "..\build\hido.bin" (
    echo ERROR: Firmware file not found: ..\build\hido.bin
    echo Please compile the firmware first!
    pause
    exit /b 1
)

echo Firmware found: ..\build\hido.bin
echo.

echo Waiting for STM32 DFU device...
echo.
echo MANUAL DFU ENTRY REQUIRED:
echo   1. Disconnect USB cable
echo   2. Short BOOT0 (pin 60) to 3.3V with a wire/jumper
echo   3. Reconnect USB while holding the connection
echo   4. Release after 1 second
echo.
echo Press any key when device is connected in DFU mode...
pause >nul

REM Check for DFU device
echo.
echo Checking for DFU device...
dfu-util.exe -l | findstr /C:"0483:df11" >nul
if errorlevel 1 (
    echo.
    echo ERROR: STM32 DFU device not detected!
    echo.
    echo Troubleshooting:
    echo   - Check if BOOT0 is connected to 3.3V during USB connect
    echo   - Try different USB port
    echo   - Install STM32 DFU drivers (Windows only)
    echo.
    pause
    exit /b 1
)

echo STM32 DFU device found!
echo.

echo ========================================
echo   FLASHING FIRMWARE...
echo ========================================
echo.

REM Flash firmware
dfu-util.exe -a 0 -s 0x08000000:leave -D "..\build\hido.bin"

if errorlevel 1 (
    echo.
    echo ========================================
    echo   FLASH FAILED!
    echo ========================================
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo   FLASH SUCCESSFUL!
echo ========================================
echo.
echo Device should automatically restart.
echo If not, disconnect and reconnect USB.
echo.
pause
