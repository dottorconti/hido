/**
  ******************************************************************************
  * @file           : arcade_keyboard.h
  * @brief          : Header for arcade keyboard handling
  ******************************************************************************
  * @attention
  *
  * Arcade keyboard controller with NKRO support
  * Optimized for minimal latency and no ghosting
  *
  ******************************************************************************
  */

#ifndef __ARCADE_KEYBOARD_H
#define __ARCADE_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Configuration */
#define USE_DIRECT_BUTTONS      /* Comment to enable JVS mode */
#define MAX_BUTTONS             32  /* Maximum number of arcade buttons */

/* HID Keyboard Report Structure (14 bytes for NKRO) */
typedef struct {
    uint8_t modifiers;      /* Modifier keys (Ctrl, Shift, Alt, GUI) */
    uint8_t reserved;       /* Reserved byte */
    uint8_t keys[12];       /* 96-bit bitmap for keys (12 bytes = 96 keys) */
} NKRO_KeyboardReport_t;

/* Button mapping structure */
typedef struct {
    GPIO_TypeDef* port;     /* GPIO port */
    uint16_t pin;           /* GPIO pin */
    uint8_t keycode;        /* USB HID keycode */
    bool active_low;        /* true if button pulls to GND when pressed */
} ButtonMapping_t;

/* Debouncing configuration */
#define DEBOUNCE_TIME_MS    5   /* Debounce time in milliseconds */

/* Function prototypes */
void Arcade_Init(void);
void Arcade_ProcessButtons(void);
bool Arcade_UpdateKeyboardReport(NKRO_KeyboardReport_t* report);
void Arcade_SendKeyboardReport(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARCADE_KEYBOARD_H */
