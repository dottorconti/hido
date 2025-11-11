# Direct compilation script - No Make required
# Compiles HIDO firmware using arm-none-eabi-gcc directly

$ErrorActionPreference = "Stop"

Write-Host "================================" -ForegroundColor Cyan
Write-Host "  HIDO Direct Compilation" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Check ARM GCC
if (-not (Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: ARM GCC not found!" -ForegroundColor Red
    exit 1
}

Write-Host "ARM GCC found: " -NoNewline
arm-none-eabi-gcc --version | Select-Object -First 1
Write-Host ""

# Build directory
$BUILD_DIR = "build"
if (Test-Path $BUILD_DIR) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

# Compiler flags
$MCU = "-mcpu=cortex-m3 -mthumb"
$DEFS = "-DUSE_HAL_DRIVER -DSTM32F102xB -DUSE_JOYSTICK_MODE"
$INCLUDES = @(
    "-ICore/Inc",
    "-IDrivers/STM32F1xx_HAL_Driver/Inc",
    "-IDrivers/STM32F1xx_HAL_Driver/Inc/Legacy",
    "-IDrivers/CMSIS/Device/ST/STM32F1xx/Include",
    "-IDrivers/CMSIS/Include",
    "-IUSB_DEVICE/App",
    "-IUSB_DEVICE/Target",
    "-IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc",
    "-IMiddlewares/ST/STM32_USB_Device_Library/Class/HID/Inc"
)
$CFLAGS = "$MCU $DEFS $($INCLUDES -join ' ') -Og -Wall -fdata-sections -ffunction-sections -g -gdwarf-2"
$LDFLAGS = "$MCU -specs=nano.specs -TSTM32F102RBTX_FLASH.ld -Wl,--gc-sections -Wl,-Map=$BUILD_DIR/hido.map"

# Source files
$C_SOURCES = @(
    "Core/Src/main.c",
    "Core/Src/gpio.c",
    "Core/Src/adc.c",
    "Core/Src/tim.c",
    "Core/Src/usart.c",
    "Core/Src/stm32f1xx_it.c",
    "Core/Src/stm32f1xx_hal_msp.c",
    "Core/Src/syscalls.c",
    "Core/Src/sysmem.c",
    "Core/Src/system_stm32f1xx.c",
    "Core/Src/arcade_joystick.c",
    "Core/Src/arcade_keyboard.c",
    "Core/Src/jvs_protocol.c",
    "Core/Src/usbd_hid_custom.c",
    "Core/Src/gpio_test.c",
    "USB_DEVICE/App/usb_device.c",
    "USB_DEVICE/App/usbd_desc.c",
    "USB_DEVICE/Target/usbd_conf.c",
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c",
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c",
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c",
    "Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c",
    "Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c"
)

$ASM_SOURCES = @(
    "Core/Startup/startup_stm32f102rbtx.s"
)

# Compile C sources
Write-Host "Compiling C sources..." -ForegroundColor Yellow
$OBJECTS = @()
$i = 0
foreach ($src in $C_SOURCES) {
    $i++
    $obj = "$BUILD_DIR/" + [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
    Write-Host "  [$i/$($C_SOURCES.Count)] $src" -ForegroundColor Gray
    
    $cmd = "arm-none-eabi-gcc -c $CFLAGS `"$src`" -o `"$obj`""
    Invoke-Expression $cmd
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR compiling $src" -ForegroundColor Red
        exit 1
    }
    $OBJECTS += $obj
}

# Compile ASM sources
Write-Host "Compiling ASM sources..." -ForegroundColor Yellow
foreach ($src in $ASM_SOURCES) {
    $obj = "$BUILD_DIR/" + [System.IO.Path]::GetFileNameWithoutExtension($src) + ".o"
    Write-Host "  $src" -ForegroundColor Gray
    
    $cmd = "arm-none-eabi-gcc -c $MCU -x assembler-with-cpp `"$src`" -o `"$obj`""
    Invoke-Expression $cmd
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR compiling $src" -ForegroundColor Red
        exit 1
    }
    $OBJECTS += $obj
}

# Link
Write-Host "Linking..." -ForegroundColor Yellow
$LD_ARGS = @(
    "-mcpu=cortex-m3",
    "-mthumb",
    "-specs=nano.specs",
    "-T", "STM32F102RBTX_FLASH.ld",
    "-Wl,--gc-sections",
    "-Wl,-Map=$BUILD_DIR/hido.map",
    "-o", "$BUILD_DIR/hido.elf"
) + $OBJECTS

& arm-none-eabi-gcc $LD_ARGS
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR linking" -ForegroundColor Red
    exit 1
}

# Generate hex and bin
Write-Host "Generating HEX and BIN..." -ForegroundColor Yellow
arm-none-eabi-objcopy -O ihex "$BUILD_DIR/hido.elf" "$BUILD_DIR/hido.hex"
arm-none-eabi-objcopy -O binary -S "$BUILD_DIR/hido.elf" "$BUILD_DIR/hido.bin"

# Show size
Write-Host ""
Write-Host "Build successful!" -ForegroundColor Green
Write-Host ""
Write-Host "Memory usage:" -ForegroundColor Cyan
arm-none-eabi-size "$BUILD_DIR/hido.elf"

Write-Host ""
Write-Host "Output files:" -ForegroundColor Cyan
Write-Host "  $BUILD_DIR/hido.elf" -ForegroundColor White
Write-Host "  $BUILD_DIR/hido.hex" -ForegroundColor White
Write-Host "  $BUILD_DIR/hido.bin" -ForegroundColor White
