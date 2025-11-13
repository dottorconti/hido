# HIDO - Arcade Multi-Mode Controller

Open source arcade controller with STM32F102RB microcontroller.

## 🎮 Features

- **Triple Mode Support**: NKRO Keyboard, Dual Joystick, or JVS/RS485
- **Ultra-Low Latency**: 1ms USB polling (1000Hz)
- **Plug & Play**: Standard USB HID device, no drivers needed
- **Hardware Debouncing**: 5ms configurable debounce time
- **Dual Player Support**: 2 independent joystick interfaces (34 inputs total)

## 📚 Documentation

### Quick Links
- **[📋 PINOUT.md](PINOUT.md)** - Complete pin mapping and silkscreen reference
- **[🔧 Firmware Documentation](firmware/README.md)** - Detailed firmware guide
- **[⚡ Quick Start Guide](firmware/QUICK_START.md)** - Get started quickly
- **[🔄 Mode Comparison](firmware/MODE_COMPARISON.md)** - Keyboard vs Joystick vs JVS
- **[⚙️ Hardware Configuration](firmware/HARDWARE_CONFIG.md)** - Hardware setup
- **[🕹️ JVS Protocol](firmware/JVS_PROTOCOL.md)** - JVS/RS485 arcade protocol
- **[📝 Changelog](firmware/CHANGELOG.md)** - Version history
- **[👥 Credits](firmware/CREDITS.md)** - Contributors and licenses

## 🚀 Quick Start

1. **Connect your arcade controls** - See [PINOUT.md](PINOUT.md) for connector pinout
2. **Select operating mode** - Edit `firmware/Core/Inc/usbd_hid_custom.h`
3. **Compile and flash** - See [Quick Start Guide](firmware/QUICK_START.md)
4. **Plug and play** - Device recognized as USB HID

## 🛠️ Hardware

- **MCU**: STM32F102RB (Cortex-M3, 128KB Flash)
- **USB**: Full-Speed USB 2.0 (12 Mbps)
- **Connectors**: J6 (Player 1), J7 (Player 2) - 18 pins each
- **Inputs**: 34 digital inputs (17 per player) with pull-up resistors
- **LEDs**: Debug indicators (LED1=P1, LED2=P2)

## 📋 Pin Mapping Reference

Each connector has **silkscreen labels** (0-9, A-F, 10) for easy identification:

| Silkscreen | Pin | Player 1 Key | Player 2 Key |
|------------|-----|--------------|--------------|
| COM | 1 | GND | GND |
| 0 | 2 | Z | A |
| 1 | 3 | X | S |
| 2 | 4 | C | D |
| ... | ... | ... | ... |
| 10 | 18 | ↑ | F1 |

**See [PINOUT.md](PINOUT.md) for complete mapping.**

## 🔌 Operating Modes

### Keyboard Mode (6KRO)
Standard USB keyboard with 6-key rollover for arcade gaming.

### Joystick Mode
Dual USB joystick with 14 buttons + 2 axes each.

### JVS Mode
RS485 arcade I/O board protocol for JAMMA/JVS cabinets.

**See [Mode Comparison](firmware/MODE_COMPARISON.md) for details.**

## 📦 Repository Structure

```
hido/
├── README.md              # This file
├── PINOUT.md             # Complete pinout reference
├── firmware/             # STM32 firmware source
│   ├── README.md         # Firmware documentation
│   ├── QUICK_START.md    # Quick start guide
│   ├── Core/             # Application code
│   ├── Drivers/          # STM32 HAL drivers
│   └── Middlewares/      # USB device library
├── hardware/             # KiCad PCB design files
└── fabrication/          # Gerber files for manufacturing
```

## 📄 License

See [CREDITS.md](firmware/CREDITS.md) for license information.


