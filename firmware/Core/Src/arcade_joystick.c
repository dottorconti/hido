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
#include <string.h>

/* Map HID report index (0 -> report ID 1, 1 -> report ID 2)
 * to physical player index (0 = Player1, 1 = Player2).
 */
static const uint8_t report_to_player[2] = { 0, 1 };

/* External USB device handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Joystick state - 2 separate reports (Player 1 and Player 2) */
static JoystickReport_t joystick_report[2] = {
    {.report_id = 1, .x = 127, .y = 127, .buttons = 0},  /* Player 1 */
    {.report_id = 2, .x = 127, .y = 127, .buttons = 0}   /* Player 2 */
};

/* Last sent reports - used to avoid resending identical reports each loop */
static JoystickReport_t last_sent_report[2];

/* Button mapping - Player 1 and Player 2 - 4 axes + 13 buttons each */
static const JoystickButtonMapping_t button_map[] = {
    /* Player 1 - 4 Joystick directions: IN26, IN12, IN11, IN10 */
    {P1_UP_GPIO_Port, P1_UP_Pin, 1, 255, 1, true},        /* PA15 - Up (IN26) */
    {P1_DOWN_GPIO_Port, P1_DOWN_Pin, 1, 255, 2, true},    /* PB3 - Down (IN12) */
    {P1_LEFT_GPIO_Port, P1_LEFT_Pin, 1, 255, 3, true},    /* PB4 - Left (IN11) */
    {P1_RIGHT_GPIO_Port, P1_RIGHT_Pin, 1, 255, 4, true},  /* PB5 - Right (IN10) */
    
    /* Player 1 - 13 Buttons (0-12): TIM1, TIM2, ADC1, ADC2, IN1-IN9 */
    {P1_BTN1_GPIO_Port, P1_BTN1_Pin, 1, 0, 0, true},      /* PA1 - Button 1 (TIM1) */
    {P1_BTN2_GPIO_Port, P1_BTN2_Pin, 1, 1, 0, true},      /* PA0 - Button 2 (TIM2) */
    {P1_BTN3_GPIO_Port, P1_BTN3_Pin, 1, 2, 0, true},      /* PC2 - Button 3 (ADC1) */
    {P1_BTN4_GPIO_Port, P1_BTN4_Pin, 1, 3, 0, true},      /* PC3 - Button 4 (ADC2) */
    {P1_BTN5_GPIO_Port, P1_BTN5_Pin, 1, 4, 0, true},      /* PC1 - Button 5 (IN1) */
    {P1_BTN6_GPIO_Port, P1_BTN6_Pin, 1, 5, 0, true},      /* PC0 - Button 6 (IN2) */
    {P1_BTN7_GPIO_Port, P1_BTN7_Pin, 1, 6, 0, true},      /* PC15 - Button 7 (IN3) */
    {P1_BTN8_GPIO_Port, P1_BTN8_Pin, 1, 7, 0, true},      /* PC14 - Button 8 (IN4) */
    {P1_BTN9_GPIO_Port, P1_BTN9_Pin, 1, 8, 0, true},      /* PC13 - Button 9 (IN5) */
    {P1_BTN10_GPIO_Port, P1_BTN10_Pin, 1, 9, 0, true},    /* PB9 - Button 10 (IN6) */
    {P1_BTN11_GPIO_Port, P1_BTN11_Pin, 1, 10, 0, true},   /* PB8 - Button 11 (IN7) */
    {P1_BTN12_GPIO_Port, P1_BTN12_Pin, 1, 11, 0, true},   /* PB7 - Button 12 (IN8) */
    {P1_BTN13_GPIO_Port, P1_BTN13_Pin, 1, 12, 0, true},   /* PB6 - Button 13 (IN9) */
    
    /* Player 2 - 4 Joystick directions: IN25, IN24, IN23, TIM4 */
    {P2_UP_GPIO_Port, P2_UP_Pin, 2, 255, 1, true},        /* PA6 - Up (IN25) */
    {P2_DOWN_GPIO_Port, P2_DOWN_Pin, 2, 255, 2, true},    /* PC9 - Down (IN24) */
    {P2_LEFT_GPIO_Port, P2_LEFT_Pin, 2, 255, 3, true},    /* PC8 - Left (IN23) */
    {P2_RIGHT_GPIO_Port, P2_RIGHT_Pin, 2, 255, 4, true},  /* PC7 - Right (TIM4) */
    
    /* Player 2 - 13 Buttons (0-12): IN13-IN15, IN16-IN22, TIM3, ADC3, ADC4 */
    {P2_BTN1_GPIO_Port, P2_BTN1_Pin, 2, 0, 0, true},      /* PA7 - Button 1 (IN13) */
    {P2_BTN2_GPIO_Port, P2_BTN2_Pin, 2, 1, 0, true},      /* PC4 - Button 2 (IN14) */
    {P2_BTN3_GPIO_Port, P2_BTN3_Pin, 2, 2, 0, true},      /* PC5 - Button 3 (IN15) */
    {P2_BTN4_GPIO_Port, P2_BTN4_Pin, 2, 3, 0, true},      /* PB2 - Button 4 (IN16) */
    {P2_BTN5_GPIO_Port, P2_BTN5_Pin, 2, 4, 0, true},      /* PB10 - Button 5 (IN17) */
    {P2_BTN6_GPIO_Port, P2_BTN6_Pin, 2, 5, 0, true},      /* PB11 - Button 6 (IN18) */
    {P2_BTN7_GPIO_Port, P2_BTN7_Pin, 2, 6, 0, true},      /* PB12 - Button 7 (IN19) */
    {P2_BTN8_GPIO_Port, P2_BTN8_Pin, 2, 7, 0, true},      /* PB13 - Button 8 (IN20) */
    {P2_BTN9_GPIO_Port, P2_BTN9_Pin, 2, 8, 0, true},      /* PB14 - Button 9 (IN21) */
    {P2_BTN10_GPIO_Port, P2_BTN10_Pin, 2, 9, 0, true},    /* PB15 - Button 10 (IN22) */
    {P2_BTN11_GPIO_Port, P2_BTN11_Pin, 2, 10, 0, true},   /* PC6 - Button 11 (TIM3) */
    {P2_BTN12_GPIO_Port, P2_BTN12_Pin, 2, 11, 0, true},   /* PB0 - Button 12 (ADC3) */
    {P2_BTN13_GPIO_Port, P2_BTN13_Pin, 2, 12, 0, true},   /* PB1 - Button 13 (ADC4) */
};

