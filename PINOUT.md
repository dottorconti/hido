# HIDO Pinout Mapping

This document describes the complete pin mapping between the arcade connectors (J6/J7) silkscreen labels and the corresponding keyboard keys.

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

## Player 1 (J6 Connector)

| Pin J6 | Silkscreen | Signal | MCU Pin | Key | Function |
|--------|------------|--------|---------|-----|----------|
| 1 | COM | GND | - | - | Common Ground |
| 2 | **0** | TIM1 | PA1 | Z | Button 1 |
| 3 | **1** | TIM2 | PA0 | X | Button 2 |
| 4 | **2** | ADC1 | PC2 | C | Button 3 |
| 5 | **3** | ADC2 | PC3 | V | Button 4 |
| 6 | **4** | IN1 | PC1 | B | Button 5 |
| 7 | **5** | IN2 | PC0 | N | Button 6 |
| 8 | **6** | IN3 | PC15 | M | Button 7 |
| 9 | **7** | IN4 | PC14 | Q | Button 8 |
| 10 | **8** | IN5 | PC13 | W | Button 9 |
| 11 | **9** | IN6 | PB9 | E | Button 10 |
| 12 | **A** | IN7 | PB8 | R | Button 11 |
| 13 | **B** | IN8 | PB7 | T | Button 12 |
| 14 | **C** | IN9 | PB6 | Y | Button 13 |
| 15 | **D** | IN10 | PB5 | → | Right Arrow |
| 16 | **E** | IN11 | PB4 | ← | Left Arrow |
| 17 | **F** | IN12 | PB3 | ↓ | Down Arrow |
| 18 | **10** | IN26 | PA15 | ↑ | Up Arrow |

---

## Player 2 (J7 Connector)

| Pin J7 | Silkscreen | Signal | MCU Pin | Key | Function |
|--------|------------|--------|---------|-----|----------|
| 1 | COM | GND | - | - | Common Ground |
| 2 | **0** | IN13 | PA7 | A | Button 1 |
| 3 | **1** | IN14 | PC4 | S | Button 2 |
| 4 | **2** | IN15 | PC5 | D | Button 3 |
| 5 | **3** | ADC3 | PB0 | O | Button 12 |
| 6 | **4** | ADC4 | PB1 | P | Button 13 |
| 7 | **5** | IN16 | PB2 | F | Button 4 |
| 8 | **6** | IN17 | PB10 | G | Button 5 |
| 9 | **7** | IN18 | PB11 | H | Button 6 |
| 10 | **8** | IN19 | PB12 | J | Button 7 |
| 11 | **9** | IN20 | PB13 | K | Button 8 |
| 12 | **A** | IN21 | PB14 | L | Button 9 |
| 13 | **B** | IN22 | PB15 | U | Button 10 |
| 14 | **C** | TIM3 | PC6 | I | Button 11 |
| 15 | **D** | TIM4 | PC7 | F4 | Right |
| 16 | **E** | IN23 | PC8 | F3 | Left |
| 17 | **F** | IN24 | PC9 | F2 | Down |
| 18 | **10** | IN25 | PA6 | F1 | Up |

---

## Usage Notes

### For End Users
1. Locate the silkscreen label on the PCB near the connector
2. Connect your arcade button to the corresponding pin
3. The button will generate the keyboard key shown in the "Key" column
4. Pin 1 (COM) must be connected to the common ground of all buttons

### For Developers
- All inputs are configured with internal pull-up resistors
- Buttons are active-low (press = pull to GND)
- Debouncing: 5ms in firmware
- USB HID: 6KRO keyboard mode (Report ID 1)
- LED indicators: LED1 = P1 active, LED2 = P2 active

### Connector Wiring
Both J6 and J7 use the same pinout:
```
Pin 1:  COM (GND)
Pin 2:  Silkscreen 0
Pin 3:  Silkscreen 1
...
Pin 11: Silkscreen 9
Pin 12: Silkscreen A
...
Pin 17: Silkscreen F
Pin 18: Silkscreen 10
```

---

## Technical Details

### STM32F102RB GPIO Allocation
- **GPIOA**: PA0, PA1, PA6, PA7, PA15 (5 inputs)
- **GPIOB**: PB0-PB15 (16 inputs)
- **GPIOC**: PC0-PC9, PC13-PC15 (13 inputs)
- **Total**: 34 digital inputs (17 per player)

### USB HID Keyboard Descriptor
- Report ID: 1
- Format: 6-Key Rollover (6KRO)
- Report size: 9 bytes (1 ID + 1 modifier + 1 reserved + 6 keys)
- Polling rate: 1ms

### Firmware Configuration
- **Keyboard Mode**: `USE_KEYBOARD_MODE` (default)
- **Joystick Mode**: `USE_JOYSTICK_MODE` (alternative)
- **JVS Mode**: `USE_JVS_MODE` (arcade protocol)

Select mode in `firmware/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h`

---

*Last updated: November 13, 2025*
