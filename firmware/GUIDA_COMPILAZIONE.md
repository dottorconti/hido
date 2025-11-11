# 🔧 GUIDA RAPIDA - Compilazione e Flash

## ✅ Prerequisiti

### 1. ARM GCC Toolchain
Scarica e installa da:
- **Ufficiale**: https://developer.arm.com/downloads/-/gnu-rm
- **Chocolatey**: `choco install gcc-arm-embedded`

Verifica installazione:
```powershell
arm-none-eabi-gcc --version
```

### 2. Make
- **MinGW/MSYS2**: `pacman -S make`
- **Chocolatey**: `choco install make`
- Oppure incluso in Git Bash

## 🚀 Compilazione Rapida

### Opzione 1: Usa lo script PowerShell (RACCOMANDATO)

```powershell
cd c:\temp\hido\firmware
.\build.ps1
```

Lo script automaticamente:
- ✅ Verifica i tool necessari
- ✅ Pulisce la build precedente
- ✅ Compila il progetto
- ✅ Mostra le dimensioni del firmware
- ✅ Genera .elf, .hex e .bin

### Opzione 2: Make manuale

```powershell
cd c:\temp\hido\firmware
make clean
make -j4
```

## 📦 File Generati

Dopo la compilazione trovi in `build/`:

| File | Descrizione | Uso |
|------|-------------|-----|
| `hido.elf` | ELF con simboli di debug | Debugging con GDB |
| `hido.hex` | Intel HEX format | Flash con STM32CubeProgrammer |
| `hido.bin` | Raw binary | Flash con OpenOCD/DFU |

## 📲 Flash sulla Scheda

### Metodo 1: STM32CubeProgrammer (GUI)

1. Scarica: https://www.st.com/en/development-tools/stm32cubeprog.html
2. Collega ST-Link alla scheda
3. Apri STM32CubeProgrammer
4. Connetti: USB/ST-Link, porta corretta
5. "Open file" → `build/hido.hex`
6. Click "Download"
7. Done! ✅

### Metodo 2: OpenOCD (Linea Comando)

```powershell
# Con ST-Link V2
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/hido.elf verify reset exit"
```

### Metodo 3: DFU (via USB senza ST-Link)

Se hai il bootloader DFU:
```powershell
dfu-util -a 0 -s 0x08000000:leave -D build/hido.bin
```

### Metodo 4: Make flash (se configurato)

```powershell
make flash
```

## 🧪 Test Pulsanti

Dopo il flash:

1. **Scollega ST-Link** (se usato)
2. **Collega solo USB** al PC
3. **Windows rileverà**: "USB Input Device" o "HID-compliant game controller"
4. **Apri Game Controller Tester**:
   - Windows: Impostazioni → Dispositivi → Controller USB → Proprietà
   - Web: https://gamepad-tester.com/
5. **Premi i pulsanti**: Devono apparire come premuti
6. **Verifica tutti i pulsanti** dei 2 player

## 🐛 Risoluzione Problemi

### Errore: "make: command not found"
```powershell
# Installa Make
choco install make
# Oppure usa Git Bash incluso con Git per Windows
```

### Errore: "arm-none-eabi-gcc: command not found"
```powershell
# Aggiungi al PATH o installa toolchain
choco install gcc-arm-embedded
```

### Errore: Compilazione fallisce
```powershell
# Pulisci tutto e riprova
Remove-Item -Recurse -Force build
make clean
make -j4
```

### I pulsanti non vengono rilevati
1. ✅ Verifica che hai applicato le correzioni in `gpio.c` (PA0, PA1, PC4)
2. 🧪 Usa il test diagnostico (vedi `GPIO_TEST_GUIDE.md`)
3. 🔌 Verifica hardware: resistenze, saldature, continuità

### Windows non rileva il dispositivo USB
1. Controlla Device Manager per errori
2. Prova un'altra porta USB
3. Verifica che USB D+/D- (PA11/PA12) siano collegati correttamente
4. Riflasha il firmware

## 📊 Dimensioni Firmware Attese

Dopo `arm-none-eabi-size build/hido.elf`:

```
   text    data     bss     dec     hex filename
  15234     112    1824   17170    4312 build/hido.elf
```

Circa:
- **15KB** di codice (text)
- **112B** di dati inizializzati (data)
- **1.8KB** di variabili non inizializzate (bss)

**Totale Flash usata**: ~15KB su 128KB disponibili ✅

## 🎯 Modalità Operative

Il firmware è configurato per **JOYSTICK MODE** di default.

Per cambiare modalità, modifica `Core/Inc/usbd_hid_custom.h`:

```c
/* Mode selection - uncomment only ONE */
//#define USE_KEYBOARD_MODE    // NKRO Keyboard mode
#define USE_JOYSTICK_MODE      // Arcade Joystick mode (ATTUALE)
//#define USE_JVS_MODE         // JVS protocol mode
```

Dopo la modifica, ricompila:
```powershell
.\build.ps1
```

## ✨ Ricompilazione Rapida

Dopo modifiche al codice:
```powershell
# Compilazione incrementale (solo file modificati)
make

# O ricompilazione completa
.\build.ps1
```

## 📚 Riferimenti

- **Correzioni applicate**: `CORREZIONI.md`
- **Test GPIO**: `GPIO_TEST_GUIDE.md`
- **Verifica pin**: `PIN_MAPPING_CHECK.md`
- **Riepilogo**: `RIEPILOGO_FINALE.md`

## 🎉 Tutto Pronto!

Ora hai:
- ✅ Codice corretto con PA0, PA1, PC4 configurati
- ✅ Script di build automatico
- ✅ Tool diagnostico incluso
- ✅ Documentazione completa

**Compila, flasha, e divertiti con il tuo controller arcade!** 🎮
