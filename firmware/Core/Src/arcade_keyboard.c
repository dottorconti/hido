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
#include "main.h"
#include <string.h>

/* External USB Device handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Current and previous keyboard reports for change detection */
static NKRO_KeyboardReport_t current_report = {0};
static NKRO_KeyboardReport_t previous_report = {0};

/* Debouncing state */
static uint32_t button_state[MAX_BUTTONS] = {0};      /* Current stable state */
static uint32_t button_debounce[MAX_BUTTONS] = {0};   /* Debounce counter */

/* Button to keyboard mapping - Mapped to actual hardware pins from main.h
 * Player 1 (J6): 4 directions + 13 buttons = 17 inputs
 * Player 2 (J7): 4 directions + 13 buttons = 17 inputs
 * Total: 34 inputs
 */
static const ButtonMapping_t button_map[MAX_BUTTONS] = {
    /* Player 1 Controls (indices 0-16) */
    {P1_UP_GPIO_Port,    P1_UP_Pin,     0x52, true},  // P1 Up      -> Up Arrow
    {P1_DOWN_GPIO_Port,  P1_DOWN_Pin,   0x51, true},  // P1 Down    -> Down Arrow
    {P1_LEFT_GPIO_Port,  P1_LEFT_Pin,   0x50, true},  // P1 Left    -> Left Arrow
    {P1_RIGHT_GPIO_Port, P1_RIGHT_Pin,  0x4F, true},  // P1 Right   -> Right Arrow
    {P1_BTN1_GPIO_Port,  P1_BTN1_Pin,   0x1D, true},  // P1 Button1 -> Z
    {P1_BTN2_GPIO_Port,  P1_BTN2_Pin,   0x1B, true},  // P1 Button2 -> X
    {P1_BTN3_GPIO_Port,  P1_BTN3_Pin,   0x06, true},  // P1 Button3 -> C
    {P1_BTN4_GPIO_Port,  P1_BTN4_Pin,   0x19, true},  // P1 Button4 -> V
    {P1_BTN5_GPIO_Port,  P1_BTN5_Pin,   0x05, true},  // P1 Button5 -> B
    {P1_BTN6_GPIO_Port,  P1_BTN6_Pin,   0x11, true},  // P1 Button6 -> N
    {P1_BTN7_GPIO_Port,  P1_BTN7_Pin,   0x10, true},  // P1 Button7 -> M
    {P1_BTN8_GPIO_Port,  P1_BTN8_Pin,   0x14, true},  // P1 Button8 -> Q
    {P1_BTN9_GPIO_Port,  P1_BTN9_Pin,   0x1A, true},  // P1 Button9 -> W
    {P1_BTN10_GPIO_Port, P1_BTN10_Pin,  0x08, true},  // P1 Button10 -> E
    {P1_BTN11_GPIO_Port, P1_BTN11_Pin,  0x15, true},  // P1 Button11 -> R
    {P1_BTN12_GPIO_Port, P1_BTN12_Pin,  0x17, true},  // P1 Button12 -> T
    {P1_BTN13_GPIO_Port, P1_BTN13_Pin,  0x1C, true},  // P1 Button13 -> Y
    
    /* Player 2 Controls (indices 17-33) */
    {P2_UP_GPIO_Port,    P2_UP_Pin,     0x3A, true},  // P2 Up      -> F1
    {P2_DOWN_GPIO_Port,  P2_DOWN_Pin,   0x3B, true},  // P2 Down    -> F2
    {P2_LEFT_GPIO_Port,  P2_LEFT_Pin,   0x3C, true},  // P2 Left    -> F3
    {P2_RIGHT_GPIO_Port, P2_RIGHT_Pin,  0x3D, true},  // P2 Right   -> F4
    {P2_BTN1_GPIO_Port,  P2_BTN1_Pin,   0x04, true},  // P2 Button1 -> A
    {P2_BTN2_GPIO_Port,  P2_BTN2_Pin,   0x16, true},  // P2 Button2 -> S
    {P2_BTN3_GPIO_Port,  P2_BTN3_Pin,   0x07, true},  // P2 Button3 -> D
    {P2_BTN4_GPIO_Port,  P2_BTN4_Pin,   0x09, true},  // P2 Button4 -> F
    {P2_BTN5_GPIO_Port,  P2_BTN5_Pin,   0x0A, true},  // P2 Button5 -> G
    {P2_BTN6_GPIO_Port,  P2_BTN6_Pin,   0x0B, true},  // P2 Button6 -> H
    {P2_BTN7_GPIO_Port,  P2_BTN7_Pin,   0x0D, true},  // P2 Button7 -> J
    {P2_BTN8_GPIO_Port,  P2_BTN8_Pin,   0x0E, true},  // P2 Button8 -> K
    {P2_BTN9_GPIO_Port,  P2_BTN9_Pin,   0x0F, true},  // P2 Button9 -> L
    {P2_BTN10_GPIO_Port, P2_BTN10_Pin,  0x18, true},  // P2 Button10 -> U
    {P2_BTN11_GPIO_Port, P2_BTN11_Pin,  0x0C, true},  // P2 Button11 -> I
    {P2_BTN12_GPIO_Port, P2_BTN12_Pin,  0x12, true},  // P2 Button12 -> O
    {P2_BTN13_GPIO_Port, P2_BTN13_Pin,  0x13, true},  // P2 Button13 -> P
    
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
  * @brief  Add key to 6KRO report
  * @param  report: Keyboard report
  * @param  keycode: USB HID keycode
  */
static inline void AddKey(NKRO_KeyboardReport_t* report, uint8_t keycode)
{
    /* Find first empty slot (0 = no key) */
    for (int i = 0; i < 6; i++) {
        if (report->keys[i] == 0) {
            report->keys[i] = keycode;
            return;
        }
        /* Avoid duplicates */
        if (report->keys[i] == keycode) {
            return;
        }
    }
    /* All slots full - key will be ignored (6KRO limit) */
}

/**
  * @brief  Process all arcade buttons and update report
  */
void Arcade_ProcessButtons(void)
{
    uint32_t current_time = HAL_GetTick();
    bool p1_active = false;
    bool p2_active = false;
    
    /* Clear current report */
    memset(&current_report, 0, sizeof(NKRO_KeyboardReport_t));
    current_report.report_id = 1;  /* Set Report ID */
    
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
        
        /* Add pressed button to report (max 6 keys) */
        if (button_state[i]) {
            AddKey(&current_report, button_map[i].keycode);
            
            /* Track which player is active (0-16 = P1, 17-33 = P2) */
            if (i < 17) {
                p1_active = true;
            } else if (i < 34) {
                p2_active = true;
            }
        }
    }
    
    /* LED Debug: LED1 for P1, LED2 for P2 */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, p1_active ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, p2_active ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
