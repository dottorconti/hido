# Flash script for HIDO Arcade Keyboard
# Uses STM32CubeProgrammer CLI to flash the firmware

$ErrorActionPreference = "Stop"

Write-Host "================================" -ForegroundColor Cyan
Write-Host "  HIDO Firmware Flash Tool" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# STM32CubeProgrammer path
$STM32_CLI = "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"

# Check if CLI exists
if (-not (Test-Path $STM32_CLI)) {
    Write-Host "ERROR: STM32CubeProgrammer CLI not found!" -ForegroundColor Red
    Write-Host "Expected path: $STM32_CLI" -ForegroundColor Yellow
    Write-Host "Please install STM32CubeProgrammer from:" -ForegroundColor Yellow
    Write-Host "https://www.st.com/en/development-tools/stm32cubeprog.html" -ForegroundColor Yellow
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
