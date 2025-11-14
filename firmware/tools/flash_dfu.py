#!/usr/bin/env python3
"""
HIDO DFU Trigger - Simple USB HID command sender
Sends a magic key combination to trigger bootloader entry

No pyusb required - uses pure HID protocol
"""

import sys
import time
import subprocess
import os
from pathlib import Path

def print_banner():
    print("=" * 60)
    print("  HIDO Firmware Updater - Simple Mode")
    print("=" * 60)
    print()

def check_dfu_device():
    """Check if STM32 is in DFU mode"""
    try:
        # Use dfu-util from same directory
        script_dir = Path(__file__).parent
        dfu_util = script_dir / "dfu-util.exe"
        
        if not dfu_util.exists():
            dfu_util = "dfu-util"  # Try system path
        
        result = subprocess.run([str(dfu_util), '-l'], 
                              capture_output=True, text=True, timeout=5)
        
        if '0483:df11' in result.stdout.lower() or 'stm32' in result.stdout.lower():
            return True
    except:
        pass
    return False

def flash_firmware(firmware_path):
    """Flash firmware using dfu-util"""
    print()
    print(f"📦 Flashing: {firmware_path}")
    print(f"📊 Size: {os.path.getsize(firmware_path):,} bytes")
    print()
    print("=" * 60)
    
    script_dir = Path(__file__).parent
    dfu_util = script_dir / "dfu-util.exe"
    
    if not dfu_util.exists():
        dfu_util = "dfu-util"
    
    cmd = [
        str(dfu_util),
        '-a', '0',
        '-s', '0x08000000:leave',
        '-D', firmware_path
    ]
    
    try:
        result = subprocess.run(cmd, timeout=60)
        
        if result.returncode == 0:
            print()
            print("=" * 60)
            print("✅ FLASH SUCCESSFUL!")
            print("=" * 60)
            print()
            print("Device should automatically restart.")
            return True
        else:
            print()
            print("❌ Flash failed!")
            return False
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def main():
    print_banner()
    
    # Find firmware
    firmware = Path("..") / "build" / "hido.bin"
    
    if not firmware.exists():
        print(f"❌ Firmware not found: {firmware}")
        print()
        print("Please compile firmware first:")
        print("  cd firmware")
        print("  ./build.ps1")
        input("\nPress Enter to exit...")
        return 1
    
    print(f"✅ Firmware found: {firmware}")
    print()
    
    # Check if already in DFU mode
    print("🔍 Checking for DFU device...")
    if check_dfu_device():
        print("✅ STM32 already in DFU mode!")
        return 0 if flash_firmware(str(firmware)) else 1
    
    print("⚠️  Device not in DFU mode yet")
    print()
    print("=" * 60)
    print("  MANUAL DFU ENTRY REQUIRED")
    print("=" * 60)
    print()
    print("Please put the device in DFU mode:")
    print()
    print("  1. ⚡ Disconnect USB cable")
    print("  2. 🔗 Connect BOOT0 (pin 60) to 3.3V")
    print("     - Use a jumper wire or paperclip")
    print("     - On STM32: pin marked 'BOOT0' → pin marked '3V3'")
    print("  3. 🔌 Reconnect USB while holding connection")
    print("  4. ⏱️  Wait 1 second")
    print("  5. ✂️  Remove the jumper")
    print()
    print("=" * 60)
    print()
    input("Press Enter when device is in DFU mode...")
    
    # Wait for DFU device
    print()
    print("🔍 Waiting for DFU device...")
    
    for attempt in range(10):
        if check_dfu_device():
            print("✅ DFU device detected!")
            print()
            time.sleep(0.5)
            return 0 if flash_firmware(str(firmware)) else 1
        
        print(f"  ⏳ Attempt {attempt + 1}/10...")
        time.sleep(1)
    
    print()
    print("❌ DFU device not detected!")
    print()
    print("Troubleshooting:")
    print("  • Verify BOOT0 was connected to 3.3V during USB connect")
    print("  • Try a different USB port")
    print("  • Windows: Install STM32 DFU drivers or use Zadig")
    print()
    input("Press Enter to exit...")
    return 1

if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\n\n⚠️  Aborted by user")
        sys.exit(1)

