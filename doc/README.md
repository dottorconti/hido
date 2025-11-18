- **Key Rollover**: 96 keys NKRO (all arcade buttons + extras)

## 🐛 Troubleshooting

### Computer doesn't recognize device
- Check USB cable and connections
- Verify LED1 blinks on startup
- Try different USB port

### Keys not responding
1. Check button wiring (should connect pin to GND when pressed)
2. Verify GPIO mapping in `arcade_keyboard.c`
3. Test with LED indicators in DEBUG mode

### Joystick Mode Notes
- Removed `HAL_Delay(1)` between joystick reports to avoid USB timing issues that could freeze the host.
- Restored natural mapping: Report ID 1 -> physical Player 1, Report ID 2 -> physical Player 2.
- Firmware now sends each HID joystick report only when its contents change (reduces USB traffic and host-side confusion).
- If Player 2 feels slow, try reducing `DEBOUNCE_TIME_MS` in `Core/Inc/arcade_joystick.h` (2-3 ms is a common compromise).
- To rebuild and flash joystick firmware from the `firmware` folder:

```powershell
.\compile_direct.ps1 -Mode joystick
.\flash.ps1
```

### Ghosting issues
- Should not occur with NKRO! Check USB host supports it
- Some BIOS/legacy systems might not support NKRO

### Input lag
- Ensure USB polling is 1000Hz in Device Manager (Windows)
- Check no other USB devices causing conflicts
- Disable USB power management in OS settings

## 📊 Technical Details

**MCU**: STM32F102RBT6
- 128KB Flash, 20KB RAM
- 72MHz ARM Cortex-M3
- USB 2.0 Full Speed

**USB Configuration:**
- VID: 0x0483 (STMicroelectronics)
- PID: 0x572B
- Class: HID (Keyboard or Joystick depending on mode)
- Current Mode: **Dual Joystick** (2 separate devices)
- HID Descriptor: 102 bytes (TWO Application Collections)

### USB HID Descriptors (summary)

Keyboard (NKRO) descriptor (built when `-Mode keyboard`):
- Report: NKRO keyboard (bitmap-style) — supports many simultaneous keys
- Typical report: NKRO bitmap + modifiers (implementation is in `Core/Src/usbd_hid_custom.c` / `Core/Inc/usbd_hid_custom.h`)
- Polling rate: 1ms

Joystick descriptor (built when `-Mode joystick`):
- Report IDs: `1` = Player 1, `2` = Player 2
- Each report: 5 bytes = `[ReportID][X][Y][ButtonsLow][ButtonsHigh]` (13 button bits used)
- Polling rate: 1ms

All HID descriptor code and the per-mode report layout live under `Core/Src/usbd_hid_custom.c` and the HID middleware files. For exact byte-level descriptors see `Core/Src/usbd_hid_custom.c`.

## 🔄 Updates from Original

- ✅ Updated USB HID libraries (2015 → modern)
- ✅ Replaced mouse descriptor with NKRO keyboard
- ✅ **NEW**: Dual joystick mode with TWO Application Collections (2 separate Windows devices)
- ✅ Reduced USB interval: 10ms → 1ms (10x faster)
- ✅ Added GPIO pull-ups for reliable button reading
- ✅ Implemented proper debouncing algorithm
- ✅ Removed blocking delays for minimal latency
- ✅ Added structured button mapping system
- ✅ Prepared infrastructure for JVS support
- ✅ Fixed HID descriptor byte count (102 bytes for dual joystick)
- ✅ MAME/arcade frontend compatible (sees 2 separate controllers)

## � Credits and Acknowledgments

This project uses and references the following open-source projects:

- **[OpenJVS](https://github.com/OpenJVS/OpenJVS)** by Bobby Dilley (GPL-3.0)
  - JVS protocol specification and reference implementation
  - Packet structure, command codes, and RS485 communication patterns
  - Our JVS implementation is inspired by OpenJVS but completely rewritten for STM32

- **STM32Cube HAL Drivers** by STMicroelectronics (BSD-3-Clause)
  - Hardware abstraction layer for STM32F1 series

- **STM32 USB Device Library** by STMicroelectronics (BSD-3-Clause)
  - USB HID device implementation

See [CREDITS.md](CREDITS.md) for complete attribution and license information.

## 📜 License

MIT License (for HIDO-specific code)

**Note**: JVS implementation is inspired by OpenJVS (GPL-3.0). If you use the JVS portions in a GPL project, you must comply with GPL-3.0 terms.

## 👤 Authors

- **Original Hardware**: dottorconti (2020)
- **Firmware Rewrite**: 2025 - Complete NKRO + JVS implementation

---

**Support the open-source arcade community!** ⭐
- Star this repository if you find it useful
- Check out [OpenJVS](https://github.com/OpenJVS/OpenJVS) for Linux-based JVS emulation
- Contribute to arcade hardware preservation
