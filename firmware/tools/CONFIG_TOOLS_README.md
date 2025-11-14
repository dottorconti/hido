# HIDO Configuration Tools

Strumenti per configurare il mapping dei pin via USB.

## 📋 Requisiti

```powershell
pip install pyusb
```

### Windows: Driver USB
Su Windows, serve installare il driver libusb con **Zadig**:
1. Scarica [Zadig](https://zadig.akeo.ie/)
2. Collega il dispositivo HIDO
3. In Zadig: Options → List All Devices
4. Seleziona "HIDO" (VID 0483, PID 572B)
5. Installa driver "WinUSB" o "libusb-win32"

## 🖥️ Tool Grafico (Raccomandato)

### `config_tool_gui.py`

Interfaccia grafica completa per gestire la configurazione.

```powershell
python config_tool_gui.py
```

### Funzionalità:
- ✅ **Connect**: Rileva automaticamente il dispositivo HIDO
- ✅ **Read Config**: Legge configurazione corrente dalla flash
- ✅ **Write Config**: Scrive configurazione modificata
- ✅ **Reset to Defaults**: Ripristina mapping di fabbrica
- ✅ **Export/Import JSON**: Salva/carica configurazioni
- ✅ **Edit Mapping**: Modifica mapping pin per pin
- ✅ **Device Info**: Visualizza info firmware e dispositivo

### Screenshot Funzionalità:

**Tab Player 1/2:**
- Tabella con 17 pin per giocatore
- Colonne: Pin#, Silkscreen, Function/Key, Action
- Dropdown per selezionare rapidamente tasti/funzioni
- Bottone "Edit" per modifica dettagliata

**Tab Device Info:**
- Manufacturer, Product, Serial Number
- Versione firmware
- Indirizzo flash e dimensioni
- Magic number e versione config

### Modalità Supportate:

#### Keyboard Mode
Mapping pin → HID keycode:
- A-Z, 0-9
- ENTER, ESC, SPACE, TAB
- Frecce direzionali
- Tastierino numerico
- Simboli (- = [ ] / * + .)

#### Joystick Mode
Mapping pin → Funzione joystick:
- Button 1-14
- UP, DOWN, LEFT, RIGHT
- Disabled (pin non usato)

## 🔧 Tool da Linea di Comando

### `config_tool.py`

Versione CLI per automazione e scripting.

```powershell
python config_tool.py
```

### Comandi Disponibili:
- **[R]** Reset to defaults
- **[E]** Export to JSON
- **[I]** Import from JSON (limitato)
- **[Q]** Quit

### Esempio Output:

```
==================================================================
KEYBOARD MODE CONFIGURATION
==================================================================

Player 1:
--------------------------------------------------
Pin    Silkscreen   Key      HID Code
--------------------------------------------------
0      0            W        W (0x1A)
1      1            A        A (0x04)
2      2            S        S (0x16)
3      3            D        D (0x07)
...

CRC32: 0x12345678
==================================================================
```

## 📡 Protocollo USB

### Comandi Vendor-Specific

| Comando | bRequest | Descrizione |
|---------|----------|-------------|
| `CONFIG_READ` | 0xC0 | Legge configurazione (max 1024 byte) |
| `CONFIG_WRITE` | 0xC1 | Scrive configurazione |
| `CONFIG_RESET` | 0xC2 | Reset a default |
| `GET_VERSION` | 0xAA | Versione firmware (3 byte) |
| `RESET_DEVICE` | 0xCC | Soft reset dispositivo |
| `ENTER_BOOTLOADER` | 0xBB | Entra in DFU (magic 0xB007) |

### Struttura Configurazione

#### Keyboard Mode (620 byte)
```c
struct KeyboardConfig {
    uint32_t magic;                    // 0x48494430 ("HID0")
    uint32_t version;                  // 1
    KeyboardMapping player1[17];       // 17 × 18 = 306 byte
    KeyboardMapping player2[17];       // 17 × 18 = 306 byte
    uint32_t crc32;                    // CRC32 checksum
};

struct KeyboardMapping {
    uint8_t silk_pin;       // Pin silkscreen (0-16)
    uint8_t hid_keycode;    // USB HID keycode
    char key_name[16];      // Nome leggibile (es. "W", "SPACE")
};
```

#### Joystick Mode (620 byte)
```c
struct JoystickConfig {
    uint32_t magic;                    // 0x48494430 ("HID0")
    uint32_t version;                  // 1
    JoystickMapping player1[17];       // 17 × 18 = 306 byte
    JoystickMapping player2[17];       // 17 × 18 = 306 byte
    uint32_t crc32;                    // CRC32 checksum
};

struct JoystickMapping {
    uint8_t silk_pin;           // Pin silkscreen (0-16)
    uint8_t joy_function;       // JoystickFunction_t (0-18)
    char func_name[16];         // Nome funzione (es. "Button 1", "UP")
};
```

### Funzioni Joystick (enum)

| Valore | Funzione |
|--------|----------|
| 0-13 | Button 1-14 |
| 14 | UP |
| 15 | DOWN |
| 16 | LEFT |
| 17 | RIGHT |
| 18 | Disabled |

## 💾 Storage Flash

- **Indirizzo**: `0x0801F800` (ultima pagina da 2KB)
- **Dimensione**: 2048 byte (solo primi 620 usati)
- **Validazione**: Magic number + CRC32
- **Persistenza**: Conservato dopo reset/power cycle

### Workflow Salvataggio:
1. Firmware carica config da flash al boot
2. Se CRC32 invalido → carica default
3. Config modificabile via USB
4. Write command → Erase page + Program + Calculate CRC32

## 🎮 Mappature Default

### Keyboard Mode - Player 1
```
Pin 0  → W          (Su)
Pin 1  → A          (Sinistra)
Pin 2  → S          (Giù)
Pin 3  → D          (Destra)
Pin 4  → Q          (Button 1)
Pin 5  → X          (Button 2)
Pin 6  → C          (Button 3)
Pin 7  → V          (Button 4)
Pin 8-16 → 1-9      (Button 5-13)
```

### Keyboard Mode - Player 2
```
Pin 0  → UP         (Freccia Su)
Pin 1  → LEFT       (Freccia Sinistra)
Pin 2  → DOWN       (Freccia Giù)
Pin 3  → RIGHT      (Freccia Destra)
Pin 4-16 → Numpad   (Num1-Num9)
```

### Joystick Mode - Player 1 & 2
```
Pin 0  → UP
Pin 1  → DOWN
Pin 2  → LEFT
Pin 3  → RIGHT
Pin 4-16 → Button 1-13
```

## 🐛 Troubleshooting

### Device Not Found
- Verifica connessione USB
- Controlla Device Manager (deve apparire come "HIDO")
- Installa driver con Zadig (Windows)
- Su Linux: aggiungi regola udev o usa `sudo`

### Read/Write Failed
- Verifica firmware flashato correttamente
- Controlla che non sia in DFU mode (PID 0xDF11)
- Riconnetti dispositivo
- Prova porta USB diversa

### Config Invalid
- Magic number errato → flash corrotta
- CRC32 mismatch → dati danneggiati
- Usa "Reset to Defaults" per ripristinare

### Import JSON Failed
- Verifica struttura JSON corretta
- Controlla che il mode (keyboard/joystick) corrisponda al firmware
- Assicurati che ci siano esattamente 17 pin per player

## 📝 Esempi

### Esporta Config Corrente
```powershell
python config_tool.py
# Seleziona [E] Export
# Salva come: my_config.json
```

### Modifica e Carica Config
```powershell
python config_tool_gui.py
# 1. Connect
# 2. Read Config
# 3. Edit mapping (click Edit sui pin)
# 4. Write Config
```

### Reset dopo Errore
```powershell
python config_tool.py
# Seleziona [R] Reset
# Conferma con "yes"
```

## 🔗 Link Utili

- [USB HID Usage Tables](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf) - HID keycodes
- [PyUSB Documentation](https://github.com/pyusb/pyusb) - Libreria Python USB
- [Zadig](https://zadig.akeo.ie/) - Driver installer per Windows

## 📄 License

Vedi file LICENSE nel repository principale.
