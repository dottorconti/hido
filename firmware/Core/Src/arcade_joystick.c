/**
  ******************************************************************************
  * @file           : arcade_joystick.c
  * @brief          : Dual Arcade Joystick Implementation
  ******************************************************************************
  * @attention
  *
  * Implements 2 USB HID Joysticks (Player 1 and Player 2)
  * Each joystick has:
  * - 2 analog axes (X, Y) controlled by directional buttons
  * - Up to 16 digital buttons
  *
  ******************************************************************************
  */

#include "arcade_joystick.h"
#include "usbd_hid.h"
#include "gpio.h"

/* External USB device handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Joystick state - 2 separate reports (Player 1 and Player 2) */
static JoystickReport_t joystick_report[2] = {
    {.report_id = 1, .x = 127, .y = 127, .buttons = 0},  /* Player 1 */
    {.report_id = 2, .x = 127, .y = 127, .buttons = 0}   /* Player 2 */
};

/* Button states for debouncing */
static uint32_t button_state[32] = {0};      /* Current state */
static uint32_t button_last_time[32] = {0};  /* Last change time */

/* Button mapping - Player 1 and Player 2 using new pin definitions from main.h */
static const JoystickButtonMapping_t button_map[] = {
    /* Player 1 - Joystick directions */
    {P1_UP_GPIO_Port, P1_UP_Pin, 1, 255, 1, true},        /* PA1 - Up */
    {P1_DOWN_GPIO_Port, P1_DOWN_Pin, 1, 255, 2, true},    /* PA0 - Down */
    
    /* Player 1 - Buttons (0-15) */
    {P1_BTN1_GPIO_Port, P1_BTN1_Pin, 1, 0, 0, true},      /* PC1 - Button 1 */
    {P1_BTN2_GPIO_Port, P1_BTN2_Pin, 1, 1, 0, true},      /* PC0 - Button 2 */
    {P1_BTN3_GPIO_Port, P1_BTN3_Pin, 1, 2, 0, true},      /* PC15 - Button 3 */
    {P1_BTN4_GPIO_Port, P1_BTN4_Pin, 1, 3, 0, true},      /* PC14 - Button 4 */
    {P1_BTN5_GPIO_Port, P1_BTN5_Pin, 1, 4, 0, true},      /* PC13 - Button 5 */
    {P1_BTN6_GPIO_Port, P1_BTN6_Pin, 1, 5, 0, true},      /* PB9 - Button 6 */
    {P1_BTN7_GPIO_Port, P1_BTN7_Pin, 1, 6, 0, true},      /* PB8 - Button 7 */
    {P1_BTN8_GPIO_Port, P1_BTN8_Pin, 1, 7, 0, true},      /* PB7 - Button 8 */
    {P1_BTN9_GPIO_Port, P1_BTN9_Pin, 1, 8, 0, true},      /* PB6 - Button 9 */
    {P1_BTN10_GPIO_Port, P1_BTN10_Pin, 1, 9, 0, true},    /* PB5 - Button 10 */
    {P1_BTN11_GPIO_Port, P1_BTN11_Pin, 1, 10, 0, true},   /* PB4 - Button 11 */
    {P1_BTN12_GPIO_Port, P1_BTN12_Pin, 1, 11, 0, true},   /* PB3 - Button 12 */
    {P1_START_GPIO_Port, P1_START_Pin, 1, 12, 0, true},   /* PA15 - Button 13 (Start) */
    {P1_UP_GPIO_Port, P1_UP_Pin, 1, 13, 0, true},         /* PA1 - Button 14 (can be dual-mapped) */
    
    /* Player 2 - Joystick directions */
    {P2_UP_GPIO_Port, P2_UP_Pin, 2, 255, 1, true},        /* PC6 - Up */
    {P2_DOWN_GPIO_Port, P2_DOWN_Pin, 2, 255, 2, true},    /* PC7 - Down */
    
    /* Player 2 - Buttons (0-15) */
    {P2_BTN1_GPIO_Port, P2_BTN1_Pin, 2, 0, 0, true},      /* PA7 - Button 1 */
    {P2_BTN2_GPIO_Port, P2_BTN2_Pin, 2, 1, 0, true},      /* PC4 - Button 2 */
    {P2_BTN3_GPIO_Port, P2_BTN3_Pin, 2, 2, 0, true},      /* PC5 - Button 3 */
    {P2_BTN4_GPIO_Port, P2_BTN4_Pin, 2, 3, 0, true},      /* PB2 - Button 4 */
    {P2_BTN5_GPIO_Port, P2_BTN5_Pin, 2, 4, 0, true},      /* PB10 - Button 5 */
    {P2_BTN6_GPIO_Port, P2_BTN6_Pin, 2, 5, 0, true},      /* PB11 - Button 6 */
    {P2_BTN7_GPIO_Port, P2_BTN7_Pin, 2, 6, 0, true},      /* PB12 - Button 7 */
    {P2_BTN8_GPIO_Port, P2_BTN8_Pin, 2, 7, 0, true},      /* PB13 - Button 8 */
    {P2_BTN9_GPIO_Port, P2_BTN9_Pin, 2, 8, 0, true},      /* PB14 - Button 9 */
    {P2_BTN10_GPIO_Port, P2_BTN10_Pin, 2, 9, 0, true},    /* PB15 - Button 10 */
    {P2_BTN11_GPIO_Port, P2_BTN11_Pin, 2, 10, 0, true},   /* PC8 - Button 11 */
    {P2_BTN12_GPIO_Port, P2_BTN12_Pin, 2, 11, 0, true},   /* PC9 - Button 12 */
    {P2_START_GPIO_Port, P2_START_Pin, 2, 12, 0, true},   /* PA6 - Start */
};

