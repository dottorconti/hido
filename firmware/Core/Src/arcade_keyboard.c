/**
  ******************************************************************************
  * @file           : arcade_keyboard.c
  * @brief          : Arcade keyboard implementation
  ******************************************************************************
  * @attention
  *
  * High-performance arcade keyboard with NKRO and minimal latency
  *
  ******************************************************************************
  */

#include "arcade_keyboard.h"
#include "usbd_hid.h"
#include "usb_device.h"
#include <string.h>

/* External USB Device handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Current and previous keyboard reports for change detection */
static NKRO_KeyboardReport_t current_report = {0};
static NKRO_KeyboardReport_t previous_report = {0};

/* Debouncing state */
static uint32_t button_state[MAX_BUTTONS] = {0};      /* Current stable state */
static uint32_t button_debounce[MAX_BUTTONS] = {0};   /* Debounce counter */

/* Button to keyboard mapping - Configura questi in base al tuo hardware!
 * Esempio mapping per cabinet arcade standard:
 * Player 1: Stick (arrows) + 6 buttons
 * Player 2: WASD + 6 buttons  
 */
static const ButtonMapping_t button_map[MAX_BUTTONS] = {
    /* Player 1 Controls */
    {GPIOB, GPIO_PIN_3,  0x50, true},  // P1 Up     -> Left Arrow
    {GPIOB, GPIO_PIN_4,  0x51, true},  // P1 Down   -> Down Arrow
    {GPIOB, GPIO_PIN_5,  0x4F, true},  // P1 Left   -> Right Arrow  
    {GPIOB, GPIO_PIN_6,  0x52, true},  // P1 Right  -> Up Arrow
    {GPIOB, GPIO_PIN_7,  0x2C, true},  // P1 Button1 -> Space
    {GPIOB, GPIO_PIN_8,  0x38, true},  // P1 Button2 -> /
    {GPIOB, GPIO_PIN_9,  0x1C, true},  // P1 Button3 -> Y
    {GPIOB, GPIO_PIN_10, 0x17, true},  // P1 Button4 -> T
    {GPIOB, GPIO_PIN_11, 0x15, true},  // P1 Button5 -> R
    {GPIOB, GPIO_PIN_12, 0x09, true},  // P1 Button6 -> F
    
    /* Player 2 Controls */
    {GPIOC, GPIO_PIN_0,  0x1D, true},  // P2 Up     -> W
    {GPIOC, GPIO_PIN_1,  0x16, true},  // P2 Down   -> S
    {GPIOC, GPIO_PIN_5,  0x04, true},  // P2 Left   -> A
    {GPIOC, GPIO_PIN_6,  0x07, true},  // P2 Right  -> D
    {GPIOC, GPIO_PIN_7,  0x14, true},  // P2 Button1 -> Q
    {GPIOC, GPIO_PIN_8,  0x1A, true},  // P2 Button2 -> W (duplicate use)
    {GPIOC, GPIO_PIN_9,  0x08, true},  // P2 Button3 -> E
    {GPIOC, GPIO_PIN_13, 0x12, true},  // P2 Button4 -> O
    {GPIOC, GPIO_PIN_14, 0x0C, true},  // P2 Button5 -> I
    {GPIOC, GPIO_PIN_15, 0x18, true},  // P2 Button6 -> U
    
    /* Coin/Start buttons */
    {GPIOA, GPIO_PIN_6,  0x29, true},  // Coin 1    -> Esc
    {GPIOA, GPIO_PIN_7,  0x3A, true},  // Start 1   -> F1
    {GPIOA, GPIO_PIN_15, 0x3B, true},  // Start 2   -> F2
    
    /* Extra buttons */
    {GPIOB, GPIO_PIN_2,  0x3C, true},  // Service   -> F3
    {GPIOB, GPIO_PIN_13, 0x3D, true},  // Test      -> F4
    {GPIOB, GPIO_PIN_14, 0x3E, true},  // Extra 1   -> F5
    {GPIOB, GPIO_PIN_15, 0x3F, true},  // Extra 2   -> F6
    
    /* Unused slots */
    {NULL, 0, 0, false},
    {NULL, 0, 0, false},
    {NULL, 0, 0, false},
    {NULL, 0, 0, false},
    {NULL, 0, 0, false},
};

