param(
    [ValidateSet('keyboard','joystick','jvs')]
    [string]$Mode
)

# Build script for HIDO Arcade Keyboard
# Requires ARM GCC toolchain in PATH

Write-Host "=== HIDO Arcade Keyboard Build ===" -ForegroundColor Cyan
Write-Host ""

# If a Mode is supplied, delegate to the direct compiler wrapper so users
# can build specific modes without editing source headers.
if ($PSBoundParameters.ContainsKey('Mode')) {
    Write-Host "Mode parameter detected: $Mode" -ForegroundColor Yellow
    Write-Host "Delegating to .\compile_direct.ps1 -Mode $Mode" -ForegroundColor Yellow
    & .\compile_direct.ps1 -Mode $Mode
    exit $LASTEXITCODE
}

# Check if ARM GCC is available
$gccCheck = Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue
if (-not $gccCheck) {
    Write-Host "ERROR: ARM GCC toolchain not found in PATH!" -ForegroundColor Red
    Write-Host "Please install from: https://developer.arm.com/downloads/-/gnu-rm" -ForegroundColor Yellow
    Write-Host "Or use STM32CubeIDE to build the project." -ForegroundColor Yellow
    exit 1
}

Write-Host "Toolchain found: $($gccCheck.Source)" -ForegroundColor Green
Write-Host ""

# Clean previous build
Write-Host "Cleaning previous build..." -ForegroundColor Yellow
if (Test-Path "build") {
    Remove-Item -Recurse -Force build
}

# Build project (default: use Makefile)
Write-Host "Building project using Makefile..." -ForegroundColor Yellow
$makeResult = make -j4 2>&1
$LASTEXITCODE_MAKE = $LASTEXITCODE

if ($LASTEXITCODE_MAKE -eq 0) {
    Write-Host ""
    Write-Host "=== BUILD SUCCESSFUL ===" -ForegroundColor Green
    Write-Host ""
    
    # Show binary size
    Write-Host "Binary sizes:" -ForegroundColor Cyan
    arm-none-eabi-size build/hido.elf
    
    Write-Host ""
    Write-Host "Output files:" -ForegroundColor Cyan
    Write-Host "  build/hido.elf - ELF binary for debugging" -ForegroundColor White
    Write-Host "  build/hido.hex - Intel HEX for flashing" -ForegroundColor White
    Write-Host "  build/hido.bin - Raw binary" -ForegroundColor White
    
} else {
    Write-Host ""
    Write-Host "=== BUILD FAILED ===" -ForegroundColor Red
    Write-Host $makeResult -ForegroundColor Red
    exit 1
}
