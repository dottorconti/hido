/**
  ******************************************************************************
  * @file    flash_config.h
  * @brief   Flash memory configuration storage (EEPROM emulation)
  ******************************************************************************
  */

#ifndef __FLASH_CONFIG_H
#define __FLASH_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Configuration storage address (last 2KB page of STM32F102RB) */
#define CONFIG_FLASH_ADDR       0x0801F800  /* Last page at 126KB */
#define CONFIG_FLASH_SIZE       2048        /* 2KB page size */

/* Magic number for configuration validation */
#define CONFIG_MAGIC            0x48494430  /* "HID0" */
#define CONFIG_VERSION          1

/* Maximum pins per player */
#define MAX_PINS_PER_PLAYER     17

#ifdef USE_KEYBOARD_MODE
/* Keyboard mode configuration */
typedef struct {
    uint8_t silk_pin;       /* Silkscreen pin number (0-16) */
    uint8_t hid_keycode;    /* USB HID keycode (0x04-0x65) */
    char key_name[16];      /* Human readable name ("A", "SPACE", etc.) */
} KeyboardMapping_t;

typedef struct {
    uint32_t magic;                             /* CONFIG_MAGIC */
    uint32_t version;                           /* CONFIG_VERSION */
    KeyboardMapping_t player1[MAX_PINS_PER_PLAYER];
    KeyboardMapping_t player2[MAX_PINS_PER_PLAYER];
    uint32_t crc32;                             /* CRC32 checksum */
} KeyboardConfig_t;

#else /* USE_JOYSTICK_MODE */

/* Joystick function types */
typedef enum {
    JOY_FUNC_BUTTON_1 = 0,
    JOY_FUNC_BUTTON_2,
    JOY_FUNC_BUTTON_3,
    JOY_FUNC_BUTTON_4,
    JOY_FUNC_BUTTON_5,
    JOY_FUNC_BUTTON_6,
    JOY_FUNC_BUTTON_7,
    JOY_FUNC_BUTTON_8,
    JOY_FUNC_BUTTON_9,
    JOY_FUNC_BUTTON_10,
    JOY_FUNC_BUTTON_11,
    JOY_FUNC_BUTTON_12,
    JOY_FUNC_BUTTON_13,
    JOY_FUNC_BUTTON_14,
    JOY_FUNC_AXIS_UP,       /* 14 */
    JOY_FUNC_AXIS_DOWN,     /* 15 */
    JOY_FUNC_AXIS_LEFT,     /* 16 */
    JOY_FUNC_AXIS_RIGHT,    /* 17 */
    JOY_FUNC_DISABLED       /* 18 - Pin not used */
} JoystickFunction_t;

/* Joystick mode configuration */
typedef struct {
    uint8_t silk_pin;           /* Silkscreen pin number (0-16) */
    uint8_t joy_function;       /* JoystickFunction_t */
    char func_name[16];         /* Human readable name ("Button 1", "UP", etc.) */
} JoystickMapping_t;

typedef struct {
    uint32_t magic;                             /* CONFIG_MAGIC */
    uint32_t version;                           /* CONFIG_VERSION */
    JoystickMapping_t player1[MAX_PINS_PER_PLAYER];
    JoystickMapping_t player2[MAX_PINS_PER_PLAYER];
    uint32_t crc32;                             /* CRC32 checksum */
} JoystickConfig_t;

#endif /* USE_KEYBOARD_MODE */

/* Public functions */
HAL_StatusTypeDef FlashConfig_Load(void);
HAL_StatusTypeDef FlashConfig_Save(void);
HAL_StatusTypeDef FlashConfig_Reset(void);
uint8_t FlashConfig_IsValid(void);
void FlashConfig_LoadDefaults(void);

#ifdef USE_KEYBOARD_MODE
KeyboardConfig_t* FlashConfig_Get(void);
#else
JoystickConfig_t* FlashConfig_Get(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_CONFIG_H */