/**
  * @brief  Initialize arcade keyboard system
  */
void Arcade_Init(void)
{
    /* Clear all reports */
    memset(&current_report, 0, sizeof(NKRO_KeyboardReport_t));
    memset(&previous_report, 0, sizeof(NKRO_KeyboardReport_t));
    
    /* Initialize debounce state */
    memset(button_state, 0, sizeof(button_state));
    memset(button_debounce, 0, sizeof(button_debounce));
}

/**
  * @brief  Read button state with hardware debouncing
  * @param  mapping: Button mapping configuration
  * @retval true if button is pressed
  */
static inline bool ReadButton(const ButtonMapping_t* mapping)
{
    if (mapping->port == NULL) return false;
    
    GPIO_PinState state = HAL_GPIO_ReadPin(mapping->port, mapping->pin);
    
    /* Active low: button pressed when pin is LOW */
    if (mapping->active_low) {
        return (state == GPIO_PIN_RESET);
    } else {
        return (state == GPIO_PIN_SET);
    }
}

/**
  * @brief  Set key in NKRO bitmap
  * @param  report: Keyboard report
  * @param  keycode: USB HID keycode (0-95)
  */
static inline void SetKey(NKRO_KeyboardReport_t* report, uint8_t keycode)
{
    if (keycode >= 96) return;
    
    uint8_t byte_index = keycode / 8;
    uint8_t bit_index = keycode % 8;
    
    report->keys[byte_index] |= (1 << bit_index);
}

/**
  * @brief  Clear key in NKRO bitmap
  * @param  report: Keyboard report
  * @param  keycode: USB HID keycode (0-95)
  */
static inline void ClearKey(NKRO_KeyboardReport_t* report, uint8_t keycode)
{
    if (keycode >= 96) return;
    
    uint8_t byte_index = keycode / 8;
    uint8_t bit_index = keycode % 8;
    
    report->keys[byte_index] &= ~(1 << bit_index);
}

/**
  * @brief  Process all arcade buttons and update report
  */
void Arcade_ProcessButtons(void)
{
    uint32_t current_time = HAL_GetTick();
    
    /* Clear current report */
    memset(&current_report, 0, sizeof(NKRO_KeyboardReport_t));
    
    /* Scan all buttons */
    for (int i = 0; i < MAX_BUTTONS; i++) {
        if (button_map[i].port == NULL) continue;
        
        bool pressed = ReadButton(&button_map[i]);
        
        /* Simple debouncing */
        if (pressed != button_state[i]) {
            if (button_debounce[i] == 0) {
                button_debounce[i] = current_time;
            } else if ((current_time - button_debounce[i]) >= DEBOUNCE_TIME_MS) {
                /* State change confirmed after debounce time */
                button_state[i] = pressed;
                button_debounce[i] = 0;
            }
        } else {
            button_debounce[i] = 0;
        }
        
        /* Add pressed button to report */
        if (button_state[i]) {
            SetKey(&current_report, button_map[i].keycode);
        }
    }
}

/**
  * @brief  Check if keyboard report has changed
  * @retval true if report changed
  */
bool Arcade_UpdateKeyboardReport(NKRO_KeyboardReport_t* report)
{
    /* Compare with previous report */
    if (memcmp(&current_report, &previous_report, sizeof(NKRO_KeyboardReport_t)) != 0) {
        /* Report changed, copy to output */
        memcpy(report, &current_report, sizeof(NKRO_KeyboardReport_t));
        memcpy(&previous_report, &current_report, sizeof(NKRO_KeyboardReport_t));
        return true;
    }
    
    return false;
}

/**
  * @brief  Send keyboard report via USB
  */
void Arcade_SendKeyboardReport(void)
{
    NKRO_KeyboardReport_t report;
    
    if (Arcade_UpdateKeyboardReport(&report)) {
        /* Report changed, send immediately to minimize latency */
        USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&report, sizeof(NKRO_KeyboardReport_t));
    }
}
