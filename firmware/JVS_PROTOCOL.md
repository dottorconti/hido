# JVS Protocol Configuration for HIDO

## Overview
This document describes the JVS (JAMMA Video Standard) implementation for the HIDO Arcade Controller.

## Hardware Setup

### RS485 Transceiver: SN65HVD1786D
- **TX (PA8)**: USART1 Transmit → RS485 D pin
- **RX (PA9)**: USART1 Receive → RS485 /D pin
- **Sense Line (PA10)**: GPIO output for connection detection
  - Floating: Device waiting for connection
  - 2.5V: Device ready and addressed

### Wiring to Arcade Board
1. Connect RS485 D to arcade board JVS Data+
2. Connect RS485 /D to arcade board JVS Data-
3. Connect GND to arcade board ground
4. Sense line connects to arcade board sense input

## Protocol Specifications

### Communication Parameters
- **Baud Rate**: 115200 bps
- **Data Format**: 8 data bits, No parity, 1 stop bit (8N1)
- **Half-Duplex**: RS485 with direction control

### Packet Structure
```
[SYNC] [DEST] [LENGTH] [DATA...] [CHECKSUM]

SYNC     = 0xE0 (marks packet start)
DEST     = Destination address (0x00 = broadcast, 0xFF = master)
LENGTH   = Data bytes + 1 (includes checksum)
DATA     = Command/response bytes
CHECKSUM = Sum of all bytes from DEST to last DATA byte
```

### Escape Sequences
Special bytes in data are escaped:
- **0xE0 (SYNC)** → `0xD0 0xDF`
- **0xD0 (ESCAPE)** → `0xD0 0xCF`

### Supported Commands

#### 0xF0 - Reset Device
- **Request**: `[0xF0] [0xD9]`
- **Response**: None
- **Action**: Device resets to unaddressed state, sense line goes floating

#### 0xF1 - Assign Address
- **Request**: `[0xF1] [ADDRESS]`
- **Response**: `[0x01] [0x01]` (Success)
- **Action**: Device takes the assigned address, sense line goes active (2.5V)

#### 0x10 - Request ID String
- **Request**: `[0x10]`
- **Response**: `[0x01] [0x01] ["HIDO Arcade Controller\0"]`
- **Returns**: Board identification string (null-terminated)

#### 0x11 - Get Command Version
- **Request**: `[0x11]`
- **Response**: `[0x01] [0x01] [0x13]`
- **Returns**: Command revision (1.3)

#### 0x12 - Get JVS Version
- **Request**: `[0x12]`
- **Response**: `[0x01] [0x01] [0x30]`
- **Returns**: JVS specification version (3.0)

#### 0x13 - Get Communication Version
- **Request**: `[0x13]`
- **Response**: `[0x01] [0x01] [0x10]`
- **Returns**: Communication protocol version (1.0)

#### 0x14 - Get Capabilities
- **Request**: `[0x14]`
- **Response**: `[0x01] [0x01] [CAPABILITY_LIST] [0x00]`

**Capability List:**
```
[0x01] [0x02] [0x08] [0x00]   // 2 players, 8 buttons each
[0x02] [0x02] [0x00] [0x00]   // 2 coin slots
[0x00]                         // End of list
```

Capability codes:
- `0x01`: Digital inputs (players/buttons)
- `0x02`: Coin slots
- `0x03`: Analog inputs
- `0x04`: Rotary encoders
- `0x12`: Screen position output
- `0x00`: End marker

#### 0x20 - Read Digital Switches
- **Request**: `[0x20] [NUM_PLAYERS] [BYTES_PER_PLAYER]`
- **Response**: `[0x01] [0x01] [SYSTEM] [P1_DATA...] [P2_DATA...]`

**System Byte (8 bits):**
- Bit 7: Test button
- Bit 6: Tilt1
- Bit 5: Tilt2
- Bit 4: Tilt3
- Bits 3-0: Reserved

