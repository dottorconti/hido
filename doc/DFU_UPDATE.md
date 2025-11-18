# HIDO Firmware Update Guide (Coming Soon)

## Overview

HIDO supports firmware updates via USB DFU (Device Firmware Update) without needing an ST-Link programmer.

## 🔧 Hardware Setup

The PCB includes a bootloader entry circuit:
- **PD2** (STM32) → **R resistor** → **BOOT0**
- **Capacitor** on BOOT0 line holds voltage during reset
- **BOOT0 = HIGH** at power-on → STM32 enters USB DFU bootloader

### Circuit Diagram
```
PD2 (STM32) ──[R]──┬── BOOT0 (STM32)
                    │
                   [C] Capacitor
                    │
                   GND
```

**Component Values:**
- **R**: Recommended **1kΩ - 2.2kΩ** (limits current, faster charge)
  - 10kΩ works but is slower (requires longer delay)
- **C**: **1µF - 4.7µF** (holds BOOT0 during reset)

**Operation:**
When PD2 = HIGH, capacitor charges through R resistor (time constant τ = R×C), holding BOOT0 HIGH long enough for system reset to complete and STM32 to sample BOOT0 state.

## 📦 Requirements

### Windows
```powershell
# Option 1: Chocolatey
choco install dfu-util

# Option 2: Manual download
# Download from: http://dfu-util.sourceforge.net/
```

### Linux
```bash
sudo apt install dfu-util
```

### macOS
```bash
brew install dfu-util
```

### Python (Optional - for automated script)
```bash
pip install pyusb libusb
```

## 🚀 Update Methods

### Method 1: Automatic Script (Recommended)

#### Windows
```powershell
cd firmware
.\dfu_update.bat
```

#### Python (Cross-platform)
```bash
cd firmware
python dfu_update.py
```

The script will:
1. Auto-detect firmware file (`build/hido.bin`)
2. Check for HIDO device via USB
3. **Send bootloader entry command** (USB Control Transfer)
4. Device automatically resets into DFU mode
5. Flash firmware using dfu-util
6. Device auto-resets and runs new firmware

**No manual BOOT button needed!** The Python script triggers DFU entry automatically.

### Method 2: Manual Entry + Flash

#### Step 1: Enter DFU Bootloader

**Hardware Method (Current):**
1. Disconnect USB cable
2. Short **BOOT0** pin to **3.3V** (or press BOOT button if available)
3. Connect USB while holding BOOT0 HIGH
4. Release BOOT0

**Software Method (Future - requires firmware update):**
- Send USB command to trigger `DFU_EnterBootloader()`
- Device automatically resets into DFU mode

#### Step 2: Verify DFU Mode
```bash
dfu-util -l
```

Expected output:
```
Found DFU: [0483:df11] ver=2200, devnum=X, cfg=1, intf=0, alt=0, name="@Internal Flash  /0x08000000/064*0001Kg"
```

#### Step 3: Flash Firmware
```bash
dfu-util -a 0 -s 0x08000000:leave -D build/hido.bin
```

Explanation:
- `-a 0` = Alt setting 0 (internal flash)
- `-s 0x08000000` = Flash start address
- `:leave` = Auto-reset after flashing
- `-D file.bin` = Firmware file to flash

#### Step 4: Verify
Device should automatically restart and enumerate as HID device.

## 🔍 Troubleshooting

### DFU Device Not Detected

**Windows:**
- Install STM32 DFU drivers from ST website
- Or use Zadig to install WinUSB driver for DFU device (VID:0483 PID:DF11)

**Linux:**
- Check USB permissions: `sudo dfu-util -l`
- Add udev rule:
  ```bash
  echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE="0666"' | sudo tee /etc/udev/rules.d/50-stm32-dfu.rules
  sudo udevadm control --reload-rules
  ```

**macOS:**
- No special setup needed with Homebrew dfu-util

### Flash Fails

**"Cannot open DFU device":**
- Close STM32CubeProgrammer if running
- Close other DFU tools
- Try different USB port
- Try as administrator/sudo