#define BUTTON_MAP_SIZE (sizeof(button_map) / sizeof(button_map[0]))

/**
  * @brief  Initialize joystick system
  */
void Joystick_Init(void)
{
    /* Initialize both joystick reports */
    for (uint8_t player = 0; player < 2; player++) {
        joystick_report[player].report_id = player + 1;
        joystick_report[player].x = 127;
        joystick_report[player].y = 127;
        joystick_report[player].buttons = 0;
    }
    
    /* Clear debounce state */
    for (uint8_t i = 0; i < 32; i++) {
        button_state[i] = 0;
        button_last_time[i] = 0;
    }
}

/**
  * @brief  Process all buttons with debouncing
  */
void Joystick_ProcessButtons(void)
{
    uint32_t current_time = HAL_GetTick();
    
    /* Reset both joysticks to center and clear buttons */
    for (uint8_t player = 0; player < 2; player++) {
        joystick_report[player].x = 127;
        joystick_report[player].y = 127;
        joystick_report[player].buttons = 0;
    }
    
    /* Scan all mapped buttons for both players */
    for (uint8_t i = 0; i < BUTTON_MAP_SIZE; i++) {
        const JoystickButtonMapping_t* mapping = &button_map[i];
        
        /* Get player index (0=Player1, 1=Player2) */
        uint8_t player_idx = mapping->joystick_id - 1;
        if (player_idx >= 2) continue;  /* Skip invalid IDs */
        
        /* Read GPIO pin */
        GPIO_PinState pin_state = HAL_GPIO_ReadPin(mapping->port, mapping->pin);
        bool is_pressed = (mapping->active_low) ? (pin_state == GPIO_PIN_RESET) : (pin_state == GPIO_PIN_SET);
        
        /* Debouncing */
        if (is_pressed != button_state[i]) {
            if ((current_time - button_last_time[i]) >= DEBOUNCE_TIME_MS) {
                button_state[i] = is_pressed;
                button_last_time[i] = current_time;
            }
        }
        
        /* Update joystick state if button is pressed */
        if (button_state[i]) {
            if (mapping->button_num == 255) {
                /* Axis direction */
                switch (mapping->axis_dir) {
                    case 1: joystick_report[player_idx].y = 0; break;    /* Up */
                    case 2: joystick_report[player_idx].y = 255; break;  /* Down */
                    case 3: joystick_report[player_idx].x = 0; break;    /* Left */
                    case 4: joystick_report[player_idx].x = 255; break;  /* Right */
                }
            } else {
                /* Button press (0-13 for 14 buttons) */
                if (mapping->button_num < 14) {
                    joystick_report[player_idx].buttons |= (1 << mapping->button_num);
                }
            }
        }
    }
}

/**
  * @brief  Send joystick reports via USB (both players as separate devices)
  */
void Joystick_SendReport(void)
{
    /* Send both HID reports (Windows sees 2 separate joystick devices) */
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        /* Send Player 1 report (Report ID 1) */
        USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&joystick_report[0], sizeof(JoystickReport_t));
        
        /* Small delay between reports */
        HAL_Delay(1);
        
        /* Send Player 2 report (Report ID 2) */
        USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&joystick_report[1], sizeof(JoystickReport_t));
    }
}
