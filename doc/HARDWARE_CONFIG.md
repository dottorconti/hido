# HIDO - Hardware Configuration Guide

## ⚙️ Current Mode: Dual Joystick (14 buttons + 2 axes each)

**Active Configuration**: `USE_JOYSTICK_MODE` in `Core/Inc/usbd_hid_custom.h`

For **Keyboard Mode** mapping, see below. For **Joystick Mode**, buttons are mapped to joystick IDs 1-8 (Player 1: 1-4, Player 2: 5-8) in `Core/Src/arcade_joystick.c`.

---

## 📌 GPIO Pin Mapping (Authoritative)

The tables below are the authoritative firmware pin mappings. They are derived from the firmware source (`firmware/Core/Inc/main.h`) and represent the GPIO macros the code uses at compile time.

Source of truth:
- Firmware macros: `firmware/Core/Inc/main.h`
- Connector/schematic mapping: `doc/firmware/PINOUT.md`

All inputs are configured as INPUT with PULL-UP (active LOW). Connect buttons between GPIO pin and GND (normally open switches).

### LEDs (firmware macros)
| Macro | GPIO Pin |
|-------|----------|
| `LED1` | PC10 |
| `LED2` | PC11 |
| `LED3` | PC12 |

### Player 1 (J6) - directionals and buttons (firmware macros)
| Firmware Macro | GPIO Pin | Notes |
|----------------|----------|-------|
| `P1_UP` | PA15 | Joystick Up (J6 Pin 18 / IN26)
| `P1_DOWN` | PB3  | Joystick Down (J6 Pin 17 / IN12)
| `P1_LEFT` | PB4  | Joystick Left (J6 Pin 16 / IN11)
| `P1_RIGHT` | PB5 | Joystick Right (J6 Pin 15 / IN10)
| `P1_BTN1` | PA1  | J6 Pin 2 (TIM1)
| `P1_BTN2` | PA0  | J6 Pin 3 (TIM2)
| `P1_BTN3` | PC2  | J6 Pin 4 (ADC1)
| `P1_BTN4` | PC3  | J6 Pin 5 (ADC2)
| `P1_BTN5` | PC1  | J6 Pin 6 (IN1)
| `P1_BTN6` | PC0  | J6 Pin 7 (IN2)
| `P1_BTN7` | PC15 | J6 Pin 8 (IN3)
| `P1_BTN8` | PC14 | J6 Pin 9 (IN4)
| `P1_BTN9` | PC13 | J6 Pin 10 (IN5)
| `P1_BTN10` | PB9 | J6 Pin 11 (IN6)
| `P1_BTN11` | PB8 | J6 Pin 12 (IN7)
| `P1_BTN12` | PB7 | J6 Pin 13 (IN8)
| `P1_BTN13` | PB6 | J6 Pin 14 (IN9)

### Player 2 (J7) - directionals and buttons (firmware macros)
| Firmware Macro | GPIO Pin | Notes |
|----------------|----------|-------|
| `P2_UP` | PA6  | J7 Pin 18 (IN25)
| `P2_DOWN` | PC9 | J7 Pin 17 (IN24)
| `P2_LEFT` | PC8 | J7 Pin 16 (IN23)
| `P2_RIGHT` | PC7 | J7 Pin 15 (TIM4)
| `P2_BTN1` | PA7 | J7 Pin 2 (IN13)
| `P2_BTN2` | PC4 | J7 Pin 3 (IN14)
| `P2_BTN3` | PC5 | J7 Pin 4 (IN15)
| `P2_BTN4` | PB2 | J7 Pin 7 (IN16)
| `P2_BTN5` | PB10 | J7 Pin 8 (IN17)
| `P2_BTN6` | PB11 | J7 Pin 9 (IN18)
| `P2_BTN7` | PB12 | J7 Pin 10 (IN19)
| `P2_BTN8` | PB13 | J7 Pin 11 (IN20)
| `P2_BTN9` | PB14 | J7 Pin 12 (IN21)
| `P2_BTN10` | PB15 | J7 Pin 13 (IN22)
| `P2_BTN11` | PC6 | J7 Pin 14 (TIM3)
| `P2_BTN12` | PB0 | J7 Pin 5 (ADC3, used as button)
| `P2_BTN13` | PB1 | J7 Pin 6 (ADC4, used as button)

### Notes
- If you maintain a separate schematic-level mapping (connector silkscreen → MCU pin), keep that table in `doc/firmware/PINOUT.md` and use this file for firmware-visible macros.
- If you change pin macros in `firmware/Core/Inc/main.h`, update this document to match and vice versa — `main.h` is the single source of truth for compiled firmware.


## 🔌 Connection Examples

### Standard Arcade Button (Normally Open)
```
Button Switch (NO)
    |
    |----[GPIO Pin (with internal pull-up)]
    |
   GND

When button pressed: Pin goes LOW (0V)
When button released: Pin stays HIGH (3.3V via pull-up)
```

### Multiple Buttons Sharing GND
```
    [Button1]----PB3
    [Button2]----PB4     All connected to common GND
    [Button3]----PB5     (Star ground recommended)
       |
      GND
```

