# HIDO - Hardware Configuration Guide

## ⚙️ Current Mode: Dual Joystick (14 buttons + 2 axes each)

**Active Configuration**: `USE_JOYSTICK_MODE` in `Core/Inc/usbd_hid_custom.h`

For **Keyboard Mode** mapping, see below. For **Joystick Mode**, buttons are mapped to joystick IDs 1-8 (Player 1: 1-4, Player 2: 5-8) in `Core/Src/arcade_joystick.c`.

---

## 📌 GPIO Pin Mapping

### Input Pins (Arcade Buttons)
All input pins are configured as **INPUT with PULL-UP** (active LOW).
Connect buttons between GPIO pin and GND (normally open switches).

#### Port A (GPIOA)
| Pin   | Function      | Default Mapping |
|-------|---------------|-----------------|
| PA6   | Coin 1        | ESC (0x29)      |
| PA7   | Start 1       | F1 (0x3A)       |
| PA15  | Start 2       | F2 (0x3B)       |

#### Port B (GPIOB) - Player 1 Primary
| Pin    | Function      | Default Mapping        |
|--------|---------------|------------------------|
| PB2    | Service       | F3 (0x3C)              |
| PB3    | P1 Up         | Up Arrow (0x52)        |
| PB4    | P1 Down       | Down Arrow (0x51)      |
| PB5    | P1 Left       | Left Arrow (0x50)      |
| PB6    | P1 Right      | Right Arrow (0x4F)     |
| PB7    | P1 Button 1   | Space (0x2C)           |
| PB8    | P1 Button 2   | / (0x38)               |
| PB9    | P1 Button 3   | Y (0x1C)               |
| PB10   | P1 Button 4   | T (0x17)               |
| PB11   | P1 Button 5   | R (0x15)               |
| PB12   | P1 Button 6   | F (0x09)               |
| PB13   | Test          | F4 (0x3D)              |
| PB14   | Extra 1       | F5 (0x3E)              |
| PB15   | Extra 2       | F6 (0x3F)              |

#### Port C (GPIOC) - Player 2 Primary
| Pin    | Function      | Default Mapping  |
|--------|---------------|------------------|
| PC0    | P2 Up         | W (0x1D)         |
| PC1    | P2 Down       | S (0x16)         |
| PC5    | P2 Left       | A (0x04)         |
| PC6    | P2 Right      | D (0x07)         |
| PC7    | P2 Button 1   | Q (0x14)         |
| PC8    | P2 Button 2   | E (0x08)         |
| PC9    | P2 Button 3   | [Reserved]       |
| PC10   | LED 1 Output  | Status indicator |
| PC11   | LED 2 Output  | -                |
| PC12   | LED 3 Output  | -                |
| PC13   | P2 Button 4   | O (0x12)         |
| PC14   | P2 Button 5   | I (0x0C)         |
| PC15   | P2 Button 6   | U (0x18)         |

### Output Pins (LEDs)
| Pin   | Function | Usage                    |
|-------|----------|--------------------------|
| PC10  | LED1     | Activity/Heartbeat       |
| PC11  | LED2     | Reserved                 |
| PC12  | LED3     | Reserved                 |
| PC4   | Output   | General purpose          |

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
