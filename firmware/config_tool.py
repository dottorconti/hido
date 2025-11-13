#!/usr/bin/env python3
"""
HIDO Configuration Tool
Read, modify, and write pin mapping configuration via USB
"""

import usb.core
import usb.util
import struct
import sys
import json

# USB Vendor IDs
VENDOR_ID = 0x0483  # STMicroelectronics VID
PRODUCT_ID = 0x572B # HIDO PID

# USB Vendor Commands
CMD_CONFIG_READ = 0xC0
CMD_CONFIG_WRITE = 0xC1
CMD_CONFIG_RESET = 0xC2

# Configuration constants
CONFIG_MAGIC = 0x48494430  # "HID0"
CONFIG_VERSION = 1
MAX_PINS = 17

# HID Keycode mapping (USB HID Usage IDs)
HID_KEYS = {
    0x04: 'A', 0x05: 'B', 0x06: 'C', 0x07: 'D', 0x08: 'E', 0x09: 'F',
    0x0A: 'G', 0x0B: 'H', 0x0C: 'I', 0x0D: 'J', 0x0E: 'K', 0x0F: 'L',
    0x10: 'M', 0x11: 'N', 0x12: 'O', 0x13: 'P', 0x14: 'Q', 0x15: 'R',
    0x16: 'S', 0x17: 'T', 0x18: 'U', 0x19: 'V', 0x1A: 'W', 0x1B: 'X',
    0x1C: 'Y', 0x1D: 'Z',
    0x1E: '1', 0x1F: '2', 0x20: '3', 0x21: '4', 0x22: '5',
    0x23: '6', 0x24: '7', 0x25: '8', 0x26: '9', 0x27: '0',
    0x28: 'ENTER', 0x29: 'ESC', 0x2A: 'BACKSPACE', 0x2B: 'TAB',
    0x2C: 'SPACE', 0x2D: '-', 0x2E: '=', 0x2F: '[', 0x30: ']',
    0x4F: 'RIGHT', 0x50: 'LEFT', 0x51: 'DOWN', 0x52: 'UP',
    0x54: '/', 0x55: '*', 0x56: '-', 0x57: '+',
    0x59: 'Num1', 0x5A: 'Num2', 0x5B: 'Num3', 0x5C: 'Num4',
    0x5D: 'Num5', 0x5E: 'Num6', 0x5F: 'Num7', 0x60: 'Num8',
    0x61: 'Num9', 0x62: 'Num0', 0x63: 'Num.'
}

# Reverse mapping
KEY_TO_HID = {v: k for k, v in HID_KEYS.items()}

# Joystick functions
JOY_FUNCTIONS = [
    'Button 1', 'Button 2', 'Button 3', 'Button 4', 'Button 5', 'Button 6',
    'Button 7', 'Button 8', 'Button 9', 'Button 10', 'Button 11', 'Button 12',
    'Button 13', 'Button 14', 'UP', 'DOWN', 'LEFT', 'RIGHT', 'Disabled'
]

def find_device():
    """Find HIDO device"""
    dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
    if dev is None:
        print("ERROR: HIDO device not found!")
        print(f"Looking for VID:PID = {VENDOR_ID:04X}:{PRODUCT_ID:04X}")
        print("\nMake sure:")
        print("  1. Device is connected via USB")
        print("  2. Firmware is flashed")
        print("  3. Device shows up in Device Manager")
        return None
    
    # Detach kernel driver if necessary (Linux only)
    if sys.platform.startswith('linux'):
        try:
            if dev.is_kernel_driver_active(0):
                dev.detach_kernel_driver(0)
        except:
            pass
    
    return dev

