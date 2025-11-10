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

/* Player 1 Controls (J6 Connector) */
// Joystick directional inputs (TIM1/TIM2)
#define P1_UP_Pin GPIO_PIN_1
#define P1_UP_GPIO_Port GPIOA
#define P1_DOWN_Pin GPIO_PIN_0
#define P1_DOWN_GPIO_Port GPIOA

// Joystick analog axes (ADC1/ADC2)
#define P1_AXIS_X_Pin GPIO_PIN_3
#define P1_AXIS_X_GPIO_Port GPIOC
#define P1_AXIS_Y_Pin GPIO_PIN_2
#define P1_AXIS_Y_GPIO_Port GPIOC

// Player 1 buttons (8 buttons)
#define P1_BTN1_Pin GPIO_PIN_1
#define P1_BTN1_GPIO_Port GPIOC
#define P1_BTN2_Pin GPIO_PIN_0
#define P1_BTN2_GPIO_Port GPIOC
#define P1_BTN3_Pin GPIO_PIN_15
#define P1_BTN3_GPIO_Port GPIOC
#define P1_BTN4_Pin GPIO_PIN_14
#define P1_BTN4_GPIO_Port GPIOC
#define P1_BTN5_Pin GPIO_PIN_13
#define P1_BTN5_GPIO_Port GPIOC
#define P1_BTN6_Pin GPIO_PIN_9
#define P1_BTN6_GPIO_Port GPIOB
#define P1_BTN7_Pin GPIO_PIN_8
#define P1_BTN7_GPIO_Port GPIOB
#define P1_BTN8_Pin GPIO_PIN_7
#define P1_BTN8_GPIO_Port GPIOB

// Additional P1 buttons
#define P1_BTN9_Pin GPIO_PIN_6
#define P1_BTN9_GPIO_Port GPIOB
#define P1_BTN10_Pin GPIO_PIN_5
#define P1_BTN10_GPIO_Port GPIOB
#define P1_BTN11_Pin GPIO_PIN_4
#define P1_BTN11_GPIO_Port GPIOB
#define P1_BTN12_Pin GPIO_PIN_3
#define P1_BTN12_GPIO_Port GPIOB
#define P1_START_Pin GPIO_PIN_15
#define P1_START_GPIO_Port GPIOA

/* Player 2 Controls (J7 Connector) */
// Joystick directional inputs (TIM3/TIM4)
#define P2_UP_Pin GPIO_PIN_6
#define P2_UP_GPIO_Port GPIOC
#define P2_DOWN_Pin GPIO_PIN_7
#define P2_DOWN_GPIO_Port GPIOC

// Joystick analog axes (ADC3/ADC4)
#define P2_AXIS_X_Pin GPIO_PIN_0
#define P2_AXIS_X_GPIO_Port GPIOB
#define P2_AXIS_Y_Pin GPIO_PIN_1
#define P2_AXIS_Y_GPIO_Port GPIOB

// Player 2 buttons (8 buttons)
#define P2_BTN1_Pin GPIO_PIN_7
#define P2_BTN1_GPIO_Port GPIOA
#define P2_BTN2_Pin GPIO_PIN_4
#define P2_BTN2_GPIO_Port GPIOC
#define P2_BTN3_Pin GPIO_PIN_5
#define P2_BTN3_GPIO_Port GPIOC
#define P2_BTN4_Pin GPIO_PIN_2
#define P2_BTN4_GPIO_Port GPIOB
#define P2_BTN5_Pin GPIO_PIN_10
#define P2_BTN5_GPIO_Port GPIOB
#define P2_BTN6_Pin GPIO_PIN_11
#define P2_BTN6_GPIO_Port GPIOB
#define P2_BTN7_Pin GPIO_PIN_12
#define P2_BTN7_GPIO_Port GPIOB
#define P2_BTN8_Pin GPIO_PIN_13
#define P2_BTN8_GPIO_Port GPIOB

// Additional P2 buttons
#define P2_BTN9_Pin GPIO_PIN_14
#define P2_BTN9_GPIO_Port GPIOB
#define P2_BTN10_Pin GPIO_PIN_15
#define P2_BTN10_GPIO_Port GPIOB
#define P2_BTN11_Pin GPIO_PIN_8
#define P2_BTN11_GPIO_Port GPIOC
#define P2_BTN12_Pin GPIO_PIN_9
#define P2_BTN12_GPIO_Port GPIOC
#define P2_START_Pin GPIO_PIN_6
#define P2_START_GPIO_Port GPIOA

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
