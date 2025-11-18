# Mode Comparison - HIDO Arcade Controller

## Overview

HIDO supports three distinct operating modes. Choose the one that best fits your setup.

## Mode Comparison Table

| Feature | NKRO Keyboard | Dual Joystick | JVS Protocol |
|---------|---------------|---------------|--------------|
| **Connection** | USB | USB | RS485 |
| **Device Type** | HID Keyboard | HID Joystick | JVS I/O Board |
| **Players** | 2 | 2 | 2 |
| **Buttons per Player** | Unlimited (mapped to keys) | 14 | 8 |
| **Simultaneous Inputs** | 96 keys (NKRO) | All buttons | All buttons |
| **Ghosting** | None | None | None |
| **Polling Rate** | 1000Hz (1ms) | 1000Hz (1ms) | Variable |
| **Latency** | ~1ms | ~1ms | <10ms |
| **Windows Support** | ✅ Native | ✅ Native | ❌ (needs emulator) |
| **Linux Support** | ✅ Native | ✅ Native | ✅ Native (OpenJVS) |
| **macOS Support** | ✅ Native | ✅ Native | ❌ |
| **Driver Required** | ❌ No | ❌ No | ⚠️ Depends on game |
| **Best For** | MAME, Emulators | Modern PC games | Real arcade boards |

## When to Use Each Mode

### 🎹 NKRO Keyboard Mode (Default)

**Use When:**
- Playing MAME or arcade emulators
- Need maximum compatibility with PC games
- Want simple plug-and-play setup
- Playing rhythm games (DDR, beatmania)
- Using keyboard-based game engines

**Advantages:**
- Universal compatibility
- Works on any OS without drivers
- Simple configuration in games
- 96 simultaneous key presses (true NKRO)
- Very low latency

**Limitations:**
- Some modern games prefer joystick input
- Key mapping required in games

---

### 🕹️ Dual Joystick Mode (CURRENT)

**Use When:**
- Playing MAME with joystick support
- Arcade frontends that require separate joystick devices
- Modern PC games that support joysticks
- Steam games with gamepad support
- Fighting games (Street Fighter, Tekken)
- Shmups and arcade ports
- Games that auto-detect joysticks

**Advantages:**
- Native joystick support in many games
- **Two independent controllers** (appears as 2 separate devices in Windows)
- TWO Application Collections = MAME sees 2 joysticks
- No key mapping needed in joystick-aware games
- Works with Steam Big Picture
- DirectInput/XInput compatible games
- 14 buttons + 2 axes per joystick

**Limitations:**
- Some older games don't support joysticks
- Less universal than keyboard mode
- Emulators may prefer keyboard input

---

### 📡 JVS Protocol Mode

**Use When:**
- Connecting to real arcade hardware (NAOMI, Triforce, Chihiro, etc.)
- Using with TeknoParrot
- Building authentic arcade cabinet with real PCB
- Need arcade-standard I/O communication
- Professional arcade cabinet setup

**Advantages:**
- Industry-standard arcade protocol
- Compatible with real arcade boards
- Supports coin counters
- Authentic arcade experience
- Works with professional arcade systems

**Limitations:**
- Requires RS485 hardware
- Not plug-and-play on PC
- Needs TeknoParrot or similar emulator
- More complex wiring

---

## Hardware Requirements

### All Modes
- STM32F102RBT6 MCU
- 27 GPIO inputs (buttons)
- USB connection to PC
- Internal pull-ups enabled

### JVS Mode Additional Requirements
- SN65HVD1786D RS485 transceiver
- Proper RS485 wiring (D, /D, GND)
- Sense line connection
- Arcade board with JVS support

## Software Configuration

All three modes use the **same hardware** and GPIO pin mapping. Only the firmware compile-time configuration changes.

### Switching Modes

1. Open `Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`
2. Uncomment desired mode:
   ```c
   #define USE_KEYBOARD_MODE   // Option 1
   // #define USE_JOYSTICK_MODE   // Option 2
   // #define USE_JVS_MODE        // Option 3
   ```
3. Recompile and flash firmware
4. Reconnect USB or power cycle

**Note:** Only ONE mode can be active at a time.

## Game Compatibility Examples

### NKRO Keyboard Mode Works Best With:
- MAME
- FinalBurn Neo
- Dolphin Emulator
- PCSX2
- RetroArch
- StepMania / osu!
- Most emulators

### Dual Joystick Mode Works Best With:
- Steam games with controller support
- Fighting games (Street Fighter, Guilty Gear)
- Shmups (Touhou, DoDonPachi)
- Arcade collections (Capcom Arcade Stadium)
- Modern PC arcade ports

### JVS Mode Required For:
- Real NAOMI boards
- Triforce arcade systems
- Chihiro arcade hardware
- TeknoParrot games
- Lindbergh systems
- Professional arcade setups

## Performance Comparison

### Input Latency (measured)

| Mode | Typical Latency | Max Latency |
|------|----------------|-------------|
| NKRO Keyboard | ~1ms | ~2ms |
| Dual Joystick | ~1ms | ~2ms |
| JVS Protocol | ~5ms | ~10ms |

*Lower is better. Measured from button press to USB/RS485 transmission.*

### CPU Usage

All modes use minimal CPU resources (<5% at 72MHz).

## Recommendations

### For Home Arcade Cabinet:
**Use NKRO Keyboard** for maximum compatibility with emulators and PC games.

### For Modern Gaming PC:
**Use Dual Joystick** if your games support controllers. Fall back to Keyboard for emulators.

### For Professional/Commercial Setup:
**Use JVS Protocol** for authentic arcade hardware communication.

### For Competitive Gaming:
**Use NKRO Keyboard** for lowest latency and universal compatibility.

---

## Can I Switch Between Modes?

**Yes!** Switching modes only requires:
1. Editing one #define in `usbd_hid.h`
2. Recompiling firmware
3. Flashing to device

The hardware doesn't change - all buttons work the same way in all modes.

## Questions?

- **Q: Can I use both Keyboard and Joystick modes simultaneously?**
  - A: No, only one mode at a time. You must reflash to switch.

- **Q: Which mode has lowest latency?**
  - A: Keyboard and Joystick are tied (~1ms). JVS is slightly higher (~5-10ms).

- **Q: Do I need to rewire anything to change modes?**
  - A: No! Same hardware, same wiring. Only firmware changes.

- **Q: Can I add a 4th mode?**
  - A: Yes! The code is modular. See CREDITS.md for contribution guidelines.

---

*Last updated: November 2025*
