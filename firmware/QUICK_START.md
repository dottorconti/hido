# Quick Start Guide - HIDO Arcade Controller

## Mode Selection

**Files to edit:** 
1. `Core/Inc/usbd_hid_custom.h` (primary)
2. `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h` (temporary - Solution A)

Uncomment **ONE** of these three modes in BOTH files:

### Mode 1: NKRO Keyboard
```c
#define USE_KEYBOARD_MODE      // USB HID Keyboard with 96-key NKRO
//#define USE_JOYSTICK_MODE    // Comment out other modes
//#define USE_JVS_MODE         // Comment out other modes
```

### Mode 2: Dual Joystick (CURRENT)
```c
//#define USE_KEYBOARD_MODE    // Comment out other modes
#define USE_JOYSTICK_MODE      // 2 USB HID Joysticks (14 buttons each)
//#define USE_JVS_MODE         // Comment out other modes
```

### Mode 3: JVS Protocol (RS485)
```c
//#define USE_KEYBOARD_MODE    // Comment out other modes
//#define USE_JOYSTICK_MODE    // Comment out other modes
#define USE_JVS_MODE           // JVS/RS485 arcade I/O board
```

**Note**: You must update the mode in both files until Solution B migration is complete.

## Compilation

### STM32CubeIDE (Recommended)
1. File → Open Projects from File System
2. Select `c:\GitHubProject\hido\firmware` folder
3. Project → Build All (Ctrl+B)
4. Run → Debug (F11) to flash firmware

### Command Line
```powershell
cd C:\GitHubProject\hido\firmware
make clean
make -j4
```

## Flashing

### Via STM32CubeIDE
- Connect ST-Link debugger
- Click "Debug" button (bug icon) or press F11
- Firmware will be flashed automatically

### Via ST-Link Utility
1. Connect ST-Link
2. File → Open File → Select `Debug/hido.elf`
3. Target → Program & Verify (Alt+P)

## Testing

### Mode 1: NKRO Keyboard
1. Compile with `USE_KEYBOARD_MODE` defined
2. Flash firmware
3. Connect USB to PC
4. Device appears as "STM32 Human Interface Device"
5. Test with keyboard tester: https://www.keyboardtester.com
6. All 27 buttons should work simultaneously (NKRO)
7. LED1 blinks once at startup

### Mode 2: Dual Joystick (CURRENT MODE)
1. Compile with `USE_JOYSTICK_MODE` defined (in both files)
2. Flash firmware
3. Connect USB to PC
4. **Device appears as 2 separate joysticks** (TWO Application Collections)
5. Test in Windows: 
   - Control Panel → Devices and Printers → Game Controllers (should show 2 devices)
   - Or press Win+R → `joy.cpl` → Should list 2 controllers
6. Or use joystick testing tool: https://hardwaretester.com/gamepad
7. Each joystick has: 2 axes (X, Y) + 14 buttons
8. MAME/arcade frontends will see Player 1 and Player 2 as separate controllers
9. LED1 blinks once at startup

### Mode 3: JVS Protocol
1. Compile with `USE_JVS_MODE` defined
2. Flash firmware
3. Connect RS485 to arcade board
4. Sense line (PA2) should be floating initially
5. After address assignment, PA2 goes HIGH (3.3V)
6. Test with TeknoParrot or JVS test utility

## Hardware Connections

### Keyboard/Joystick Mode (Buttons)
- **Player 1**: PB3-12 (Joystick + 6 buttons)
- **Player 2**: PC0-1, PC5-9, PC13-15 (Joystick + 6 buttons)
- **System**: PA6-7, PA15, PB2, PB13-15
- **USB**: Connect to PC

### JVS Mode
- **RS485 TX**: PA9 → SN65HVD1786D DI pin
- **RS485 RX**: PA10 → SN65HVD1786D RO pin
- **RS485 Enable**: PA8 → SN65HVD1786D DE/RE pins
- **Sense Line**: PA2 → Arcade board sense input (via JVS connector)
- **RS485 A/B**: Connect to arcade board JVS connector
- **GND**: Common ground with arcade board

## LED Indicators

- **LED1 (PC10)**: 
  - Single blink at startup = Initialization OK
  - Continuous blinking = Normal operation (button activity)
  
- **LED2 (PC11)**: Reserved for future use
- **LED3 (PC12)**: Reserved for future use

## Troubleshooting

### Device Not Recognized (Direct Button Mode)
1. Check USB cable
2. Try different USB port
3. Verify device in Device Manager (Windows)
4. Look for "STM32 Human Interface Device"

### No Response (JVS Mode)
1. Check RS485 wiring (swap A and B if needed)
2. Verify sense line voltage (should be ~3.3V when ready)
3. Confirm baud rate: 115200 8N1
4. Test with loopback (short TX to RX)

### Compilation Errors
1. Make sure STM32CubeIDE has ARM GCC toolchain installed
2. Clean project: Project → Clean
3. Rebuild: Project → Build All
4. Check USB HID library path in project settings

### IntelliSense Errors (Red Squiggles)
- These are VS Code editor warnings, NOT compilation errors
- Project will compile fine in STM32CubeIDE
- To fix: Run "C/C++: Select IntelliSense Configuration" in VS Code

## Button Mapping Reference

### Direct Button Mode (USB HID)

**Player 1:**
- PB3/4/5/6: Arrow Keys (Up/Down/Left/Right)
- PB7/8/9/10/11/12: Space, /, Y, T, R, F

**Player 2:**
- PC0/1/5/6: W, S, A, D
- PC7/8/9/13/14/15: Q, W, E, O, I, U

**System:**
- PA6/7/15: Esc, F1, F2 (Coin/Start)
- PB2/13/14/15: F3, F4, F5, F6 (Service/Test)

### JVS Mode

**Player 1:**
- Start, Up, Down, Left, Right, Buttons 1-8

**Player 2:**
- Start, Up, Down, Left, Right, Buttons 1-8

**System:**
- Test, Service, Coin 1, Coin 2

## Performance

### Direct Button Mode
- USB Polling Rate: 1000Hz (1ms)
- Button Scan Rate: ~2000Hz (500μs)
- Debounce Time: 5ms (configurable)
- Total Input Lag: ~6-7ms worst case

### JVS Mode
- RS485 Baud Rate: 115200 bps
- Packet Response: < 10ms typical
- Button Update Rate: Per arcade board request
- Supported: 2 players, 8 buttons each, 2 coins

## Configuration Files

### Debounce Time
File: `Core/Inc/arcade_keyboard.h`
```c
#define DEBOUNCE_TIME_MS 5  // Change to adjust
```

### JVS Board Name
File: `Core/Inc/jvs_protocol.h`
```c
#define JVS_BOARD_NAME "HIDO Arcade Controller"
```

### JVS Capabilities
File: `Core/Inc/jvs_protocol.h`
```c
#define JVS_NUM_PLAYERS 2
#define JVS_BUTTONS_PER_PLAYER 8
#define JVS_NUM_COINS 2
```

## Documentation

- `README.md`: Full project documentation
- `HARDWARE_CONFIG.md`: Detailed pin mapping
- `JVS_PROTOCOL.md`: JVS protocol specification
- `ISTRUZIONI_UTENTE.md`: Italian user guide

## Support

For issues or questions:
1. Check documentation files
2. Verify hardware connections
3. Test with example programs
4. Check STM32CubeIDE console for build errors

## License

Open source - see LICENSE file in project root
