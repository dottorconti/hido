# HIDO Tools

Questa cartella contiene tutti gli strumenti necessari per l'aggiornamento firmware e la configurazione di HIDO.

## Contenuto

### DFU (Device Firmware Update)
- **`dfu-util.exe`** - Tool per flash firmware via USB DFU (v0.11 static)
- **`dfu_update.py`** - Script Python per aggiornamento automatico firmware
- **`dfu_update.bat`** - Script Windows per aggiornamento manuale firmware

### Configurazione
- **`config_tool.py`** - Tool Python per gestione configurazione pin mapping
- **`libusb-1.0.dll`** - Libreria USB necessaria per pyusb su Windows

## Utilizzo

### 1. Aggiornamento Firmware (DFU)

#### Metodo Automatico (Python)
```bash
# Installa pyusb se non presente
pip install pyusb

# Esegui script automatico (entra in DFU e flasha firmware)
python dfu_update.py
```

#### Metodo Manuale (Batch)
```bash
# Esegui script manuale
.\dfu_update.bat
```

Lo script:
1. Rileva il dispositivo HIDO
2. Invia comando USB per entrare in modalità DFU
3. Attende 3 secondi
4. Flasha il firmware con dfu-util
5. Resetta il dispositivo

### 2. Configurazione Pin Mapping

#### Prerequisiti Windows
1. **Installa WinUSB driver** con Zadig:
   - Scarica Zadig: https://zadig.akeo.ie/
   - Options → "List All Devices"
   - Seleziona "HIDO Arcade Keyboard"
   - Driver → "WinUSB"
   - Install/Replace Driver

2. **Installa pyusb**:
   ```bash
   pip install pyusb
   ```

3. **Assicurati che `libusb-1.0.dll` sia in questa cartella**

#### Utilizzo Config Tool
```bash
python config_tool.py
```

Il tool permette di:
- **Leggere** la configurazione attuale dal dispositivo
- **Visualizzare** la mappatura pin per Player 1 e Player 2
- **Resettare** ai valori di default
- **Esportare** la configurazione in JSON

#### Comandi USB Supportati
- `0xC0` - Leggi configurazione (512 byte)
- `0xC1` - Scrivi configurazione (salva in FLASH)
- `0xC2` - Reset configurazione ai default

### 3. Configurazione Predefinita

#### Modalità Keyboard
**Player 1 (WASD)**:
- Pin 0-3: A, S, W, D (movimento)
- Pin 4-7: Q, X, C, V (azioni)
- Pin 8-15: 1, 2, 3, 4, 5, 6, 7, 8 (tasti numerici)
- Pin 16: 9

**Player 2 (Frecce + Numpad)**:
- Pin 0-3: LEFT, DOWN, UP, RIGHT
- Pin 4-15: Numpad 1-8, 9, 0, ., /
- Pin 16: Numpad *

#### Modalità Joystick
- Pin 0-3: LEFT, DOWN, UP, RIGHT
- Pin 4-16: Button 1-13

## Persistenza
La configurazione viene salvata nella **FLASH** (ultima pagina da 2KB) con:
- **Validazione CRC32** per integrità dati
- **10.000+ cicli di scrittura** garantiti
- **Caricamento automatico** all'avvio con fallback ai default

## Note Tecniche

### Struttura Configurazione
```c
typedef struct {
    uint32_t magic;      // 0x48494430 ("HID0")
    uint32_t version;    // 1
    KeyboardMapping_t player1[17];  // 17 pin per player
    KeyboardMapping_t player2[17];
    uint32_t crc32;      // Checksum validazione
} KeyboardConfig_t;
```

### Indirizzo FLASH
- **Base**: `0x0801F800` (ultima pagina 2KB)
- **Dimensione**: 2048 byte (0x800)

### VID:PID
- **Vendor ID**: `0x0483` (STMicroelectronics)
- **Product ID**: `0x572B` (HIDO)

## Troubleshooting

### pyusb non trova il dispositivo
1. Verifica che WinUSB driver sia installato (Device Manager → "Universal Serial Bus devices")
2. Verifica che `libusb-1.0.dll` sia nella stessa cartella dello script
3. Riavvia il dispositivo (scollega/ricollega USB)

### dfu-util non trova il dispositivo
1. Verifica che il dispositivo sia in modalità DFU (LED dovrebbe lampeggiare)
2. Controlla Device Manager: cerca "STM32 BOOTLOADER"
3. Prova a tenere premuto BOOT0 durante reset

### Configurazione non viene salvata
1. Verifica che il firmware sia aggiornato (versione con supporto FLASH)
2. Controlla che la FLASH non sia protetta da scrittura
3. Usa comando 0xC2 per reset e riprova

## Link Utili
- **pyusb**: https://github.com/pyusb/pyusb
- **libusb**: https://libusb.info/
- **dfu-util**: https://dfu-util.sourceforge.net/
- **Zadig**: https://zadig.akeo.ie/
