/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_12
#define LED3_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* Player 1 Controls (J6 Connector) - 4 Axes + 13 Buttons */
// Joystick directional inputs - mapped to IN26, IN12, IN11, IN10
#define P1_UP_Pin GPIO_PIN_15       // J6 Pin 18 - IN26
#define P1_UP_GPIO_Port GPIOA
#define P1_DOWN_Pin GPIO_PIN_3      // J6 Pin 17 - IN12
#define P1_DOWN_GPIO_Port GPIOB
#define P1_LEFT_Pin GPIO_PIN_4      // J6 Pin 16 - IN11
#define P1_LEFT_GPIO_Port GPIOB
#define P1_RIGHT_Pin GPIO_PIN_5     // J6 Pin 15 - IN10
#define P1_RIGHT_GPIO_Port GPIOB

// Player 1 buttons (13 buttons) - use TIM1, TIM2, ADC1, ADC2 and remaining inputs
#define P1_BTN1_Pin GPIO_PIN_1      // J6 Pin 2 - TIM1
#define P1_BTN1_GPIO_Port GPIOA
#define P1_BTN2_Pin GPIO_PIN_0      // J6 Pin 3 - TIM2
#define P1_BTN2_GPIO_Port GPIOA
#define P1_BTN3_Pin GPIO_PIN_2      // J6 Pin 4 - ADC1
#define P1_BTN3_GPIO_Port GPIOC
#define P1_BTN4_Pin GPIO_PIN_3      // J6 Pin 5 - ADC2
#define P1_BTN4_GPIO_Port GPIOC
#define P1_BTN5_Pin GPIO_PIN_1      // J6 Pin 6 - IN1
#define P1_BTN5_GPIO_Port GPIOC
#define P1_BTN6_Pin GPIO_PIN_0      // J6 Pin 7 - IN2
#define P1_BTN6_GPIO_Port GPIOC
#define P1_BTN7_Pin GPIO_PIN_15     // J6 Pin 8 - IN3
#define P1_BTN7_GPIO_Port GPIOC
#define P1_BTN8_Pin GPIO_PIN_14     // J6 Pin 9 - IN4
#define P1_BTN8_GPIO_Port GPIOC
#define P1_BTN9_Pin GPIO_PIN_13     // J6 Pin 10 - IN5
#define P1_BTN9_GPIO_Port GPIOC
#define P1_BTN10_Pin GPIO_PIN_9     // J6 Pin 11 - IN6
#define P1_BTN10_GPIO_Port GPIOB
#define P1_BTN11_Pin GPIO_PIN_8     // J6 Pin 12 - IN7
#define P1_BTN11_GPIO_Port GPIOB
#define P1_BTN12_Pin GPIO_PIN_7     // J6 Pin 13 - IN8
#define P1_BTN12_GPIO_Port GPIOB
#define P1_BTN13_Pin GPIO_PIN_6     // J6 Pin 14 - IN9
#define P1_BTN13_GPIO_Port GPIOB

/* Player 2 Controls (J7 Connector) - 4 Axes + 13 Buttons */
// Joystick directional inputs - keep same as before
#define P2_UP_Pin GPIO_PIN_6        // J7 Pin 18 - IN25
#define P2_UP_GPIO_Port GPIOA
#define P2_DOWN_Pin GPIO_PIN_9      // J7 Pin 17 - IN24
#define P2_DOWN_GPIO_Port GPIOC
#define P2_LEFT_Pin GPIO_PIN_8      // J7 Pin 16 - IN23
#define P2_LEFT_GPIO_Port GPIOC
#define P2_RIGHT_Pin GPIO_PIN_7     // J7 Pin 15 - TIM4
#define P2_RIGHT_GPIO_Port GPIOC

// Player 2 buttons (13 buttons) - skip ADC3/4 (PB0/PB1)
#define P2_BTN1_Pin GPIO_PIN_7      // J7 Pin 2 - IN13
#define P2_BTN1_GPIO_Port GPIOA
#define P2_BTN2_Pin GPIO_PIN_4      // J7 Pin 3 - IN14
#define P2_BTN2_GPIO_Port GPIOC
#define P2_BTN3_Pin GPIO_PIN_5      // J7 Pin 4 - IN15
#define P2_BTN3_GPIO_Port GPIOC
#define P2_BTN4_Pin GPIO_PIN_2      // J7 Pin 7 - IN16
#define P2_BTN4_GPIO_Port GPIOB
#define P2_BTN5_Pin GPIO_PIN_10     // J7 Pin 8 - IN17
#define P2_BTN5_GPIO_Port GPIOB
#define P2_BTN6_Pin GPIO_PIN_11     // J7 Pin 9 - IN18
#define P2_BTN6_GPIO_Port GPIOB
#define P2_BTN7_Pin GPIO_PIN_12     // J7 Pin 10 - IN19
#define P2_BTN7_GPIO_Port GPIOB
#define P2_BTN8_Pin GPIO_PIN_13     // J7 Pin 11 - IN20
#define P2_BTN8_GPIO_Port GPIOB
#define P2_BTN9_Pin GPIO_PIN_14     // J7 Pin 12 - IN21
#define P2_BTN9_GPIO_Port GPIOB
#define P2_BTN10_Pin GPIO_PIN_15    // J7 Pin 13 - IN22
#define P2_BTN10_GPIO_Port GPIOB
#define P2_BTN11_Pin GPIO_PIN_6     // J7 Pin 14 - TIM3
#define P2_BTN11_GPIO_Port GPIOC
#define P2_BTN12_Pin GPIO_PIN_0     // J7 Pin 5 - ADC3 (as button, for compatibility)
#define P2_BTN12_GPIO_Port GPIOB
#define P2_BTN13_Pin GPIO_PIN_1     // J7 Pin 6 - ADC4 (as button, for compatibility)
#define P2_BTN13_GPIO_Port GPIOB

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
