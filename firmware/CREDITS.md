# Credits and Acknowledgments

## Third-Party Libraries and References

### OpenJVS - JVS Protocol Implementation Reference
- **Repository**: [OpenJVS/OpenJVS](https://github.com/OpenJVS/OpenJVS)
- **Author**: Bobby Dilley and contributors
- **License**: GPL-3.0
- **Usage**: JVS protocol implementation in `Core/Src/jvs_protocol.c` is based on the OpenJVS reference implementation
- **Description**: OpenJVS is a Linux-based JVS emulator that provided invaluable reference for:
  - JVS packet structure and escape sequences
  - Command codes and response formats
  - Sense line control
  - RS485 communication patterns
  - Capability reporting structure

**Specific references from OpenJVS:**
- Packet parsing with SYNC (0xE0) and ESCAPE (0xD0) byte handling
- Checksum calculation algorithm
- JVS 3.0 protocol specification implementation
- Command handling patterns (reset, assign address, capabilities, read switches)

### STMicroelectronics HAL Drivers
- **Library**: STM32Cube HAL Drivers for STM32F1 series
- **Author**: STMicroelectronics
- **License**: BSD-3-Clause
- **Usage**: Hardware abstraction layer for STM32F102RBT6
- **Files**: `Drivers/STM32F1xx_HAL_Driver/*`

### STM32 USB Device Library
- **Library**: STM32 USB Device Library
- **Author**: STMicroelectronics
- **License**: BSD-3-Clause
- **Usage**: USB HID device implementation
- **Files**: `Middlewares/ST/STM32_USB_Device_Library/*`

## Documentation References

- **USB HID Specification**: USB.org - HID Usage Tables
- **JVS Specification**: JAMMA Video Standard 3.0 (arcade-docs.com)
- **RS485 Standard**: TIA/EIA-485-A

## Hardware Components

- **MCU**: STM32F102RBT6 - STMicroelectronics
- **RS485 Transceiver**: SN65HVD1786D - Texas Instruments

## Special Thanks

- **OpenJVS Project** for the comprehensive JVS protocol documentation and reference code
- **STMicroelectronics** for the excellent HAL drivers and USB libraries
- **Bobby Dilley** (OpenJVS maintainer) for the well-documented JVS implementation
- The arcade preservation community for maintaining JVS documentation

## License Compliance

This project (HIDO) is released under MIT License (see LICENSE file).

The JVS protocol implementation is inspired by OpenJVS (GPL-3.0), but has been:
- Completely rewritten for STM32 embedded environment
- Adapted for hardware-specific requirements (SN65HVD1786D transceiver)
- Optimized for real-time arcade button scanning
- Integrated with custom NKRO keyboard implementation

If you use this code in a GPL-3.0 project, you must comply with GPL-3.0 terms.
For commercial use, consult the respective component licenses.

## Contributing

If you improve this implementation or find it useful, please consider:
1. Contributing back improvements to this repository
2. Supporting the OpenJVS project
3. Documenting arcade hardware preservation efforts

---

*Last updated: November 2025*
