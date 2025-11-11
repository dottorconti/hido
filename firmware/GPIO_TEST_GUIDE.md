# Guida al Test Diagnostico GPIO

## Problema Rilevato

I pulsanti non vengono rilevati. Schema hardware:
- Pull-up esterna: 10kΩ a VCC
- Resistenza serie: 100Ω
- Logica: **Active LOW** (pulsante premuto = GND, non premuto = VCC)

## Configurazione Attuale

✅ **GPIO configurati correttamente**: `GPIO_NOPULL` (nessuna pull-up interna)  
✅ **Codice corretto**: `active_low = true` per tutti i pulsanti  
✅ **Logica di lettura corretta**: `GPIO_PIN_RESET` = pulsante premuto

## Test Diagnostico

Ho creato un tool per verificare direttamente i pin GPIO.

### Come Attivare il Test

1. **Modifica `Core/Src/main.c`**:
   ```c
   /* GPIO Diagnostic Test */
   #define GPIO_TEST_MODE  /* <-- Decommenta questa riga */
   ```

2. **Ricompila il firmware**:
   ```powershell
   make clean
   make
   ```

3. **Collega un convertitore USB-Serial** alla UART1:
   - **TX (STM32)**: PA9  
   - **RX (STM32)**: PA10  
   - **Baudrate**: 115200 baud, 8N1

4. **Apri un terminale seriale** (PuTTY, TeraTerm, Arduino Serial Monitor)

5. **Flash il firmware** e osserva l'output

### Output Atteso

Ogni 500ms vedrai qualcosa come:

```
========== GPIO PIN TEST ==========

--- PLAYER 1 ---
P1_UP (PA1):     HIGH
P1_DOWN (PA0):   HIGH
P1_BTN1 (PC1):   HIGH
P1_BTN2 (PC0):   HIGH
P1_BTN3 (PC15):  HIGH
P1_BTN4 (PC14):  HIGH
P1_BTN5 (PC13):  HIGH
P1_BTN6 (PB9):   HIGH
P1_BTN7 (PB8):   HIGH
P1_BTN8 (PB7):   HIGH
P1_START (PA15): HIGH

--- PLAYER 2 ---
P2_UP (PC6):     HIGH
P2_DOWN (PC7):   HIGH
P2_BTN1 (PA7):   HIGH
P2_BTN2 (PC4):   HIGH
P2_BTN3 (PC5):   HIGH
P2_START (PA6):  HIGH

===================================
```

### Cosa Verificare

1. **Con pulsanti NON premuti**, tutti i pin dovrebbero mostrare **HIGH**
2. **Premi un pulsante**: il pin corrispondente deve mostrare **LOW (Pressed)**
3. **Se un pin resta sempre LOW**: possibile cortocircuito a GND
4. **Se un pin resta sempre HIGH anche premendo**: 
   - Pulsante non connesso
   - Cavo interrotto
   - Problema con le resistenze

### Possibili Cause del Problema

#### 1. **Pin flottanti (tutti HIGH, nessun cambio)**
   - Pulsanti non collegati
   - Cavo interrotto
   - Connettore non inserito bene

#### 2. **Pin sempre LOW**
   - Cortocircuito a GND
   - Resistenza serie mancante (100Ω)
   - Problema con la pull-up

#### 3. **Pin intermittenti**
   - Problema di contatto
   - Saldatura fredda
   - Cavo difettoso

#### 4. **Inversione logica** (LOW quando non premuto)
   - Le resistenze potrebbero essere invertite
   - Schema montato al contrario

## Tornare alla Modalità Normale

Dopo il test, **ricommenta** la define:

```c
/* GPIO Diagnostic Test */
// #define GPIO_TEST_MODE  /* <-- Ricommenta */
```

Ricompila con `make` e avrai di nuovo la modalità Joystick attiva.

## Verifica Hardware Suggerita

Se i pin non rispondono correttamente, verifica con un multimetro:

1. **Tensione a riposo** (pulsante NON premuto): ~3.3V
2. **Tensione premuto**: ~0V (GND)
3. **Resistenza pull-up**: 10kΩ tra pin e VCC
4. **Resistenza serie**: 100Ω tra pulsante e pin GPIO

## Note Importanti

- Il test usa **UART1** (115200 baud)
- Se UART1 non è disponibile, verifica `usart.c`
- Il test NON interferisce con USB
- Puoi lasciare l'USB collegato durante il test

## Prossimi Passi

Una volta identificato il problema:

1. Se è hardware: sistema i collegamenti/saldature
2. Se è software: fammi sapere l'output e possiamo correggere il codice
3. Se tutti i pin funzionano ma l'USB non vede i pulsanti: problema nel layer USB HID
