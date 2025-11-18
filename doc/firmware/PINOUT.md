# HIDO Pinout Mapping

This document describes the complete pin mapping between the arcade connectors (A/B) silkscreen labels and the corresponding keyboard keys.

> 📚 **Back to**: [Main README](README.md) | [Firmware Documentation](doc/firmware/README.md) | [Quick Start](doc/firmware/QUICK_START.md)

## Connector Pin Numbering
- **Pin 1**: Common (COM) - Ground reference for all buttons
- **Pins 2-18**: Button/Direction inputs

## Silkscreen Labeling
The silkscreen labels on the PCB correspond to connector pins as follows:
- Pin 2 → Silkscreen **0**
- Pins 3-11 → Silkscreen **1-9**
- Pins 12-17 → Silkscreen **A-F**
- Pin 18 → Silkscreen **10**

---

## Player 1 (A)

| CN (A) | Signal | MCU Pin | Key | Function |
|---------|--------|---------|-----|----------|
| COM | GND | - | - | Common Ground |
| **0** | TIM1 | PA1 | Z | Button 1 |
| **1** | TIM2 | PA0 | X | Button 2 |
| **2** | ADC1 | PC2 | C | Button 3 |
| **3** | ADC2 | PC3 | V | Button 4 |
| **4** | IN1 | PC1 | B | Button 5 |
| **5** | IN2 | PC0 | N | Button 6 |
| **6** | IN3 | PC15 | M | Button 7 |
| **7** | IN4 | PC14 | Q | Button 8 |
| **8** | IN5 | PC13 | W | Button 9 |
| **9** | IN6 | PB9 | E | Button 10 |
| **A** | IN7 | PB8 | R | Button 11 |
| **B** | IN8 | PB7 | T | Button 12 |
| **C** | IN9 | PB6 | Y | Button 13 |
| **D** | IN10 | PB5 | → | Right Arrow |
| **E** | IN11 | PB4 | ← | Left Arrow |
| **F** | IN12 | PB3 | ↓ | Down Arrow |
| **10** | IN26 | PA15 | ↑ | Up Arrow |

---

## Player 2 (B)

| CN (B) | Signal | MCU Pin | Key | Function |
|---------|--------|---------|-----|----------|
| COM | GND | - | - | Common Ground |
| **0** | IN13 | PA7 | A | Button 1 |
| **1** | IN14 | PC4 | S | Button 2 |
| **2** | IN15 | PC5 | D | Button 3 |
| **3** | ADC3 | PB0 | O | Button 4 |
| **4** | ADC4 | PB1 | P | Button 5 |
| **5** | IN16 | PB2 | F | Button 6 |
| **6** | IN17 | PB10 | G | Button 7 |
| **7** | IN18 | PB11 | H | Button 8 |
| **8** | IN19 | PB12 | J | Button 9 |
| **9** | IN20 | PB13 | K | Button 10 |
| **A** | IN21 | PB14 | L | Button 11 |
| **B** | IN22 | PB15 | U | Button 12 |
| **C** | TIM3 | PC6 | I | Button 13 |
| **D** | TIM4 | PC7 | F4 | Right |
| **E** | IN23 | PC8 | F3 | Left |
| **F** | IN24 | PC9 | F2 | Down |
| **10** | IN25 | PA6 | F1 | Up |

---

## Usage Notes

### For End Users
1. Locate the silkscreen label on the PCB near the connector
2. Connect your arcade button to the corresponding pin
3. The button will generate the keyboard key shown in the "Key" column
4. COM Pin must be connected to the common ground of all buttons

### For Developers
- All inputs are configured with internal pull-up resistors
- Buttons are active-low (press = pull to GND)
- Debouncing: 5ms in firmware
- LED indicators: LED1 = P1 active, LED2 = P2 active, LED3 = Not implemented yet

## Technical Details

### STM32F102RB GPIO Allocation
- **GPIOA**: PA0, PA1, PA6, PA7, PA15 (5 inputs)
- **GPIOB**: PB0-PB15 (16 inputs)
- **GPIOC**: PC0-PC9, PC13-PC15 (13 inputs)
- **Total**: 34 digital inputs (17 per player)

### USB HID Descriptors

HID descriptor and report-format details (keyboard and joystick descriptors, report IDs, and exact byte layouts) have been consolidated into `doc/firmware/README.md`. See that file for the authoritative HID descriptor information and examples for building each firmware mode.

### JVS Mode (RS485)
-- JVS/RS485 uses a separate firmware stack; physical pins use UART/RS485 transceiver lines (see `doc/firmware/HARDWARE_CONFIG.md`).
- Not a USB HID device; used for JAMMA/JVS cabinets and IO boards.

### Firmware Configuration
- **Keyboard Mode**: `USE_KEYBOARD_MODE` (default)
- **Joystick Mode**: `USE_JOYSTICK_MODE` (alternative)
- **JVS Mode**: `USE_JVS_MODE` (arcade protocol)

Select mode in `firmware/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h` or ...
Select mode via the build scripts (`compile_direct.ps1 -Mode` or `build.ps1 -Mode`) — see `doc/firmware/README.md` for details.
---

*Last updated: November 13, 2025*
