@echo off
REM Download dfu-util for Windows using PowerShell
REM Multiple mirrors for reliability

echo ============================================================
echo   Downloading dfu-util for Windows
echo ============================================================
echo.

echo Checking if dfu-util is already installed...
where dfu-util >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] dfu-util is already installed!
    dfu-util --version
    pause
    exit /b 0
)

if exist "dfu-util.exe" (
    echo [OK] dfu-util.exe found in current directory!
    .\dfu-util.exe --version
    pause
    exit /b 0
)

echo.
echo [INFO] dfu-util not found. Downloading...
echo.

REM Try multiple download sources
echo Trying mirror 1: GitHub releases...
powershell -Command "try { Invoke-WebRequest -Uri 'https://github.com/matheustavarestrindade/dfu-util-releases/releases/download/v0.11/dfu-util-static.exe' -OutFile 'dfu-util.exe' -UseBasicParsing -TimeoutSec 30; exit 0 } catch { exit 1 }"
if %ERRORLEVEL% EQU 0 goto :verify

echo Failed. Trying mirror 2: Alternative GitHub...
powershell -Command "try { $ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/arduino/ArduinoCore-renesas/main/extras/dfu-util/dfu-util-0.11-binaries/win64/dfu-util.exe' -OutFile 'dfu-util.exe' -UseBasicParsing -TimeoutSec 30; exit 0 } catch { exit 1 }"
if %ERRORLEVEL% EQU 0 goto :verify

echo Failed. Trying mirror 3: Direct binary...
powershell -Command "try { $ProgressPreference = 'SilentlyContinue'; [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; $url='https://github.com/rpicopter/ArduinoWeightSensor/raw/main/win64/dfu-util.exe'; (New-Object System.Net.WebClient).DownloadFile($url, 'dfu-util.exe'); exit 0 } catch { exit 1 }"
if %ERRORLEVEL% EQU 0 goto :verify

echo.
echo [ERROR] All download mirrors failed!
echo.
echo Please download manually:
echo 1. Visit: https://dfu-util.sourceforge.net/releases/
echo 2. Download: dfu-util-0.11-binaries.tar.xz
echo 3. Extract win64\dfu-util.exe to this folder
echo.
echo Or download directly:
echo https://github.com/matheustavarestrindade/dfu-util-releases/releases/download/v0.11/dfu-util-static.exe
echo.
pause
exit /b 1

:verify
echo.
echo [OK] dfu-util downloaded successfully!
echo.
echo Verifying...
.\dfu-util.exe --version
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Downloaded file is corrupted!
    del dfu-util.exe
    pause
    exit /b 1
)

echo.
echo ============================================================
echo [SUCCESS] dfu-util is ready to use!
echo ============================================================
echo.
echo Location: %CD%\dfu-util.exe
echo.
echo You can now use:
echo   .\dfu-util.exe -l           (list DFU devices)
echo   .\dfu_update.bat            (update HIDO firmware)
echo.
pause
