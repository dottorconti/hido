#!/usr/bin/env python3
"""
HIDO Firmware Updater
Easy USB DFU firmware update tool for HIDO Arcade Controller

Requirements:
    pip install pyusb libusb

Usage:
    python dfu_update.py firmware.bin
    
    Or double-click to auto-detect firmware in current directory
"""

import sys
import os
import time
import subprocess
from pathlib import Path

# HIDO Device identifiers
HIDO_VID = 0x1234  # Your custom VID (update this!)
HIDO_PID_KEYBOARD = 0x5678  # Keyboard mode PID
HIDO_PID_JOYSTICK = 0x5679  # Joystick mode PID

# STM32 DFU Bootloader identifiers
STM32_DFU_VID = 0x0483
STM32_DFU_PID = 0xDF11

def print_banner():
    print("=" * 60)
    print("  HIDO Firmware Updater")
    print("  STM32 USB DFU Flash Tool")
    print("=" * 60)
    print()

def check_dfu_util():
    """Check if dfu-util is installed"""
    try:
        result = subprocess.run(['dfu-util', '--version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print("✓ dfu-util found:", result.stdout.split('\n')[0])
            return True
    except FileNotFoundError:
        pass
    
    print("✗ dfu-util not found!")
    print()
    print("Please install dfu-util:")
    print("  Windows: Download from http://dfu-util.sourceforge.net/")
    print("           Or use: choco install dfu-util")
    print("  Linux:   sudo apt install dfu-util")
    print("  macOS:   brew install dfu-util")
    return False

def find_firmware():
    """Auto-detect firmware file in current directory"""
    patterns = ['*.bin', 'hido*.bin', 'firmware*.bin', 'build/hido.bin']
    
    for pattern in patterns:
        files = list(Path('.').glob(pattern))
        if files:
            # Sort by modification time, newest first
            files.sort(key=lambda x: x.stat().st_mtime, reverse=True)
            return str(files[0])
    
    return None

def send_bootloader_command():
    """Send command to HIDO to enter DFU bootloader"""
    try:
        import usb.core
        import usb.util
        
        print("Searching for HIDO device...")
        
        # Try to find HIDO in keyboard or joystick mode
        dev = usb.core.find(idVendor=HIDO_VID, idProduct=HIDO_PID_KEYBOARD)
        if dev is None:
            dev = usb.core.find(idVendor=HIDO_VID, idProduct=HIDO_PID_JOYSTICK)
        
        if dev is None:
            print("✗ HIDO device not found in normal mode")
            print("  Device might already be in DFU mode or disconnected")
            return False
        
        print(f"✓ Found HIDO device at {dev.bus}-{dev.address}")
        
        # Detach kernel driver if necessary (Linux)
        try:
            if dev.is_kernel_driver_active(0):
                dev.detach_kernel_driver(0)
        except:
            pass
        
        # Send vendor-specific control transfer to enter bootloader
        # bmRequestType: 0x40 = Host-to-Device, Vendor, Device
        # bRequest: 0xBB = Enter Bootloader command
        # wValue: 0xB007 = Magic value for confirmation
        # wIndex: 0
        BOOTLOADER_REQUEST = 0xBB
        BOOTLOADER_MAGIC = 0xB007
        
        print("  Sending bootloader entry command via USB...")
        
        try:
            result = dev.ctrl_transfer(
                bmRequestType=0x40,      # Host-to-Device, Vendor, Device
                bRequest=BOOTLOADER_REQUEST,
                wValue=BOOTLOADER_MAGIC,
                wIndex=0,
                data_or_wLength=None,
                timeout=1000
            )
            
            print("✓ Command sent successfully!")
            print("  Device should reset into DFU mode...")
            
            # Give device time to reset
            time.sleep(2)
            
            return True
            
        except usb.core.USBError as e:
            if e.errno == 19:  # No such device (device reset)
                print("✓ Device acknowledged and is resetting...")
                time.sleep(2)
                return True
            else:
                print(f"✗ USB communication error: {e}")
                return False
        
    except ImportError:
        print("⚠ pyusb not installed - skipping automatic bootloader entry")
        print("  Install with: pip install pyusb")
        print()
        print("Manual entry: Hold BOOT button while connecting USB")
        return False
    except Exception as e:
        print(f"⚠ Could not communicate with device: {e}")
        return False

def check_dfu_device():
    """Check if STM32 is in DFU mode"""
    result = subprocess.run(['dfu-util', '-l'], 
                          capture_output=True, text=True)
    
    if 'STM32' in result.stdout or f'{STM32_DFU_VID:04x}:{STM32_DFU_PID:04x}' in result.stdout:
        print("✓ STM32 DFU device detected")
        return True
    else:
        print("✗ STM32 DFU device not found")
        return False

def flash_firmware(firmware_path):
    """Flash firmware using dfu-util"""
    print()
    print(f"Flashing: {firmware_path}")
    print(f"Size: {os.path.getsize(firmware_path)} bytes")
    print()
    print("=" * 60)
    
    cmd = [
        'dfu-util',
        '-a', '0',                          # Alt setting 0 (internal flash)
        '-s', '0x08000000:leave',           # Flash address + auto-reset after flash
        '-D', firmware_path                 # Firmware file
    ]
    
    try:
        result = subprocess.run(cmd, text=True)
        
        if result.returncode == 0:
            print()
            print("=" * 60)
            print("✓ Firmware update successful!")
            print("  Device should automatically restart...")
            return True
        else:
            print()
            print("✗ Firmware update failed!")
            return False
            
    except Exception as e:
        print(f"✗ Error during flash: {e}")
        return False

def main():
    print_banner()
    
    # Check for dfu-util
    if not check_dfu_util():
        input("\nPress Enter to exit...")
        return 1
    
    # Get firmware file
    firmware = None
    if len(sys.argv) > 1:
        firmware = sys.argv[1]
    else:
        print("Auto-detecting firmware file...")
        firmware = find_firmware()
    
    if firmware is None:
        print("✗ No firmware file specified or found")
        print()
        print("Usage:")
        print(f"  {sys.argv[0]} firmware.bin")
        input("\nPress Enter to exit...")
        return 1
    
    if not os.path.exists(firmware):
        print(f"✗ Firmware file not found: {firmware}")
        input("\nPress Enter to exit...")
        return 1
    
    print(f"✓ Firmware file: {firmware}")
    print()
    
    # Try to send bootloader command to HIDO
    send_bootloader_command()
    
    # Check for DFU device
    print()
    print("Checking for STM32 DFU device...")
    
    for attempt in range(5):
        if check_dfu_device():
            break
        
        if attempt == 0:
            print()
            print("Please put device in DFU mode:")
            print("  1. Disconnect USB")
            print("  2. Hold BOOT button (or short BOOT0 to 3.3V)")
            print("  3. Connect USB while holding BOOT")
            print("  4. Release BOOT button")
            print()
        
        print(f"  Waiting... ({attempt+1}/5)")
        time.sleep(2)
    
    if not check_dfu_device():
        print()
        print("✗ Could not detect DFU device")
        input("\nPress Enter to exit...")
        return 1
    
    # Flash firmware
    if flash_firmware(firmware):
        print()
        print("Update complete! Device is ready to use.")
        input("\nPress Enter to exit...")
        return 0
    else:
        input("\nPress Enter to exit...")
        return 1

if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\n\nAborted by user")
        sys.exit(1)