#define BUTTON_MAP_SIZE (sizeof(button_map) / sizeof(button_map[0]))

/* Button states for debouncing */
static uint32_t button_state[BUTTON_MAP_SIZE];      /* Current state */
static uint32_t button_last_time[BUTTON_MAP_SIZE];  /* Last change time */

/**
  * @brief  Initialize joystick system
  */
void Joystick_Init(void)
{
    /* Initialize both joystick reports */
    for (uint8_t player = 0; player < 2; player++) {
        /* Always store reports with the natural report IDs (1 and 2).
         * The mapping to physical joystick data is applied at send time
         * using `report_to_player[]` so tests can swap mapping without
         * changing stored report data layout. */
        joystick_report[player].report_id = player + 1;
        joystick_report[player].x = 127;
        joystick_report[player].y = 127;
        joystick_report[player].buttons = 0;
        /* Initialize last_sent_report to an impossible value to force first send */
        last_sent_report[player].report_id = 0xFF;
        last_sent_report[player].x = 0xFF;
        last_sent_report[player].y = 0xFF;
        last_sent_report[player].buttons = 0xFFFF;
    }
    
    /* Clear debounce state */
    for (uint8_t i = 0; i < BUTTON_MAP_SIZE; i++) {
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
    static bool player_activity[2] = {false, false};
    
    /* Reset both joysticks to center and clear buttons */
    for (uint8_t player = 0; player < 2; player++) {
        joystick_report[player].x = 127;
        joystick_report[player].y = 127;
        joystick_report[player].buttons = 0;
        player_activity[player] = false;
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
            player_activity[player_idx] = true;  // Mark activity for LED blink
            
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
    
    /* Mask buttons to ensure only 14 bits are used (bits 0-13), bits 14-15 must be 0 */
    joystick_report[0].buttons &= 0x3FFF;  // 0011111111111111 = mask for 14 bits
    joystick_report[1].buttons &= 0x3FFF;
    
    /* LED blink for activity - Player 1: LED1, Player 2: LED2 */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, player_activity[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, player_activity[1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  Send joystick reports via USB (both players as separate devices)
  */
void Joystick_SendReport(void)
{
    /* Send both HID reports (Windows sees 2 separate joystick devices) */
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        /* Send reports by report index (0 -> report ID 1, 1 -> report ID 2).
         * Use `report_to_player[]` to choose which physical player's data
         * is sent for each report ID. Also ensure the buffer's report_id
         * field matches the report ID sent to the host. */
        for (uint8_t report_idx = 0; report_idx < 2; report_idx++) {
                JoystickReport_t sendbuf = joystick_report[ report_to_player[report_idx] ];
                /* Force report_id to the expected report index + 1 */
                sendbuf.report_id = report_idx + 1;

                /* Send only if report changed since last sent - reduces USB traffic
                 * and avoids confusing the host with repeated identical reports. */
                if (memcmp(&sendbuf, &last_sent_report[report_idx], sizeof(JoystickReport_t)) != 0) {
                    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&sendbuf, sizeof(JoystickReport_t));
                    memcpy(&last_sent_report[report_idx], &sendbuf, sizeof(JoystickReport_t));
                }
        }
    }
}
