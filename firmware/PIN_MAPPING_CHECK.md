# Verifica Mappatura Pin GPIO

## Pin Definiti in main.h vs Configurati in gpio.c

### PORTA A (GPIOA)

| Pin | Nome Segnale | Definito in main.h | Configurato in gpio.c | Tipo Config | Note |
|-----|--------------|--------------------|-----------------------|-------------|------|
| PA0 | P1_DOWN | ✅ | ✅ (CORRETTO) | INPUT NOPULL | **CORRETTO ORA** |
| PA1 | P1_UP | ✅ | ✅ (CORRETTO) | INPUT NOPULL | **CORRETTO ORA** |
| PA4 | (ADC1) | - | ✅ | ANALOG | Per ADC |
| PA5 | (ADC2) | - | ✅ | ANALOG | Per ADC |
| PA6 | P2_START | ✅ | ✅ | INPUT NOPULL | OK |
| PA7 | P2_BTN1 | ✅ | ✅ | INPUT NOPULL | OK |
| PA8 | - | - | ✅ | OUTPUT | Uscita generica |
| PA15 | P1_START | ✅ | ✅ | INPUT NOPULL | OK |
| PA11/PA12 | USB D-/D+ | - | (USB periph) | - | Gestito da USB |

### PORTA B (GPIOB)

| Pin | Nome Segnale | Definito in main.h | Configurato in gpio.c | Tipo Config | Note |
|-----|--------------|--------------------|-----------------------|-------------|------|
| PB0 | P2_AXIS_X | ✅ | ❓ | - | **ANALOG/ADC?** |
| PB1 | P2_AXIS_Y | ✅ | ❓ | - | **ANALOG/ADC?** |
| PB2 | P2_BTN4 | ✅ | ✅ | INPUT NOPULL | OK |
| PB3 | P1_BTN12 | ✅ | ✅ | INPUT NOPULL | OK |
| PB4 | P1_BTN11 | ✅ | ✅ | INPUT NOPULL | OK |
| PB5 | P1_BTN10 | ✅ | ✅ | INPUT NOPULL | OK |
| PB6 | P1_BTN9 | ✅ | ✅ | INPUT NOPULL | OK |
| PB7 | P1_BTN8 | ✅ | ✅ | INPUT NOPULL | OK |
| PB8 | P1_BTN7 | ✅ | ✅ | INPUT NOPULL | OK |
| PB9 | P1_BTN6 | ✅ | ✅ | INPUT NOPULL | OK |
| PB10 | P2_BTN5 | ✅ | ✅ | INPUT NOPULL | OK |
| PB11 | P2_BTN6 | ✅ | ✅ | INPUT NOPULL | OK |
| PB12 | P2_BTN7 | ✅ | ✅ | INPUT NOPULL | OK |
| PB13 | P2_BTN8 | ✅ | ✅ | INPUT NOPULL | OK |
| PB14 | P2_BTN9 | ✅ | ✅ | INPUT NOPULL | OK |
| PB15 | P2_BTN10 | ✅ | ✅ | INPUT NOPULL | OK |

### PORTA C (GPIOC)

| Pin | Nome Segnale | Definito in main.h | Configurato in gpio.c | Tipo Config | Note |
|-----|--------------|--------------------|-----------------------|-------------|------|
| PC0 | P1_BTN2 | ✅ | ✅ | INPUT NOPULL | OK |
| PC1 | P1_BTN1 | ✅ | ✅ | INPUT NOPULL | OK |
| PC2 | P1_AXIS_Y | ✅ | ❓ | - | **ANALOG/ADC?** |
| PC3 | P1_AXIS_X | ✅ | ❓ | - | **ANALOG/ADC?** |
| PC4 | P2_BTN2 | ✅ | ✅ | OUTPUT (LED?) | **ATTENZIONE** |
| PC5 | P2_BTN3 | ✅ | ✅ | INPUT NOPULL | OK |
| PC6 | P2_UP | ✅ | ✅ | INPUT NOPULL | OK |
| PC7 | P2_DOWN | ✅ | ✅ | INPUT NOPULL | OK |
| PC8 | P2_BTN11 | ✅ | ✅ | INPUT NOPULL | OK |
| PC9 | P2_BTN12 | ✅ | ✅ | INPUT NOPULL | OK |
| PC10 | LED1 | ✅ | ✅ | OUTPUT | LED |
| PC11 | LED2 | ✅ | ✅ | OUTPUT | LED |
| PC12 | LED3 | ✅ | ✅ | OUTPUT | LED |
| PC13 | P1_BTN5 | ✅ | ✅ | INPUT NOPULL | OK |
| PC14 | P1_BTN4 | ✅ | ✅ | INPUT NOPULL | OK |
| PC15 | P1_BTN3 | ✅ | ✅ | INPUT NOPULL | OK |

## ⚠️ PROBLEMI RILEVATI

### 1. **PA0, PA1 - RISOLTO ✅**
- **Problema**: Non erano configurati
- **Soluzione**: Aggiunti come INPUT NOPULL in `gpio.c`

### 2. **PC4 (P2_BTN2) - POSSIBILE CONFLITTO ⚠️**
```c
GPIO_InitStruct.Pin = GPIO_PIN_4|LED1_Pin|LED2_Pin|LED3_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
```
PC4 è configurato come **OUTPUT** insieme ai LED, ma dovrebbe essere **INPUT** per il pulsante P2_BTN2!

### 3. **Pin AXIS (PB0, PB1, PC2, PC3) - Da Verificare 🔍**
Questi pin sono per gli assi analogici (ADC). Se non usi gli stick analogici, non sono un problema. Altrimenti dovrebbero essere configurati come ANALOG in `adc.c`.

## 🔧 CORREZIONE NECESSARIA PER PC4

PC4 è usato sia come:
- Output per qualcosa (forse un LED?)
- Input per P2_BTN2

**Devi scegliere**:
1. Se non usi nessun LED su PC4, cambia la config in INPUT
2. Se c'è un LED su PC4, usa un altro pin per P2_BTN2

### Modifica Suggerita in `gpio.c`:

**Prima:**
```c
/*Configure GPIO pins : PC4 PCPin PCPin PCPin */
GPIO_InitStruct.Pin = GPIO_PIN_4|LED1_Pin|LED2_Pin|LED3_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
```

**Dopo (se non c'è LED su PC4):**
```c
/*Configure GPIO pins : PCPin PCPin PCPin */
GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin;  // Rimuovi GPIO_PIN_4
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
```

E aggiungi PC4 agli input di GPIOC:
```c
GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0
                      |GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7  // Aggiungi PIN_4
                      |GPIO_PIN_8|GPIO_PIN_9;
```

## 📊 RIEPILOGO STATO

| Categoria | Stato |
|-----------|-------|
| PA0, PA1 (P1 UP/DOWN) | ✅ CORRETTO |
| Tutti i pin GPIOB usati | ✅ OK |
| Pin GPIOC (tranne PC4) | ✅ OK |
| PC4 (P2_BTN2) | ⚠️ **DA CORREGGERE** |
| Pin ANALOG/ADC | 🔍 Da verificare se usi stick analogici |
| Pull-up interne | ✅ Tutte disabilitate (corretto) |
