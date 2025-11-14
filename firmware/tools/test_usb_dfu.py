#!/usr/bin/env python3
"""
Quick test - Find HIDO device and try to enter DFU bootloader
"""

import usb.core
import usb.util
import time

# Known STM32 VID
STM32_VID = 0x0483

print("Searching for STM32-based USB devices...")
print()

# Find all USB devices with STM32 VID
devices = list(usb.core.find(find_all=True, idVendor=STM32_VID))

if not devices:
    print("No STM32 devices found!")
    print()
    print("Trying to find ANY HID device...")
    # Try to find any HID device
    devices = list(usb.core.find(find_all=True, bDeviceClass=0x03))  # HID class

if not devices:
    print("No USB devices found at all!")
    exit(1)

print(f"Found {len(devices)} device(s):")
print()

for dev in devices:
    print(f"Device: VID=0x{dev.idVendor:04X}, PID=0x{dev.idProduct:04X}")
    print(f"  Bus {dev.bus}, Address {dev.address}")
    
    try:
        manufacturer = usb.util.get_string(dev, dev.iManufacturer)
        product = usb.util.get_string(dev, dev.iProduct)
        print(f"  Manufacturer: {manufacturer}")
        print(f"  Product: {product}")
    except:
        print(f"  (Cannot read strings)")
    
    print()
    
    # If this is HIDO (STM32 VID, PID 0x572B from earlier scan)
    if dev.idVendor == 0x0483 and dev.idProduct == 0x572B:
        print("⭐ This looks like HIDO!")
        print()
        print("Attempting to send bootloader command...")
        
        try:
            # Detach kernel driver if needed
            if dev.is_kernel_driver_active(0):
                try:
                    dev.detach_kernel_driver(0)
                except:
                    pass
            
            # Try vendor-specific control transfer
            # bmRequestType: 0x40 = Host-to-Device, Vendor, Device
            result = dev.ctrl_transfer(
                bmRequestType=0x40,
                bRequest=0xBB,      # Bootloader request
                wValue=0xB007,      # Magic value
                wIndex=0,
                data_or_wLength=None,
                timeout=1000
            )
            
            print("✓ Command sent! Device should reset into DFU mode...")
            time.sleep(3)
            
        except usb.core.USBError as e:
            if e.errno == 19:  # Device disappeared (reset)
                print("✓ Device reset! Check for DFU mode...")
            else:
                print(f"⚠ USB Error: {e}")
                print()
                print("Firmware might not have DFU command support yet.")
                print("Use manual BOOT0 method instead.")

print()
print("=" * 60)
print("Checking for DFU device...")
print()

# Check for DFU device
import subprocess
result = subprocess.run(['dfu-util.exe', '-l'], capture_output=True, text=True)
if '0483:df11' in result.stdout.lower():
    print("✓ STM32 DFU device detected!")
    print()
    print("Ready to flash! Run:")
    print("  python flash_dfu.py")
else:
    print("✗ No DFU device found")
    print()
    print("Manual DFU entry required:")
    print("  1. Disconnect USB")
    print("  2. Connect BOOT0 to 3.3V")
    print("  3. Reconnect USB")
    print("  4. Run: python flash_dfu.py")
