@echo off
REM HIDO Configuration Tool - GUI Launcher
REM Quick launcher for the graphical configuration tool

echo ========================================
echo HIDO Configuration Tool GUI
echo ========================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python not found!
    echo Please install Python from https://www.python.org/
    echo.
    pause
    exit /b 1
)

REM Check if pyusb is installed
python -c "import usb.core" >nul 2>&1
if errorlevel 1 (
    echo WARNING: PyUSB not installed!
    echo Installing PyUSB...
    echo.
    pip install pyusb
    if errorlevel 1 (
        echo ERROR: Failed to install PyUSB
        echo Try manually: pip install pyusb
        echo.
        pause
        exit /b 1
    )
)

REM Launch GUI
echo Starting GUI...
echo.
python "%~dp0config_tool_gui.py"

if errorlevel 1 (
    echo.
    echo ERROR: GUI failed to start
    echo.
    echo Possible issues:
    echo   - Device not connected
    echo   - Driver not installed (use Zadig on Windows)
    echo   - PyUSB backend missing
    echo.
    echo See CONFIG_TOOLS_README.md for help
    pause
)
