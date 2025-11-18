# Correzioni Hardware RS485 - Protezione 5V

## ⚠️ Problema Identificato

L'attuale design alimenta il transceiver RS485 (SN65HVD1786D) a **+5V** con resistenze di pullup a **+5V** sui pin STM32F102. Questo è **pericoloso** perché:

- STM32F102 **NON ha pin 5V tolerant** (solo pin FT sono tolerant)
- PA8, PA9, PA10 sono pin standard: **Vmax = VDD + 0.3V = 3.6V**
- Pullup a 5V possono **danneggiare il microcontrollore**

## ✅ Modifiche Hardware Richieste

### 1. Alimentazione RS485 (U3)
**Componente**: U3 (SN65HVD1786D) pin 8 (VCC)
- ❌ **Rimuovi**: Connessione a +5V
- ✅ **Collega**: A VDD (3.3V)
- 📝 **Nello schematico**: Cambia simbolo `#PWR0110` da `+5V` a `+3.3V`

**Nota**: SN65HVD1786D supporta VCC 3.0-5.5V, funziona perfettamente a 3.3V

### 2. Resistenze Pullup
**Componenti**: R37, R38, R42 (10kΩ) pin 2
- ❌ **Rimuovi**: Connessione a +5V
- ✅ **Collega**: A VDD (3.3V)

**Funzione resistenze**:
- **R37** (10kΩ): Pullup PA9 (USART1_TX → RS485 DI)
- **R38** (10kΩ): Pullup PA10 (USART1_RX → RS485 RO)
- **R42** (10kΩ): Pullup PA8 (DE/RE Enable)

### 3. Resistenze da NON montare
**Componenti**:
- **R34** (0Ω): Da USB VBUS a +5V rail → ❌ **NON MONTARE**
- **R36** (0Ω): Connettore JVS1 VBUS → ❌ **NON MONTARE**

**Motivo**: Non serve rail +5V, tutto funziona a 3.3V

### 4. JVS Sense Line - Modifica Pin
**Cambio di assegnazione pin**:
- ❌ **Vecchio**: PA10 (conflitto con USART1_RX)
- ✅ **Nuovo**: PA2 (libero, era USART2_TX debug)

**Nello schematico**:
- Collega il pin JVS Sense dal connettore JVS1 a **PA2** invece di PA10
- PA2 sarà configurato come GPIO Output nel firmware

## 📋 Checklist Modifiche Schematico

### rs485.kicad_sch
- [ ] U3 pin 8 (VCC): Cambia simbolo power da `+5V` a `+3.3V`
- [ ] R37 pin 2: Cambia net da `+5V` a `VDD`
- [ ] R38 pin 2: Cambia net da `+5V` a `VDD`
- [ ] R42 pin 2: Cambia net da `+5V` a `VDD`

### Jopen.kicad_sch (schema principale)
- [ ] R34: Aggiungi attributo `DNP` (Do Not Place) o rimuovi
- [ ] R36: Aggiungi attributo `DNP` (Do Not Place) o rimuovi
- [ ] JVS1 Sense: Cambia connessione da PA10 a PA2

### mcu.kicad_sch
- [ ] PA2: Verifica che sia libero (rimuovi USART2_TX se presente)
- [ ] PA2: Aggiungi label "JVS_SENSE" o "SENSE_LINE"

## 🔌 Pinout Finale RS485/JVS

| Pin STM32 | Funzione | Collegamento Hardware | Note |
|-----------|----------|----------------------|------|
| PA9 | USART1_TX | U3 pin 4 (DI) | Trasmissione RS485 |
| PA10 | USART1_RX | U3 pin 1 (RO) | Ricezione RS485 |
| PA8 | GPIO Output | U3 pin 2,3 (DE/RE) | Enable transceiver |
| PA2 | GPIO Output | JVS1 Sense pin | Sense Line (Floating/HIGH) |

**Resistenze pullup a 3.3V**:
- R37: PA9 → VDD (3.3V)
- R38: PA10 → VDD (3.3V)
- R42: PA8 → VDD (3.3V)

## 🔧 Note Implementazione PCB

### Se PCB già prodotta
**Workaround manuale**:
1. NON montare R34, R36
2. Collegare manualmente U3 pin 8 a VDD (3.3V) con wire
3. Tagliare tracce R37/R38/R42 pin 2 da +5V
4. Collegare R37/R38/R42 pin 2 a VDD (3.3V) con wire/jumper
5. Spostare connessione JVS Sense da PA10 a PA2 con wire

### Per nuova revisione PCB
- Implementare tutte le modifiche nello schematico
- Verificare DRC e ERC prima di produzione
- Testare prototipo con multimetro:
  - U3 pin 8 = 3.3V ✓
  - PA8/PA9/PA10 con pullup = 3.3V ✓
  - PA2 = Floating o 3.3V (dipende da stato firmware) ✓

## 📚 Reference

**STM32F102RBT6 Datasheet**:
- Pin PA8, PA9, PA10: NON FT (Five-volt Tolerant)
- Absolute Max Voltage: VDD + 0.3V = 3.6V
- Typical VDD: 3.3V

**SN65HVD1786D Datasheet**:
- VCC Range: 3.0V to 5.5V
- Logic Input Compatible: 3.3V and 5V systems
- Output HIGH @ 3.3V: ~2.6V min (sufficiente per RS485)

## ✅ Verifica Post-Modifica

Dopo implementazione modifiche, verificare con multimetro:
1. ✅ U3 pin 8 (VCC) = 3.3V
2. ✅ PA9 idle (HIGH) = 3.3V
3. ✅ PA10 idle = 3.3V
4. ✅ PA8 idle = 3.3V
5. ✅ PA2 = Floating o 3.3V (controllabile da firmware)
6. ❌ Nessun pin STM32 > 3.6V

**Test funzionale**:
- RS485 comunica correttamente a 115200 baud
- JVS Sense Line controllabile (Floating → HIGH)
- Nessun surriscaldamento STM32 o U3

---

**Data creazione**: 13 Novembre 2025  
**Autore**: GitHub Copilot + dottorconti  
**Versione hardware**: Rev. 1.x → Rev. 2.0 (con fix 5V)
