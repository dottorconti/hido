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

/* Joystick states */
static JoystickReport_t joystick1_report = {0};
static JoystickReport_t joystick2_report = {0};

/* Button states for debouncing */
static uint32_t button_state[32] = {0};      /* Current state */
static uint32_t button_last_time[32] = {0};  /* Last change time */

/* Button mapping - Player 1 and Player 2 */
static const JoystickButtonMapping_t button_map[] = {
    /* Player 1 - Joystick directions */
    {GPIOB, GPIO_PIN_3, 1, 255, 1, true},  /* PB3 - Up */
    {GPIOB, GPIO_PIN_4, 1, 255, 2, true},  /* PB4 - Down */
    {GPIOB, GPIO_PIN_5, 1, 255, 3, true},  /* PB5 - Left */
    {GPIOB, GPIO_PIN_6, 1, 255, 4, true},  /* PB6 - Right */
    
    /* Player 1 - Buttons (0-11) */
    {GPIOB, GPIO_PIN_7,  1, 0, 0, true},   /* Button 1 */
    {GPIOB, GPIO_PIN_8,  1, 1, 0, true},   /* Button 2 */
    {GPIOB, GPIO_PIN_9,  1, 2, 0, true},   /* Button 3 */
    {GPIOB, GPIO_PIN_10, 1, 3, 0, true},   /* Button 4 */
    {GPIOB, GPIO_PIN_11, 1, 4, 0, true},   /* Button 5 */
    {GPIOB, GPIO_PIN_12, 1, 5, 0, true},   /* Button 6 */
    {GPIOA, GPIO_PIN_6,  1, 6, 0, true},   /* Coin */
    {GPIOA, GPIO_PIN_7,  1, 7, 0, true},   /* Start */
    
    /* Player 2 - Joystick directions */
    {GPIOC, GPIO_PIN_0, 2, 255, 1, true},  /* PC0 - Up (W) */
    {GPIOC, GPIO_PIN_1, 2, 255, 2, true},  /* PC1 - Down (S) */
    {GPIOC, GPIO_PIN_5, 2, 255, 3, true},  /* PC5 - Left (A) */
    {GPIOC, GPIO_PIN_6, 2, 255, 4, true},  /* PC6 - Right (D) */
    
    /* Player 2 - Buttons (0-11) */
    {GPIOC, GPIO_PIN_7,  2, 0, 0, true},   /* Button 1 */
    {GPIOC, GPIO_PIN_8,  2, 1, 0, true},   /* Button 2 */
    {GPIOC, GPIO_PIN_9,  2, 2, 0, true},   /* Button 3 */
    {GPIOC, GPIO_PIN_13, 2, 3, 0, true},   /* Button 4 */
    {GPIOC, GPIO_PIN_14, 2, 4, 0, true},   /* Button 5 */
    {GPIOC, GPIO_PIN_15, 2, 5, 0, true},   /* Button 6 */
    {GPIOA, GPIO_PIN_15, 2, 6, 0, true},   /* Coin */
    {GPIOB, GPIO_PIN_2,  2, 7, 0, true},   /* Start */
    
    /* System buttons shared */
    {GPIOB, GPIO_PIN_13, 1, 8, 0, true},   /* Service (P1) */
    {GPIOB, GPIO_PIN_14, 1, 9, 0, true},   /* Test (P1) */
};

#define BUTTON_MAP_SIZE (sizeof(button_map) / sizeof(button_map[0]))

/**
  * @brief  Initialize joystick system
  */
void Joystick_Init(void)
{
    /* Initialize joystick 1 (Report ID = 1) */
    joystick1_report.report_id = 1;
    joystick1_report.x = 0;
    joystick1_report.y = 0;
    joystick1_report.buttons = 0;
    
    /* Initialize joystick 2 (Report ID = 2) */
    joystick2_report.report_id = 2;
    joystick2_report.x = 0;
    joystick2_report.y = 0;
    joystick2_report.buttons = 0;
    
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
    
    /* Reset axis positions */
    joystick1_report.x = 0;
    joystick1_report.y = 0;
    joystick2_report.x = 0;
    joystick2_report.y = 0;
    
    /* Scan all mapped buttons */
    for (uint8_t i = 0; i < BUTTON_MAP_SIZE; i++) {
        const JoystickButtonMapping_t* mapping = &button_map[i];
        
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
        
        /* Update joystick state */
        JoystickReport_t* report = (mapping->joystick_id == 1) ? &joystick1_report : &joystick2_report;
        
        if (button_state[i]) {
            if (mapping->button_num == 255) {
                /* Axis direction */
                switch (mapping->axis_dir) {
                    case 1: report->y = -127; break;  /* Up */
                    case 2: report->y = +127; break;  /* Down */
                    case 3: report->x = -127; break;  /* Left */
                    case 4: report->x = +127; break;  /* Right */
                }
            } else {
                /* Button press */
                report->buttons |= (1 << mapping->button_num);
            }
        } else {
            if (mapping->button_num != 255) {
                /* Button release */
                report->buttons &= ~(1 << mapping->button_num);
            }
        }
    }
}

/**
  * @brief  Send joystick report via USB
  * @param  joystick_id: 1 or 2
  */
void Joystick_SendReport(uint8_t joystick_id)
{
    JoystickReport_t* report = (joystick_id == 1) ? &joystick1_report : &joystick2_report;
    
    /* Send HID report */
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)report, sizeof(JoystickReport_t));
    }
}