def read_config(dev):
    """Read configuration from device"""
    try:
        # Send control transfer to read config
        data = dev.ctrl_transfer(
            bmRequestType=0xC0,  # Device-to-Host, Vendor, Device
            bRequest=CMD_CONFIG_READ,
            wValue=0,
            wIndex=0,
            data_or_wLength=1024  # Max buffer size
        )
        
        if len(data) < 8:
            print("ERROR: Invalid config data received")
            return None
        
        # Parse header
        magic, version = struct.unpack('<II', bytes(data[0:8]))
        
        if magic != CONFIG_MAGIC:
            print(f"ERROR: Invalid magic number: 0x{magic:08X} (expected 0x{CONFIG_MAGIC:08X})")
            return None
        
        if version != CONFIG_VERSION:
            print(f"WARNING: Config version mismatch: {version} (expected {CONFIG_VERSION})")
        
        return bytes(data)
        
    except usb.core.USBError as e:
        print(f"ERROR reading config: {e}")
        return None

def parse_keyboard_config(data):
    """Parse keyboard mode configuration"""
    config = {
        'magic': struct.unpack('<I', data[0:4])[0],
        'version': struct.unpack('<I', data[4:8])[0],
        'player1': [],
        'player2': []
    }
    
    offset = 8
    
    # Parse Player 1 (17 mappings)
    for i in range(MAX_PINS):
        silk_pin = data[offset]
        hid_keycode = data[offset + 1]
        key_name = data[offset + 2:offset + 18].decode('utf-8', errors='ignore').rstrip('\x00')
        
        config['player1'].append({
            'silk_pin': silk_pin,
            'hid_keycode': hid_keycode,
            'key_name': key_name
        })
        offset += 18
    
    # Parse Player 2 (17 mappings)
    for i in range(MAX_PINS):
        silk_pin = data[offset]
        hid_keycode = data[offset + 1]
        key_name = data[offset + 2:offset + 18].decode('utf-8', errors='ignore').rstrip('\x00')
        
        config['player2'].append({
            'silk_pin': silk_pin,
            'hid_keycode': hid_keycode,
            'key_name': key_name
        })
        offset += 18
    
    config['crc32'] = struct.unpack('<I', data[offset:offset+4])[0]
    
    return config

def parse_joystick_config(data):
    """Parse joystick mode configuration"""
    config = {
        'magic': struct.unpack('<I', data[0:4])[0],
        'version': struct.unpack('<I', data[4:8])[0],
        'player1': [],
        'player2': []
    }
    
    offset = 8
    
    # Parse Player 1 (17 mappings)
    for i in range(MAX_PINS):
        silk_pin = data[offset]
        joy_function = data[offset + 1]
        func_name = data[offset + 2:offset + 18].decode('utf-8', errors='ignore').rstrip('\x00')
        
        config['player1'].append({
            'silk_pin': silk_pin,
            'joy_function': joy_function,
            'func_name': func_name
        })
        offset += 18
    
    # Parse Player 2 (17 mappings)
    for i in range(MAX_PINS):
        silk_pin = data[offset]
        joy_function = data[offset + 1]
        func_name = data[offset + 2:offset + 18].decode('utf-8', errors='ignore').rstrip('\x00')
        
        config['player2'].append({
            'silk_pin': silk_pin,
            'joy_function': joy_function,
            'func_name': func_name
        })
        offset += 18
    
    config['crc32'] = struct.unpack('<I', data[offset:offset+4])[0]
    
    return config

def print_keyboard_config(config):
    """Display keyboard configuration"""
    print("\n" + "="*70)
    print("KEYBOARD MODE CONFIGURATION")
    print("="*70)
    
    print("\nPlayer 1:")
    print("-" * 50)
    print(f"{'Pin':<6} {'Silkscreen':<12} {'Key':<8} {'HID Code':<10}")
    print("-" * 50)
    for i, mapping in enumerate(config['player1']):
        key_display = HID_KEYS.get(mapping['hid_keycode'], f"0x{mapping['hid_keycode']:02X}")
        print(f"{i:<6} {mapping['silk_pin']:<12} {mapping['key_name']:<8} {key_display} (0x{mapping['hid_keycode']:02X})")
    
    print("\nPlayer 2:")
    print("-" * 50)
    print(f"{'Pin':<6} {'Silkscreen':<12} {'Key':<8} {'HID Code':<10}")
    print("-" * 50)
    for i, mapping in enumerate(config['player2']):
        key_display = HID_KEYS.get(mapping['hid_keycode'], f"0x{mapping['hid_keycode']:02X}")
        print(f"{i:<6} {mapping['silk_pin']:<12} {mapping['key_name']:<8} {key_display} (0x{mapping['hid_keycode']:02X})")
    
    print(f"\nCRC32: 0x{config['crc32']:08X}")
    print("="*70)

