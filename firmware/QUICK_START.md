# Quick Start Guide - HIDO Arcade Controller

## Mode Selection

### Direct Button Mode (USB HID Keyboard) - Default
File: `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`

```c
#define USE_DIRECT_BUTTONS    // Keep this line
```

### JVS Mode (RS485 Arcade I/O)
File: `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`

```c
// #define USE_DIRECT_BUTTONS    // Comment out or delete this line
```

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

### Direct Button Mode
1. Compile with `USE_DIRECT_BUTTONS` defined
2. Flash firmware
3. Connect USB to PC
4. Device should appear as "STM32 Human Interface Device"
5. Test buttons with keyboard tester website
6. LED1 should blink once at startup

### JVS Mode
1. Compile **WITHOUT** `USE_DIRECT_BUTTONS`
2. Flash firmware
3. Connect RS485 to arcade board
4. Sense line (PA10) should be floating initially
5. After address assignment, PA10 goes to 2.5V
6. Test with TeknoParrot or JVS test utility

## Hardware Connections

### Direct Button Mode
- **Player 1**: PB3-12 (Joystick + 6 buttons)
- **Player 2**: PC0-1, PC5-9, PC13-15 (Joystick + 6 buttons)
- **System**: PA6-7, PA15, PB2, PB13-15
- **USB**: Connect to PC

### JVS Mode
- **RS485 TX**: PA8 → SN65HVD1786D D pin
- **RS485 RX**: PA9 → SN65HVD1786D /D pin
- **Sense Line**: PA10 → Arcade board sense input
- **RS485 D/D̄**: Connect to arcade board JVS connector
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
1. Check RS485 wiring (swap D and /D if needed)
2. Verify sense line voltage (should be 2.5V when ready)
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
