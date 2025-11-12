# Flash script for HIDO Arcade Keyboard
# Uses STM32CubeProgrammer CLI to flash the firmware

$ErrorActionPreference = "Stop"

Write-Host "================================" -ForegroundColor Cyan
Write-Host "  HIDO Firmware Flash Tool" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# STM32CubeProgrammer path - try multiple common locations
$POSSIBLE_PATHS = @(
    "C:\ST\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe",
    "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe",
    "C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
)

$STM32_CLI = $null
foreach ($path in $POSSIBLE_PATHS) {
    if (Test-Path $path) {
        $STM32_CLI = $path
        Write-Host "Found STM32CubeProgrammer: $path" -ForegroundColor Green
        break
    }
}

# If not found in common locations, try to find it using Get-Command (if in PATH)
if (-not $STM32_CLI) {
    $cliInPath = Get-Command "STM32_Programmer_CLI.exe" -ErrorAction SilentlyContinue
    if ($cliInPath) {
        $STM32_CLI = $cliInPath.Source
        Write-Host "Found STM32CubeProgrammer in PATH: $STM32_CLI" -ForegroundColor Green
    }
}

# Check if CLI exists
if (-not $STM32_CLI) {
    Write-Host "ERROR: STM32CubeProgrammer CLI not found!" -ForegroundColor Red
    Write-Host "Searched in:" -ForegroundColor Yellow
    foreach ($path in $POSSIBLE_PATHS) {
        Write-Host "  - $path" -ForegroundColor Yellow
    }
    Write-Host ""
    Write-Host "Please either:" -ForegroundColor Yellow
    Write-Host "  1. Install STM32CubeProgrammer from:" -ForegroundColor White
    Write-Host "     https://www.st.com/en/development-tools/stm32cubeprog.html" -ForegroundColor White
    Write-Host "  2. Add STM32_Programmer_CLI.exe to your system PATH" -ForegroundColor White
    exit 1
}

# Check if firmware file exists
$HEX_FILE = "build\hido.hex"
if (-not (Test-Path $HEX_FILE)) {
    Write-Host "ERROR: Firmware file not found: $HEX_FILE" -ForegroundColor Red
    Write-Host "Please build the project first using compile_direct.ps1" -ForegroundColor Yellow
    exit 1
}

Write-Host "Firmware file: $HEX_FILE" -ForegroundColor Green
Write-Host ""

# Flash via ST-Link (USB)
Write-Host "Connecting to STM32 via ST-Link..." -ForegroundColor Yellow
Write-Host ""

# Command arguments for ST-Link connection and flashing
$FLASH_ARGS = @(
    "-c", "port=SWD",           # Connect via ST-Link (SWD interface)
    "-w", $HEX_FILE,            # Write hex file
    "-v",                       # Verify after programming
    "-rst",                     # Reset after programming
    "-run"                      # Run application after reset
)

# Execute flashing
& $STM32_CLI $FLASH_ARGS

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "================================" -ForegroundColor Green
    Write-Host "  Flashing completed!" -ForegroundColor Green
    Write-Host "================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "The board should now be running the new firmware." -ForegroundColor White
} else {
    Write-Host ""
    Write-Host "================================" -ForegroundColor Red
    Write-Host "  Flashing FAILED!" -ForegroundColor Red
    Write-Host "================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "Common issues:" -ForegroundColor Yellow
    Write-Host "  1. ST-Link not connected" -ForegroundColor White
    Write-Host "  2. Board not powered" -ForegroundColor White
    Write-Host "  3. Wrong connection (check SWD pins: SWDIO, SWCLK, GND, 3.3V)" -ForegroundColor White
    Write-Host "  4. ST-Link drivers not installed" -ForegroundColor White
    exit 1
}
