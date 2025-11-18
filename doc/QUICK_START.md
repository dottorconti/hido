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

## Support

For issues or questions:
1. Check documentation files
2. Verify hardware connections
3. Test with example programs
4. Check STM32CubeIDE console for build errors

## License

Open source - see LICENSE file in project root