**Player Data (2 bytes per player):**
- Byte 1 bits:
  - Bit 7: Start
  - Bit 6: Service
  - Bit 5: Up
  - Bit 4: Down
  - Bit 3: Left
  - Bit 2: Right
  - Bit 1: Button 1
  - Bit 0: Button 2
- Byte 2 bits:
  - Bit 7: Button 3
  - Bit 6: Button 4
  - Bit 5: Button 5
  - Bit 4: Button 6
  - Bit 3: Button 7
  - Bit 2: Button 8
  - Bits 1-0: Reserved

#### 0x21 - Read Coin Counters
- **Request**: `[0x21] [NUM_SLOTS]`
- **Response**: `[0x01] [0x01] [COIN1_H] [COIN1_L] [COIN2_H] [COIN2_L]`

Each coin counter is 14-bit value (0-16383):
- High byte: bits 13-8 (6 bits)
- Low byte: bits 7-0 (8 bits)

#### 0x2F - Retransmit Previous Response
- **Request**: `[0x2F]`
- **Response**: Previous response packet is sent again
- **Use**: Error recovery when packet is corrupted

### Status Codes

**Report Status (first byte of response):**
- `0x01`: Normal success
- `0x02`: Unsupported command
- `0x03`: Checksum error
- `0x04`: Overflow (too much data)

**Command Status (second byte for specific commands):**
- `0x01`: Success
- `0x02`: Parameter error
- `0x03`: Data error

## Implementation Details

### Sense Line Control
```c
// Floating (waiting for connection)
JVS_SetSenseLine(false);

// Active 2.5V (ready and addressed)
JVS_SetSenseLine(true);
```

### Initialization Sequence
1. Device powers on
2. Sense line set to floating
3. Wait for Reset command (0xF0)
4. Wait for Address Assignment (0xF1)
5. Set sense line active (2.5V)
6. Begin normal operation

### Button Mapping Example
```c
// Update JVS state from GPIO
JVS_UpdateInputs();

// Or set individual switches
JVS_SetSwitch(1, 0, button_pressed);  // Player 1, Button 1

// Increment coin counter
JVS_IncrementCoin(0);  // Coin slot 1
```

### Main Loop Integration
```c
while (1) {
    JVS_ProcessPackets();  // Handle incoming commands
    // JVS_UpdateInputs() is called internally
}
```

## Testing

### Test with TeknoParrot
1. Set HIDO to JVS mode (comment out USE_DIRECT_BUTTONS)
2. Compile and flash firmware
3. Connect RS485 to arcade board
4. Launch TeknoParrot with JVS test utility
5. Verify device appears as "HIDO Arcade Controller"

### Loopback Test
1. Short TX to RX on RS485 transceiver
2. Send test packets, should receive echo
3. Verify escape sequences work correctly
4. Check checksum validation

### Timing
- Response latency: < 10ms typical
- Packet processing: ~1-2ms per command
- Sense line activation: < 1ms

## Troubleshooting

### Device Not Detected
- Check RS485 wiring polarity (swap D and /D if needed)
- Verify sense line voltage (should be 2.5V when ready)
- Confirm baud rate is 115200 on both sides

### Communication Errors
- Check for noise on RS485 lines (use shielded cable)
- Verify termination resistors (120Ω) on RS485 bus
- Test with shorter cable first

### Wrong Button Readings
- Verify GPIO mapping in JVS_UpdateInputs()
- Check button active state (LOW vs HIGH)
- Confirm pull-up resistors are enabled

## References

- JVS Specification 3.0: [arcade-docs.com](https://arcade-docs.com)
- OpenJVS Implementation: [github.com/OpenJVS/OpenJVS](https://github.com/OpenJVS/OpenJVS)
- SN65HVD1786D Datasheet: [ti.com](https://www.ti.com/product/SN65HVD1786)

## License
Same as main project (see root LICENSE file)
