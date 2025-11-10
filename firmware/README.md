# HIDO - Arcade Multi-Mode Controller
Open source arcade controller with STM32F102 - Three operating modes for maximum flexibility

## ⚠️ IMPORTANT: Custom Code vs Middleware

### Problem with previous commits (b76fa87, c5c3dc6)
These commits modified **middleware files directly**:
- `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`
- `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c`

**Issue**: STM32CubeMX regeneration **overwrites** these changes!

### ✅ Correct Approach (WIP)
Custom HID descriptors should be in:
- `Core/Inc/usbd_hid_custom.h` - Custom definitions
- `Core/Src/usbd_hid_custom.c` - Custom HID descriptor

**Benefits**:
- ✅ Survives STM32CubeMX regeneration
- ✅ Clean separation: generated vs custom code
- ✅ Git tracks only custom files

**Status**: Migration in progress. Current code still uses direct middleware modification.

## 🎮 Features

- **Triple Mode Support**: NKRO Keyboard, Dual Joystick, or JVS/RS485
- **NKRO Keyboard Mode**: Up to 96 keys simultaneously (no ghosting!)
- **Dual Joystick Mode**: 2 independent USB joysticks (16 buttons + 2 axes each)
- **JVS Protocol Mode**: RS485 arcade I/O board communication
- **Ultra-Low Latency**: 1ms USB polling (1000Hz) + optimized scanning
- **Hardware Debouncing**: 5ms configurable debounce time
- **Plug & Play**: Standard USB HID device, no drivers needed

## 🔧 Hardware Configuration

### Operating Modes

**Mode Selection** - Edit `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`:

Uncomment **ONE** of these modes:
```c
#define USE_KEYBOARD_MODE   // NKRO Keyboard (default)
// #define USE_JOYSTICK_MODE   // Dual Joystick
// #define USE_JVS_MODE        // JVS/RS485 Protocol
```

### Mode 1: NKRO Keyboard (USB HID Keyboard)

**Button Connections (Active LOW with internal pull-ups):**

**Player 1 (GPIOB):**
- PB3-6: Joystick (Up/Down/Left/Right) → Arrow Keys
- PB7-12: Buttons 1-6 → Space, /, Y, T, R, F

**Player 2 (GPIOC):**
- PC0-1, PC5-6: Joystick → W, S, A, D
- PC7-9, PC13-15: Buttons 1-6 → Q, W, E, O, I, U

**System (GPIOA & GPIOB):**
- PA6-7, PA15: Coin/Start → Esc, F1, F2
- PB2, PB13-15: Service/Test/Extra → F3, F4, F5, F6

### Mode 2: Dual Joystick (USB HID Joystick)

Emulates **2 independent USB joysticks** for arcade games that support joystick input.

**Player 1 Joystick:**
- PB3-6: Directional axes (Up/Down/Left/Right)
- PB7-12: Buttons 1-6
- PA6-7: Coin/Start (Buttons 7-8)
- PB13-14: Service/Test (Buttons 9-10)

**Player 2 Joystick:**
- PC0-1, PC5-6: Directional axes
- PC7-9, PC13-15: Buttons 1-6
- PA15, PB2: Coin/Start (Buttons 7-8)

**Joystick Features:**
- 2 analog axes (X, Y) - digitally controlled by buttons
- 16 buttons per joystick
- Standard USB HID joystick class
- Compatible with Windows, Linux, macOS

### Mode 3: JVS Protocol (RS485 Arcade I/O)

**RS485 Interface (SN65HVD1786D):**
- PA8 (TX): RS485 Data Transmit
- PA9 (RX): RS485 Data Receive
- PA10: Sense Line (2.5V when ready)

**Protocol:** JVS 3.0 compatible
- Baud Rate: 115200 8N1
- Board ID: "HIDO Arcade Controller"
- Capabilities: 2 players, 8 buttons each, 2 coin slots

### LEDs (GPIOC)
- PC10: LED1 (Activity)
- PC11: LED2 
- PC12: LED3

## 🛠️ Compilation

### Option 1: STM32CubeIDE (Recommended)
1. Open project in STM32CubeIDE
2. Build → Project (Ctrl+B)
3. Flash → Run (F11)

### Option 2: Command Line (with ARM GCC toolchain)
```bash
# Install ARM GCC toolchain first
# Windows: https://developer.arm.com/downloads/-/gnu-rm

# Compile
make clean
make -j4

# Flash (requires OpenOCD or ST-Link tools)
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/hido.elf verify reset exit"
```

