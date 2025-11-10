/**
  ******************************************************************************
  * @file           : arcade_joystick.h
  * @brief          : Dual Arcade Joystick Emulation Header
  ******************************************************************************
  * @attention
  *
  * Emulates 2 USB HID Joysticks for arcade cabinets
  * Alternative to keyboard mode - uses joystick buttons instead of keys
  *
  ******************************************************************************
  */

#ifndef __ARCADE_JOYSTICK_H
#define __ARCADE_JOYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Configuration */
#define DEBOUNCE_TIME_MS    5    /* Button debounce time in milliseconds */
#define MAX_JOYSTICK_BUTTONS 16  /* Maximum buttons per joystick */

/* Joystick Report Structure - 2 axes + 14 buttons with Report ID */
typedef struct {
    uint8_t report_id;      /* Report ID: 1=Player1, 2=Player2 */
    uint8_t x;              /* X axis: 0-255 (127=center) */
    uint8_t y;              /* Y axis: 0-255 (127=center) */
    uint16_t buttons;       /* 14 buttons (bits 0-13) + 2 padding bits (14-15) */
} __attribute__((packed)) JoystickReport_t;

/* Button mapping structure */
typedef struct {
    GPIO_TypeDef* port;     /* GPIO port */
    uint16_t pin;           /* GPIO pin */
    uint8_t joystick_id;    /* 1 = Player 1, 2 = Player 2 */
    uint8_t button_num;     /* Button number (0-15, or 255 for axis) */
    uint8_t axis_dir;       /* 0=none, 1=up, 2=down, 3=left, 4=right */
    bool active_low;        /* true if button is active LOW */
} JoystickButtonMapping_t;

/* Function prototypes */
void Joystick_Init(void);
void Joystick_ProcessButtons(void);
void Joystick_SendReport(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARCADE_JOYSTICK_H */