def print_joystick_config(config):
    """Display joystick configuration"""
    print("\n" + "="*70)
    print("JOYSTICK MODE CONFIGURATION")
    print("="*70)
    
    print("\nPlayer 1:")
    print("-" * 50)
    print(f"{'Pin':<6} {'Silkscreen':<12} {'Function':<20}")
    print("-" * 50)
    for i, mapping in enumerate(config['player1']):
        print(f"{i:<6} {mapping['silk_pin']:<12} {mapping['func_name']:<20}")
    
    print("\nPlayer 2:")
    print("-" * 50)
    print(f"{'Pin':<6} {'Silkscreen':<12} {'Function':<20}")
    print("-" * 50)
    for i, mapping in enumerate(config['player2']):
        print(f"{i:<6} {mapping['silk_pin']:<12} {mapping['func_name']:<20}")
    
    print(f"\nCRC32: 0x{config['crc32']:08X}")
    print("="*70)

def reset_config(dev):
    """Reset configuration to defaults"""
    try:
        dev.ctrl_transfer(
            bmRequestType=0x40,  # Host-to-Device, Vendor, Device
            bRequest=CMD_CONFIG_RESET,
            wValue=0,
            wIndex=0,
            data_or_wLength=0
        )
        print("✓ Configuration reset to defaults")
        return True
    except usb.core.USBError as e:
        print(f"ERROR resetting config: {e}")
        return False

def main():
    print("="*70)
    print("HIDO Configuration Tool v1.0")
    print("="*70)
    
    # Find device
    print("\nSearching for HIDO device...")
    dev = find_device()
    if dev is None:
        return 1
    
    print(f"✓ Found device: {dev.manufacturer} {dev.product}")
    print(f"  Serial: {dev.serial_number}")
    
    # Read configuration
    print("\nReading configuration...")
    data = read_config(dev)
    if data is None:
        return 1
    
    print(f"✓ Read {len(data)} bytes")
    
    # Detect mode and parse
    # For now, assume keyboard mode (check first mapping structure)
    # In real implementation, you'd check device descriptor or ask firmware
    
    try:
        config = parse_keyboard_config(data)
        print_keyboard_config(config)
        mode = "keyboard"
    except:
        try:
            config = parse_joystick_config(data)
            print_joystick_config(config)
            mode = "joystick"
        except Exception as e:
            print(f"ERROR parsing config: {e}")
            return 1
    
    # Menu
    while True:
        print("\nOptions:")
        print("  [R] Reset to defaults")
        print("  [E] Export to JSON")
        print("  [Q] Quit")
        
        choice = input("\nSelect option: ").strip().upper()
        
        if choice == 'R':
            confirm = input("Reset configuration to defaults? (yes/no): ").strip().lower()
            if confirm == 'yes':
                if reset_config(dev):
                    # Re-read config
                    data = read_config(dev)
                    if data:
                        config = parse_keyboard_config(data) if mode == "keyboard" else parse_joystick_config(data)
                        print_keyboard_config(config) if mode == "keyboard" else print_joystick_config(config)
        
        elif choice == 'E':
            filename = input("Enter filename (default: hido_config.json): ").strip()
            if not filename:
                filename = "hido_config.json"
            
            with open(filename, 'w') as f:
                json.dump(config, f, indent=2)
            print(f"✓ Configuration exported to {filename}")
        
        elif choice == 'Q':
            break
    
    print("\nBye!")
    return 0

if __name__ == '__main__':
    sys.exit(main())