### LED Connection
```
GPIO Pin (PC10) ----[Current Limit Resistor 220Ω]----[LED]----GND

LED turns ON when GPIO is HIGH
```

## 🛠️ Customizing Button Mapping

### For Joystick Mode (CURRENT)
Edit `Core/Src/arcade_joystick.c`, locate `joystick_map[]` array:

```c
static const JoystickMapping_t joystick_map[MAX_JOYSTICK_BUTTONS] = {
    // {Port,      Pin,           Joystick_ID, Active_Low}
    {GPIOB, GPIO_PIN_3,  1, true},  // PB3 -> Joystick 1 (P1 Up)
    {GPIOB, GPIO_PIN_4,  2, true},  // PB4 -> Joystick 2 (P1 Down)
    // Joystick ID 1-4 = Player 1, ID 5-8 = Player 2
    
    // Buttons are automatically assigned sequentially
};
```

### For Keyboard Mode
Edit `Core/Src/arcade_keyboard.c`, locate `button_map[]` array:

```c
static const ButtonMapping_t button_map[MAX_BUTTONS] = {
    // {Port,      Pin,           HID_Code, Active_Low}
    {GPIOB, GPIO_PIN_3,  0x52, true},  // Example: PB3 -> Up Arrow
    
    // Add your custom button here:
    {GPIOA, GPIO_PIN_0,  0x04, true},  // PA0 -> 'A' key
};
```

### Common USB HID Keyboard Codes
```c
// Letters
0x04-0x1D = A-Z

// Numbers
0x1E = 1,  0x1F = 2,  ...,  0x27 = 0

// Special Keys
0x28 = Enter       0x2C = Space      0x29 = ESC
0x2A = Backspace   0x2B = Tab        0x39 = Caps Lock

// Function Keys
0x3A = F1,  0x3B = F2,  ...,  0x45 = F12

// Arrow Keys
0x50 = Left,  0x4F = Right,  0x51 = Down,  0x52 = Up

// Keypad
0x59 = Keypad 1,  0x5A = Keypad 2,  ...,  0x62 = Keypad 0
```

Full reference: [USB HID Usage Tables v1.21](https://usb.org/sites/default/files/hut1_21.pdf) (Page 83)

## ⚡ Performance Optimization Tips

### 1. Minimize Wire Length
- Keep button wires < 50cm for best response
- Use twisted pair for long runs
- Star ground topology preferred

### 2. Add External Capacitors (Optional)
For electrical noise environments:
```
GPIO Pin ----[10nF ceramic capacitor]---- GND
             (placed close to MCU)
```

### 3. Adjust Debounce Time
In `Core/Inc/arcade_joystick.h` (Joystick Mode) or `Core/Inc/arcade_keyboard.h` (Keyboard Mode):
```c
#define DEBOUNCE_TIME_MS    5   // Reduce to 3ms for faster response
                                 // Increase to 10ms for noisy buttons
```

### 4. Power Supply
- Use quality 5V USB power (>500mA capable)
- Add 100µF capacitor near STM32 VDD pins
- Avoid USB hubs with poor power regulation

## 🔍 Testing & Troubleshooting

### Test Button Response
1. Open any text editor
2. Press arcade buttons
3. Verify correct keys appear

### Check USB Polling Rate (Windows)
1. Open Device Manager
2. Find "HID-compliant device" under "Human Interface Devices"
3. Properties → Details → Device Descriptor
4. Verify `bInterval = 1` (1ms = 1000Hz)

### LED Indicators
- **LED1 blinks once on startup**: System OK
- **LED1 steady during DEBUG**: Activity indicator
- **No LEDs**: Check power/clock configuration

### Button Not Working
1. Verify button connection (continuity test)
2. Check GPIO mapping in code
3. Ensure button connects pin to GND when pressed
4. Test with multimeter: Pin should be 3.3V when released, 0V when pressed

### Multiple Keys Triggering
- Check for short circuits between GPIO pins
- Verify common GND connections
- Inspect solder joints

### Latency Issues
Run this test:
```
1. Open https://inputlag.science or similar tool
2. Press button and measure response time
3. Should be < 2ms total latency
```

## 📐 Schematic Recommendations

### Minimal Circuit
```
STM32F102RBT6
    VDD(1,2) -----[100nF]-----GND
    VDDA ---------[100nF]-----GNDA
    VUSB ---------[1µF]-------GND
    
    PA11 (USB D-)  -------- USB Data-
    PA12 (USB D+)  -------- USB Data+
    
    PB3-PB15 ---[Internal Pull-up]---[Buttons to GND]
    PC0-PC15 ---[Internal Pull-up]---[Buttons to GND]
    
    PC10 ---[220Ω]---[LED]---GND
```

### Protection Circuit (Recommended)
```
Each Button Input:
GPIO ---[10kΩ]---[Schottky Diode to VDD]---[Button]---GND
```

## 🚀 Advanced: Matrix Scanning (Future)

For >32 buttons, implement matrix scanning:
```c
// Not yet implemented - requires code modification
// Allows 64+ buttons with fewer GPIO pins
```

---
**Last Updated**: November 2025
