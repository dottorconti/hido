#!/usr/bin/env python3
"""
Test PyUSB Installation and USB Connection
Quick diagnostic tool to verify everything is working
"""

import sys

def test_pyusb():
    """Test if PyUSB is installed and working"""
    print("=" * 60)
    print("HIDO Configuration Tools - System Check")
    print("=" * 60)
    print()
    
    # Test 1: PyUSB import
    print("[1/4] Testing PyUSB installation...", end=" ")
    try:
        import usb.core
        import usb.util
        print("✓ OK")
    except ImportError as e:
        print("✗ FAILED")
        print(f"      Error: {e}")
        print("      Fix: pip install pyusb")
        return False
    
    # Test 2: Backend
    print("[2/4] Testing libusb backend...", end=" ")
    try:
        backend = usb.backend.libusb1.get_backend()
        if backend is None:
            print("✗ FAILED")
            print("      Error: libusb backend not found")
            print("      Fix Windows: Copy libusb-1.0.dll to this folder")
            print("      Fix Linux: sudo apt install libusb-1.0-0")
            return False
        print("✓ OK")
        print(f"      Backend: {backend}")
    except Exception as e:
        print("✗ FAILED")
        print(f"      Error: {e}")
        return False
    
    # Test 3: List USB devices
    print("[3/4] Scanning USB devices...", end=" ")
    try:
        devices = list(usb.core.find(find_all=True))
        print(f"✓ OK ({len(devices)} devices found)")
        
        if len(devices) == 0:
            print("      WARNING: No USB devices detected!")
            print("      This might be a permission issue (Linux) or driver issue (Windows)")
        else:
            print("      Devices:")
            for dev in devices[:5]:  # Show first 5
                try:
                    mfg = dev.manufacturer if dev.manufacturer else "N/A"
                    prod = dev.product if dev.product else "N/A"
                    print(f"        - VID:{dev.idVendor:04X} PID:{dev.idProduct:04X} {mfg} {prod}")
                except:
                    print(f"        - VID:{dev.idVendor:04X} PID:{dev.idProduct:04X}")
            
            if len(devices) > 5:
                print(f"        ... and {len(devices)-5} more")
    except Exception as e:
        print("✗ FAILED")
        print(f"      Error: {e}")
        return False
    
    # Test 4: Find HIDO device
    print("[4/4] Looking for HIDO device...", end=" ")
    try:
        VENDOR_ID = 0x0483
        PRODUCT_ID = 0x572B
        
        hido = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
        
        if hido is None:
            print("✗ NOT FOUND")
            print(f"      Looking for VID:{VENDOR_ID:04X} PID:{PRODUCT_ID:04X}")
            print("      Make sure:")
            print("        • Device is connected via USB")
            print("        • Firmware is flashed correctly")
            print("        • Device is not in DFU mode (PID 0xDF11)")
            print("        • Driver is installed (Windows: use Zadig)")
        else:
            print("✓ FOUND")
            try:
                print(f"      Manufacturer: {hido.manufacturer if hido.manufacturer else 'N/A'}")
                print(f"      Product: {hido.product if hido.product else 'N/A'}")
                print(f"      Serial: {hido.serial_number if hido.serial_number else 'N/A'}")
                
                # Try to read firmware version
                try:
                    version_data = hido.ctrl_transfer(
                        bmRequestType=0xC0,
                        bRequest=0xAA,  # GET_VERSION
                        wValue=0,
                        wIndex=0,
                        data_or_wLength=3
                    )
                    fw_version = f"{version_data[0]}.{version_data[1]}.{version_data[2]}"
                    print(f"      Firmware: v{fw_version}")
                except:
                    print("      Firmware: (unable to read - might need driver)")
                    
            except Exception as e:
                print(f"      Note: Device found but cannot read details")
                print(f"      This is normal on Windows without WinUSB driver")
                print(f"      Use Zadig to install driver for full functionality")
                
            print()
            print("=" * 60)
            print("✓ ALL CHECKS PASSED - Ready to use config tools!")
            print("=" * 60)
            return True
            
    except Exception as e:
        print("✗ FAILED")
        print(f"      Error: {e}")
        return False
    
    print()
    print("=" * 60)
    print("System check completed with warnings")
    print("=" * 60)
    return True


if __name__ == '__main__':
    success = test_pyusb()
    
    if not success:
        print()
        print("⚠️  Some checks failed. Please fix the issues above.")
        print()
        input("Press Enter to exit...")
        sys.exit(1)
    else:
        print()
        print("Next steps:")
        print("  • Run config_tool_gui.py for graphical interface")
        print("  • Run config_tool.py for command-line interface")
        print("  • See CONFIG_TOOLS_README.md for full documentation")
        print()
        input("Press Enter to exit...")
        sys.exit(0)