**"Verification failed":**
- Try without `:leave` option
- Manually reset after flash
- Check firmware file size (must fit in 128KB)

**Device stuck in DFU:**
- Disconnect USB
- Power cycle
- Connect without BOOT0 held HIGH

## 🛠️ Developer Notes

### Firmware Implementation

The firmware includes `dfu_bootloader.c/h` module:

```c
#include "dfu_bootloader.h"

// Initialize in main()
DFU_Init();

// Trigger bootloader entry (e.g., from USB command)
DFU_EnterBootloader(100); // 100ms delay for USB cleanup
```

### How It Works

1. **PD2 = HIGH** → Current flows through R resistor → charges capacitor
2. Capacitor voltage holds **BOOT0 = HIGH**
3. **NVIC_SystemReset()** resets MCU
4. During reset, capacitor keeps BOOT0 HIGH (slow discharge through R)
5. STM32 samples BOOT0 at power-on → sees HIGH → enters ROM bootloader
6. ROM bootloader exposes USB DFU interface (VID:0483 PID:DF11)
7. PC flashes new firmware to 0x08000000
8. DFU bootloader resets MCU
9. Capacitor discharged → BOOT0 = LOW → STM32 boots normally from flash

### Component Selection Guide

**If you need to change the resistor value:**

| R Value | Current | Charge Time (C=1µF) | Firmware Delay | Notes |
|---------|---------|---------------------|----------------|-------|
| 470Ω | 7mA | ~2.5ms | 10ms | ⚠️ High current, needs longer firmware delay |
| **1kΩ** | **3.3mA** | **5ms** | **10ms** | ✅ **Best choice** - fast and safe |
| **2.2kΩ** | **1.5mA** | **11ms** | **15ms** | ✅ Very safe, slightly slower |
| 4.7kΩ | 0.7mA | ~24ms | 30ms | ⚠️ Slow but very low current |
| 10kΩ | 0.33mA | ~50ms | 60ms | ⚠️ Too slow, not recommended |

**Update firmware delay in `dfu_bootloader.h`:**
```c
#define BOOT_CHARGE_DELAY_MS    20  // Set to 2× charge time for safety
```

### Memory Layout

```
0x0000 0000  System Memory (ROM Bootloader - DFU)
0x0800 0000  Flash Start (User Application)
0x0801 FFFF  Flash End (128KB for STM32F102RB)
0x2000 0000  RAM Start
0x2000 4FFF  RAM End (20KB)
```

### USB Command Integration (TODO)

To enable automatic bootloader entry without BOOT button:

**Option 1: HID Feature Report**
```c
// In usbd_hid.c - handle feature report 0xF0
if (req->wValue == 0xF0) {
    DFU_EnterBootloader(100);
}
```

**Option 2: Vendor-Specific Control Transfer**
```c
// In usbd_hid.c or usbd_conf.c
case USB_REQ_TYPE_VENDOR:
    if (req->bRequest == 0xBB) { // Bootloader command
        DFU_EnterBootloader(100);
    }
    break;
```

**Option 3: Magic Key Sequence**
```c
// In arcade_keyboard.c
if (key_combo == SPECIAL_BOOTLOADER_COMBO) {
    LED_Blink_Pattern(); // Visual feedback
    DFU_EnterBootloader(1000);
}
```

## 📚 References

- [dfu-util Documentation](http://dfu-util.sourceforge.net/)
- [STM32 AN2606](https://www.st.com/resource/en/application_note/an2606-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) - System memory boot mode
- [USB DFU Specification](https://www.usb.org/sites/default/files/DFU_1.1.pdf)
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) - Alternative GUI tool

## 🔐 Security Considerations

**Production devices should implement:**
- Firmware signature verification
- Encrypted firmware images
- Bootloader password protection
- Flash readout protection (RDP Level 1 or 2)

**Current implementation:** None (open development board)

## ⚠️ Warnings

- **DO NOT** flash corrupted firmware - device will be bricked (requires ST-Link recovery)
- **ALWAYS** verify firmware file integrity before flashing
- **BACKUP** working firmware before testing new versions
- **TEST** on development board before production deployment

---

*Last updated: November 13, 2025*
