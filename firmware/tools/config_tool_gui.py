#!/usr/bin/env python3
"""
HIDO Configuration Tool - GUI Version
Graphical interface for reading, modifying, and writing pin mapping configuration
"""

import usb.core
import usb.util
import struct
import sys
import json
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import threading

# USB Vendor IDs
VENDOR_ID = 0x0483  # STMicroelectronics VID
PRODUCT_ID = 0x572B # HIDO PID

# USB Vendor Commands
CMD_CONFIG_READ = 0xC0
CMD_CONFIG_WRITE = 0xC1
CMD_CONFIG_RESET = 0xC2
CMD_GET_VERSION = 0xAA

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
    0x61: 'Num9', 0x62: 'Num0', 0x63: 'Num.',
    0x00: 'NONE'
}

# Reverse mapping
KEY_TO_HID = {v: k for k, v in HID_KEYS.items()}

# Joystick functions (matching firmware enum)
JOY_FUNCTIONS = {
    0: 'Button 1', 1: 'Button 2', 2: 'Button 3', 3: 'Button 4',
    4: 'Button 5', 5: 'Button 6', 6: 'Button 7', 7: 'Button 8',
    8: 'Button 9', 9: 'Button 10', 10: 'Button 11', 11: 'Button 12',
    12: 'Button 13', 13: 'Button 14',
    14: 'UP', 15: 'DOWN', 16: 'LEFT', 17: 'RIGHT',
    18: 'Disabled'
}

FUNC_TO_JOY = {v: k for k, v in JOY_FUNCTIONS.items()}


class HIDOConfigGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("HIDO Configuration Tool")
        self.root.geometry("1000x700")
        
        self.device = None
        self.config = None
        self.mode = None  # 'keyboard' or 'joystick'
        self.config_data = None  # Raw bytes
        
        self.setup_ui()
        self.update_status("Ready. Click 'Connect' to find device.")
        
    def setup_ui(self):
        """Create the user interface"""
        
        # Top toolbar
        toolbar = tk.Frame(self.root, bd=1, relief=tk.RAISED)
        toolbar.pack(side=tk.TOP, fill=tk.X)
        
        self.btn_connect = tk.Button(toolbar, text="🔌 Connect", command=self.connect_device, width=12)
        self.btn_connect.pack(side=tk.LEFT, padx=2, pady=2)
        
        self.btn_read = tk.Button(toolbar, text="📥 Read Config", command=self.read_config, state=tk.DISABLED, width=12)
        self.btn_read.pack(side=tk.LEFT, padx=2, pady=2)
        
        self.btn_write = tk.Button(toolbar, text="📤 Write Config", command=self.write_config, state=tk.DISABLED, width=12)
        self.btn_write.pack(side=tk.LEFT, padx=2, pady=2)
        
        self.btn_reset = tk.Button(toolbar, text="🔄 Reset to Defaults", command=self.reset_config, state=tk.DISABLED, width=15)
        self.btn_reset.pack(side=tk.LEFT, padx=2, pady=2)
        
        tk.Frame(toolbar, width=20).pack(side=tk.LEFT)  # Spacer
        
        self.btn_export = tk.Button(toolbar, text="💾 Export JSON", command=self.export_json, state=tk.DISABLED, width=12)
        self.btn_export.pack(side=tk.LEFT, padx=2, pady=2)
        
        self.btn_import = tk.Button(toolbar, text="📂 Import JSON", command=self.import_json, state=tk.DISABLED, width=12)
        self.btn_import.pack(side=tk.LEFT, padx=2, pady=2)
        
        # Status bar
        self.status_var = tk.StringVar(value="Ready")
        status_bar = tk.Label(self.root, textvariable=self.status_var, bd=1, relief=tk.SUNKEN, anchor=tk.W)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)
        
        # Main content area with notebook (tabs)
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Player 1 tab
        self.frame_p1 = tk.Frame(self.notebook)
        self.notebook.add(self.frame_p1, text="Player 1")
        self.setup_player_frame(self.frame_p1, 1)
        
        # Player 2 tab
        self.frame_p2 = tk.Frame(self.notebook)
        self.notebook.add(self.frame_p2, text="Player 2")
        self.setup_player_frame(self.frame_p2, 2)
        
        # Info tab
        self.frame_info = tk.Frame(self.notebook)
        self.notebook.add(self.frame_info, text="Device Info")
        self.setup_info_frame(self.frame_info)
        
    def setup_player_frame(self, parent, player_num):
        """Create configuration table for a player"""
        
        # Create scrollable frame
        canvas = tk.Canvas(parent)
        scrollbar = tk.Scrollbar(parent, orient="vertical", command=canvas.yview)
        scrollable_frame = tk.Frame(canvas)
        
        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        
        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        # Header
        headers = ["Pin #", "Silkscreen", "Function/Key", "Action"]
        for col, header in enumerate(headers):
            label = tk.Label(scrollable_frame, text=header, font=('Arial', 10, 'bold'), 
                           bg='#E0E0E0', relief=tk.RAISED, padx=10, pady=5)
            label.grid(row=0, column=col, sticky='ew', padx=1, pady=1)
        
        # Create entry widgets for each pin
        entries = []
        for pin in range(MAX_PINS):
            # Pin number
            tk.Label(scrollable_frame, text=str(pin), width=8, 
                    relief=tk.GROOVE).grid(row=pin+1, column=0, padx=1, pady=1)
            
            # Silkscreen (read-only)
            silk_entry = tk.Entry(scrollable_frame, width=12, state='readonly')
            silk_entry.grid(row=pin+1, column=1, padx=1, pady=1)
            
            # Function/Key (dropdown)
            func_var = tk.StringVar()
            func_combo = ttk.Combobox(scrollable_frame, textvariable=func_var, width=20, state='readonly')
            func_combo.grid(row=pin+1, column=2, padx=1, pady=1)
            
            # Edit button
            btn = tk.Button(scrollable_frame, text="Edit", width=8,
                          command=lambda p=pin, pn=player_num: self.edit_mapping(p, pn))
            btn.grid(row=pin+1, column=3, padx=1, pady=1)
            
            entries.append({
                'silk': silk_entry,
                'func': func_combo,
                'func_var': func_var,
                'button': btn
            })
        
        # Store references
        if player_num == 1:
            self.p1_entries = entries
        else:
            self.p2_entries = entries
        
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
        
    def setup_info_frame(self, parent):
        """Create device info display"""
        
        info_text = tk.Text(parent, wrap=tk.WORD, font=('Courier', 10))
        info_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        scrollbar = tk.Scrollbar(info_text, command=info_text.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        info_text.config(yscrollcommand=scrollbar.set)
        
        self.info_text = info_text
        self.info_text.insert('1.0', "No device connected.\n\nConnect a device to view information.")
        self.info_text.config(state=tk.DISABLED)
        
    def update_status(self, message):
        """Update status bar"""
        self.status_var.set(message)
        self.root.update_idletasks()
        
    def connect_device(self):
        """Find and connect to HIDO device"""
        self.update_status("Searching for device...")
        
        try:
            dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
            if dev is None:
                messagebox.showerror("Device Not Found", 
                    f"HIDO device not found!\n\n"
                    f"Looking for VID:PID = {VENDOR_ID:04X}:{PRODUCT_ID:04X}\n\n"
                    f"Make sure:\n"
                    f"  • Device is connected via USB\n"
                    f"  • Firmware is flashed\n"
                    f"  • Device shows up in Device Manager")
                self.update_status("Device not found")
                return
            
            # Detach kernel driver if necessary (Linux only)
            if sys.platform.startswith('linux'):
                try:
                    if dev.is_kernel_driver_active(0):
                        dev.detach_kernel_driver(0)
                except:
                    pass
            
            self.device = dev
            
            # Get firmware version
            try:
                version_data = dev.ctrl_transfer(
                    bmRequestType=0xC0,
                    bRequest=CMD_GET_VERSION,
                    wValue=0,
                    wIndex=0,
                    data_or_wLength=3
                )
                fw_version = f"{version_data[0]}.{version_data[1]}.{version_data[2]}"
            except:
                fw_version = "Unknown"
            
            # Update info tab
            self.info_text.config(state=tk.NORMAL)
            self.info_text.delete('1.0', tk.END)
            info = f"""
Device Connected Successfully!

Manufacturer: {dev.manufacturer if dev.manufacturer else 'N/A'}
Product: {dev.product if dev.product else 'HIDO'}
Serial Number: {dev.serial_number if dev.serial_number else 'N/A'}

USB Information:
  Vendor ID: 0x{dev.idVendor:04X}
  Product ID: 0x{dev.idProduct:04X}
  Firmware Version: {fw_version}

Configuration:
  Flash Address: 0x0801F800
  Flash Size: 2048 bytes
  Magic: 0x{CONFIG_MAGIC:08X} ("HID0")
  Version: {CONFIG_VERSION}
"""
            self.info_text.insert('1.0', info)
            self.info_text.config(state=tk.DISABLED)
            
            # Enable buttons
            self.btn_read.config(state=tk.NORMAL)
            self.btn_reset.config(state=tk.NORMAL)
            self.btn_connect.config(text="✓ Connected", state=tk.DISABLED)
            
            self.update_status(f"Connected: {dev.product if dev.product else 'HIDO'} - FW v{fw_version}")
            messagebox.showinfo("Success", f"Device connected!\nFirmware: v{fw_version}")
            
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect:\n{e}")
            self.update_status(f"Connection failed: {e}")
            
    def read_config(self):
        """Read configuration from device"""
        if not self.device:
            return
            
        self.update_status("Reading configuration...")
        
        try:
            # Send control transfer to read config
            data = self.device.ctrl_transfer(
                bmRequestType=0xC0,
                bRequest=CMD_CONFIG_READ,
                wValue=0,
                wIndex=0,
                data_or_wLength=1024
            )
            
            self.config_data = bytes(data)
            
            # Parse header
            magic, version = struct.unpack('<II', self.config_data[0:8])
            
            if magic != CONFIG_MAGIC:
                messagebox.showerror("Invalid Config", 
                    f"Invalid magic number: 0x{magic:08X}\nExpected: 0x{CONFIG_MAGIC:08X}")
                self.update_status("Read failed: Invalid magic")
                return
            
            # Try to detect mode (check if data looks like keyboard or joystick)
            # Simple heuristic: keyboard has HID keycodes (0x04-0x65 typically)
            # joystick has function IDs (0-18)
            offset = 9  # Skip magic + version, look at first mapping's hid_keycode/joy_function
            test_value = self.config_data[offset]
            
            if test_value <= 18:
                self.mode = 'joystick'
                self.config = self.parse_joystick_config(self.config_data)
            else:
                self.mode = 'keyboard'
                self.config = self.parse_keyboard_config(self.config_data)
            
            self.populate_ui()
            
            self.btn_write.config(state=tk.NORMAL)
            self.btn_export.config(state=tk.NORMAL)
            self.btn_import.config(state=tk.NORMAL)
            
            self.update_status(f"Configuration loaded ({self.mode} mode) - {len(data)} bytes")
            messagebox.showinfo("Success", f"Configuration read successfully!\n\nMode: {self.mode.upper()}\nSize: {len(data)} bytes")
            
        except usb.core.USBError as e:
            messagebox.showerror("Read Error", f"Failed to read configuration:\n{e}")
            self.update_status(f"Read failed: {e}")
            
    def parse_keyboard_config(self, data):
        """Parse keyboard mode configuration"""
        config = {
            'magic': struct.unpack('<I', data[0:4])[0],
            'version': struct.unpack('<I', data[4:8])[0],
            'player1': [],
            'player2': []
        }
        
        offset = 8
        
        # Parse Player 1
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
        
        # Parse Player 2
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
        
    def parse_joystick_config(self, data):
        """Parse joystick mode configuration"""
        config = {
            'magic': struct.unpack('<I', data[0:4])[0],
            'version': struct.unpack('<I', data[4:8])[0],
            'player1': [],
            'player2': []
        }
        
        offset = 8
        
        # Parse Player 1
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
        
        # Parse Player 2
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
        
    def populate_ui(self):
        """Populate UI with loaded configuration"""
        if not self.config:
            return
        
        # Update Player 1
        for i, mapping in enumerate(self.config['player1']):
            entries = self.p1_entries[i]
            
            # Silkscreen
            entries['silk'].config(state='normal')
            entries['silk'].delete(0, tk.END)
            entries['silk'].insert(0, str(mapping['silk_pin']))
            entries['silk'].config(state='readonly')
            
            # Function/Key
            if self.mode == 'keyboard':
                key_display = HID_KEYS.get(mapping['hid_keycode'], f"0x{mapping['hid_keycode']:02X}")
                entries['func_var'].set(f"{mapping['key_name']} ({key_display})")
                entries['func']['values'] = list(KEY_TO_HID.keys())
            else:
                func_display = JOY_FUNCTIONS.get(mapping['joy_function'], f"Unknown ({mapping['joy_function']})")
                entries['func_var'].set(func_display)
                entries['func']['values'] = list(FUNC_TO_JOY.keys())
        
        # Update Player 2
        for i, mapping in enumerate(self.config['player2']):
            entries = self.p2_entries[i]
            
            # Silkscreen
            entries['silk'].config(state='normal')
            entries['silk'].delete(0, tk.END)
            entries['silk'].insert(0, str(mapping['silk_pin']))
            entries['silk'].config(state='readonly')
            
            # Function/Key
            if self.mode == 'keyboard':
                key_display = HID_KEYS.get(mapping['hid_keycode'], f"0x{mapping['hid_keycode']:02X}")
                entries['func_var'].set(f"{mapping['key_name']} ({key_display})")
                entries['func']['values'] = list(KEY_TO_HID.keys())
            else:
                func_display = JOY_FUNCTIONS.get(mapping['joy_function'], f"Unknown ({mapping['joy_function']})")
                entries['func_var'].set(func_display)
                entries['func']['values'] = list(FUNC_TO_JOY.keys())
                
    def edit_mapping(self, pin, player):
        """Edit a specific pin mapping"""
        if not self.config:
            return
        
        player_key = 'player1' if player == 1 else 'player2'
        mapping = self.config[player_key][pin]
        entries = self.p1_entries[pin] if player == 1 else self.p2_entries[pin]
        
        # Create edit dialog
        dialog = tk.Toplevel(self.root)
        dialog.title(f"Edit Pin {pin} - Player {player}")
        dialog.geometry("400x200")
        dialog.transient(self.root)
        dialog.grab_set()
        
        tk.Label(dialog, text=f"Pin {pin} (Silkscreen: {mapping['silk_pin']})", 
                font=('Arial', 12, 'bold')).pack(pady=10)
        
        frame = tk.Frame(dialog)
        frame.pack(pady=10)
        
        if self.mode == 'keyboard':
            tk.Label(frame, text="HID Key:").grid(row=0, column=0, padx=5, pady=5, sticky='e')
            
            key_var = tk.StringVar(value=HID_KEYS.get(mapping['hid_keycode'], 'NONE'))
            key_combo = ttk.Combobox(frame, textvariable=key_var, values=list(KEY_TO_HID.keys()), 
                                    width=20, state='readonly')
            key_combo.grid(row=0, column=1, padx=5, pady=5)
            
            def save_keyboard():
                selected_key = key_var.get()
                hid_code = KEY_TO_HID.get(selected_key, 0x00)
                mapping['hid_keycode'] = hid_code
                mapping['key_name'] = selected_key
                entries['func_var'].set(f"{selected_key} ({HID_KEYS.get(hid_code, 'NONE')})")
                dialog.destroy()
            
            tk.Button(dialog, text="Save", command=save_keyboard, width=10).pack(pady=10)
            
        else:  # joystick mode
            tk.Label(frame, text="Function:").grid(row=0, column=0, padx=5, pady=5, sticky='e')
            
            func_var = tk.StringVar(value=JOY_FUNCTIONS.get(mapping['joy_function'], 'Disabled'))
            func_combo = ttk.Combobox(frame, textvariable=func_var, values=list(FUNC_TO_JOY.keys()), 
                                     width=20, state='readonly')
            func_combo.grid(row=0, column=1, padx=5, pady=5)
            
            def save_joystick():
                selected_func = func_var.get()
                func_code = FUNC_TO_JOY.get(selected_func, 18)
                mapping['joy_function'] = func_code
                mapping['func_name'] = selected_func
                entries['func_var'].set(selected_func)
                dialog.destroy()
            
            tk.Button(dialog, text="Save", command=save_joystick, width=10).pack(pady=10)
        
    def build_config_binary(self):
        """Build binary configuration data from current config"""
        if not self.config:
            return None
        
        data = bytearray()
        
        # Header
        data.extend(struct.pack('<I', CONFIG_MAGIC))
        data.extend(struct.pack('<I', CONFIG_VERSION))
        
        # Player 1
        for mapping in self.config['player1']:
            data.append(mapping['silk_pin'])
            
            if self.mode == 'keyboard':
                data.append(mapping['hid_keycode'])
                name = mapping['key_name'][:15].encode('utf-8')
            else:
                data.append(mapping['joy_function'])
                name = mapping['func_name'][:15].encode('utf-8')
            
            data.extend(name)
            data.extend(b'\x00' * (16 - len(name)))  # Pad to 16 bytes
        
        # Player 2
        for mapping in self.config['player2']:
            data.append(mapping['silk_pin'])
            
            if self.mode == 'keyboard':
                data.append(mapping['hid_keycode'])
                name = mapping['key_name'][:15].encode('utf-8')
            else:
                data.append(mapping['joy_function'])
                name = mapping['func_name'][:15].encode('utf-8')
            
            data.extend(name)
            data.extend(b'\x00' * (16 - len(name)))  # Pad to 16 bytes
        
        # CRC32 (simplified - just use existing CRC or 0)
        crc = self.config.get('crc32', 0)
        data.extend(struct.pack('<I', crc))
        
        return bytes(data)
        
    def write_config(self):
        """Write configuration to device"""
        if not self.device or not self.config:
            return
        
        if not messagebox.askyesno("Confirm Write", 
            "Write configuration to device?\n\n"
            "This will overwrite the current configuration in flash memory."):
            return
        
        self.update_status("Writing configuration...")
        
        try:
            # Build binary data
            config_data = self.build_config_binary()
            
            # Send control transfer to write config
            result = self.device.ctrl_transfer(
                bmRequestType=0x40,
                bRequest=CMD_CONFIG_WRITE,
                wValue=0,
                wIndex=0,
                data_or_wLength=config_data
            )
            
            self.update_status(f"Configuration written ({len(config_data)} bytes)")
            messagebox.showinfo("Success", f"Configuration written successfully!\n\n{len(config_data)} bytes written to flash.")
            
        except usb.core.USBError as e:
            messagebox.showerror("Write Error", f"Failed to write configuration:\n{e}")
            self.update_status(f"Write failed: {e}")
            
    def reset_config(self):
        """Reset configuration to defaults"""
        if not self.device:
            return
        
        if not messagebox.askyesno("Confirm Reset", 
            "Reset configuration to factory defaults?\n\n"
            "This will restore default key mappings."):
            return
        
        self.update_status("Resetting configuration...")
        
        try:
            self.device.ctrl_transfer(
                bmRequestType=0x40,
                bRequest=CMD_CONFIG_RESET,
                wValue=0,
                wIndex=0,
                data_or_wLength=0
            )
            
            self.update_status("Configuration reset to defaults")
            messagebox.showinfo("Success", "Configuration reset to defaults!\n\nClick 'Read Config' to view.")
            
            # Clear current config
            self.config = None
            self.config_data = None
            
        except usb.core.USBError as e:
            messagebox.showerror("Reset Error", f"Failed to reset configuration:\n{e}")
            self.update_status(f"Reset failed: {e}")
            
    def export_json(self):
        """Export configuration to JSON file"""
        if not self.config:
            return
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")],
            initialfile="hido_config.json"
        )
        
        if filename:
            try:
                with open(filename, 'w') as f:
                    json.dump(self.config, f, indent=2)
                self.update_status(f"Exported to {filename}")
                messagebox.showinfo("Success", f"Configuration exported to:\n{filename}")
            except Exception as e:
                messagebox.showerror("Export Error", f"Failed to export:\n{e}")
                
    def import_json(self):
        """Import configuration from JSON file"""
        filename = filedialog.askopenfilename(
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if filename:
            try:
                with open(filename, 'r') as f:
                    imported_config = json.load(f)
                
                # Validate structure
                if 'player1' not in imported_config or 'player2' not in imported_config:
                    raise ValueError("Invalid config structure: missing player data")
                
                # Detect mode
                if 'hid_keycode' in imported_config['player1'][0]:
                    self.mode = 'keyboard'
                elif 'joy_function' in imported_config['player1'][0]:
                    self.mode = 'joystick'
                else:
                    raise ValueError("Cannot detect config mode")
                
                self.config = imported_config
                self.populate_ui()
                
                self.btn_write.config(state=tk.NORMAL)
                self.btn_export.config(state=tk.NORMAL)
                
                self.update_status(f"Imported from {filename}")
                messagebox.showinfo("Success", f"Configuration imported from:\n{filename}\n\nMode: {self.mode.upper()}")
                
            except Exception as e:
                messagebox.showerror("Import Error", f"Failed to import:\n{e}")


def main():
    """Main entry point"""
    root = tk.Tk()
    app = HIDOConfigGUI(root)
    root.mainloop()


if __name__ == '__main__':
    main()