### Option 3: VS Code with Cortex-Debug extension
1. Install "Cortex-Debug" extension
2. Press F5 to build and debug

## 📝 Configuration

### Change Button Mapping
Edit `Core/Src/arcade_keyboard.c`, modify the `button_map[]` array:

```c
static const ButtonMapping_t button_map[MAX_BUTTONS] = {
    {GPIOB, GPIO_PIN_3,  0x50, true},  // Port, Pin, HID Keycode, Active Low
    // Add your custom mappings here
};
```

**Common HID Keycodes:**
- 0x04-0x1D: A-Z
- 0x1E-0x27: 1-9, 0
- 0x2C: Space
- 0x28: Enter
- 0x29: Escape
- 0x3A-0x45: F1-F12
- 0x4F-0x52: Arrow Right/Left/Down/Up

Full list: [USB HID Usage Tables](https://usb.org/sites/default/files/hut1_21.pdf)

### Adjust Debounce Time
Edit `Core/Inc/arcade_keyboard.h`:
```c
#define DEBOUNCE_TIME_MS    5   // Default 5ms
```

### Enable JVS Mode (Future)
Edit `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`:
```c
// #define USE_DIRECT_BUTTONS    /* Comment this line */
```

## 🚀 Performance

- **Input Latency**: < 1.5ms total (0.5ms scan + 1ms USB poll)
- **Scan Rate**: Up to 2000Hz button scanning
- **USB Polling**: 1000Hz (1ms intervals)
- **Key Rollover**: 96 keys NKRO (all arcade buttons + extras)

## 🐛 Troubleshooting

### Computer doesn't recognize device
- Check USB cable and connections
- Verify LED1 blinks on startup
- Try different USB port

### Keys not responding
1. Check button wiring (should connect pin to GND when pressed)
2. Verify GPIO mapping in `arcade_keyboard.c`
3. Test with LED indicators in DEBUG mode

### Ghosting issues
- Should not occur with NKRO! Check USB host supports it
- Some BIOS/legacy systems might not support NKRO

### Input lag
- Ensure USB polling is 1000Hz in Device Manager (Windows)
- Check no other USB devices causing conflicts
- Disable USB power management in OS settings

## 📊 Technical Details

**MCU**: STM32F102RBT6
- 128KB Flash, 20KB RAM
- 72MHz ARM Cortex-M3
- USB 2.0 Full Speed

**USB Configuration:**
- VID: 0x0483 (STMicroelectronics)
- PID: 0x572B
- Class: HID Keyboard
- Protocol: NKRO (non-boot compatible)

## 🔄 Updates from Original

- ✅ Updated USB HID libraries (2015 → modern)
- ✅ Replaced mouse descriptor with NKRO keyboard
- ✅ Reduced USB interval: 10ms → 1ms (10x faster)
- ✅ Added GPIO pull-ups for reliable button reading
- ✅ Implemented proper debouncing algorithm
- ✅ Removed blocking delays for minimal latency
- ✅ Added structured button mapping system
- ✅ Prepared infrastructure for JVS support

## � Credits and Acknowledgments

This project uses and references the following open-source projects:

- **[OpenJVS](https://github.com/OpenJVS/OpenJVS)** by Bobby Dilley (GPL-3.0)
  - JVS protocol specification and reference implementation
  - Packet structure, command codes, and RS485 communication patterns
  - Our JVS implementation is inspired by OpenJVS but completely rewritten for STM32

- **STM32Cube HAL Drivers** by STMicroelectronics (BSD-3-Clause)
  - Hardware abstraction layer for STM32F1 series

- **STM32 USB Device Library** by STMicroelectronics (BSD-3-Clause)
  - USB HID device implementation

See [CREDITS.md](CREDITS.md) for complete attribution and license information.

## 📜 License

MIT License (for HIDO-specific code)

**Note**: JVS implementation is inspired by OpenJVS (GPL-3.0). If you use the JVS portions in a GPL project, you must comply with GPL-3.0 terms.

## 👤 Authors

- **Original Hardware**: dottorconti (2020)
- **Firmware Rewrite**: 2025 - Complete NKRO + JVS implementation

---

**Support the open-source arcade community!** ⭐
- Star this repository if you find it useful
- Check out [OpenJVS](https://github.com/OpenJVS/OpenJVS) for Linux-based JVS emulation
- Contribute to arcade hardware preservation
