# 🚀 HIDO Configuration - Quick Start

**5 minuti per configurare HIDO!**

## ✅ Step 1: Prerequisiti (Una tantum)

### Windows

1. **Installa Python 3.x**
   - Download: https://www.python.org/downloads/
   - ☑️ Check "Add Python to PATH" durante installazione

2. **Installa PyUSB**
   ```powershell
   pip install pyusb
   ```

3. **Installa Driver USB** (solo prima volta)
   - Collega HIDO via USB
   - Esegui `zadig.exe` (in questa cartella)
   - Options → "List All Devices"
   - Seleziona "HIDO" (VID 0483, PID 572B)
   - Scegli driver "WinUSB"
   - Click "Install Driver" o "Replace Driver"
   - Attendi completamento (~30 sec)

### Linux

```bash
sudo apt install python3 python3-pip libusb-1.0-0
pip3 install pyusb

# Optional: avoid sudo requirement
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="572b", MODE="0666"' | sudo tee /etc/udev/rules.d/99-hido.rules
sudo udevadm control --reload-rules
```

## 🎮 Step 2: Prima Configurazione

### Metodo A: GUI (Facile)

1. **Doppio click** su `run_config_gui.bat`

2. Click **"🔌 Connect"**
   - Dovrebbe trovare automaticamente il dispositivo
   - Mostra info firmware in tab "Device Info"

3. Click **"📥 Read Config"**
   - Carica configurazione corrente
   - Mostra Player 1 e Player 2 in tab separati

4. **Modifica pin** (opzionale)
   - Vai su tab "Player 1" o "Player 2"
   - Click "Edit" sul pin da modificare
   - Scegli nuovo tasto/funzione dal dropdown
   - Click "Save"

5. Click **"📤 Write Config"**
   - Conferma scrittura
   - Attendere "Configuration written successfully!"

6. **Fatto!** 🎉
   - Config salvata in flash
   - Persistente dopo reboot

### Metodo B: CLI (Avanzato)

```powershell
python config_tool.py
```

- Mostra config corrente
- [R] Reset to defaults
- [E] Export JSON
- [Q] Quit

## 🔍 Step 3: Verifica (Opzionale)

```powershell
python test_system.py
```

Controlla:
- ✓ PyUSB installato
- ✓ libusb backend funzionante
- ✓ HIDO rilevato
- ✓ Versione firmware

## 💾 Mappature Default

### Keyboard Mode

**Player 1 (WASD Style)**
```
Pin 0 → W        (Su)
Pin 1 → A        (Sinistra)
Pin 2 → S        (Giù)
Pin 3 → D        (Destra)
Pin 4 → Q        (Button 1)
Pin 5 → X        (Button 2)
Pin 6 → C        (Button 3)
Pin 7 → V        (Button 4)
Pin 8-16 → 1-9   (Button 5-13)
```

**Player 2 (Arrows + Numpad)**
```
Pin 0 → UP       (Freccia Su)
Pin 1 → LEFT     (Freccia Sinistra)
Pin 2 → DOWN     (Freccia Giù)
Pin 3 → RIGHT    (Freccia Destra)
Pin 4-16 → Num1-Num9  (Numpad)
```

### Joystick Mode
```
Pin 0-3 → UP/DOWN/LEFT/RIGHT
Pin 4-16 → Button 1-13
```

## ❓ Problemi Comuni

### "Device Not Found"
- ✅ Verifica cavo USB collegato
- ✅ Check Device Manager (Windows) / lsusb (Linux)
- ✅ Reinstalla driver con Zadig
- ✅ Prova altra porta USB

### "PyUSB Not Installed"
```powershell
pip install pyusb
```

### "Backend Not Found" (Windows)
- ✅ Controlla che `libusb-1.0.dll` sia in questa cartella
- ✅ Scarica da: https://libusb.info/ se mancante

### "Permission Denied" (Linux)
```bash
sudo python3 config_tool_gui.py
# OR: add udev rules (see Step 1)
```

### Config non si salva
- ✅ Dopo "Write Config" attendi messaggio di conferma
- ✅ Non scollegare durante scrittura
- ✅ Se persiste: click "Reset to Defaults" poi riprova

## 📚 Documentazione Completa

- **CONFIG_TOOLS_README.md** - Guida dettagliata tools
- **IMPLEMENTATION_SUMMARY.md** - Dettagli tecnici implementazione
- **README.md** - Overview generale tools

## 🆘 Supporto

**GitHub Issues**: https://github.com/dottorconti/hido/issues

**Info rapide**:
- VID: 0x0483 (STMicroelectronics)
- PID: 0x572B (HIDO)
- Flash: 0x0801F800 (2KB page)
- Config size: 620 bytes

---

**Ready to play! 🎮**
