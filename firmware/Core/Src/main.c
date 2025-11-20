/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "flash_config.h"

/* Mode-specific includes */
#ifdef USE_KEYBOARD_MODE
#include "arcade_keyboard.h"
#elif defined(USE_JOYSTICK_MODE)
#include "arcade_joystick.h"
#elif defined(USE_JVS_MODE)
#include "jvs_protocol.h"
#endif

/* GPIO Diagnostic Test */
// #define GPIO_TEST_MODE  /* Uncomment to enable GPIO diagnostic test */
#ifdef GPIO_TEST_MODE
#include "gpio_test.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Arcade Keyboard Controller - NKRO with minimal latency */
#define SCAN_INTERVAL_US    500  /* Scan buttons every 500us (2000Hz) */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern USBD_HandleTypeDef hUsbDeviceFS;
volatile uint8_t composite_blink_enabled = 0;
static uint32_t composite_blink_last = 0;
static uint8_t composite_led_state = 0;
/* One-shot LED timers (milliseconds since HAL_GetTick()) */
volatile uint32_t composite_led_one_shot_until = 0;
volatile uint32_t cdc_led_one_shot_until = 0;
volatile uint32_t hid_led_one_shot_until = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  
  /* USER CODE BEGIN 2 */
  
  /* Ensure DFU pin (PD2) is LOW on startup to prevent accidental bootloader entry */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
  
  /* USER CODE END 2 */
  
  MX_USB_DEVICE_Init();
  // MX_ADC1_Init();  /* Disabled - ADC pins used as digital inputs for buttons */
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
  /* Load configuration from FLASH */
  if (FlashConfig_Load() != HAL_OK)
  {
    /* No valid config found, load defaults and save to FLASH */
    FlashConfig_LoadDefaults();
    FlashConfig_Save();
  }
  
#ifdef USE_KEYBOARD_MODE
  /* Initialize arcade keyboard system (NKRO USB HID mode) */
  Arcade_Init();
#elif defined(USE_JOYSTICK_MODE)
  /* Initialize arcade joystick system (Dual Joystick USB HID mode) */
  Joystick_Init();
#elif defined(USE_JVS_MODE)
  /* Initialize JVS protocol system (RS485 mode) */
  JVS_Init();
#endif
  
  /* Blink LED to indicate successful initialization */
  HAL_GPIO_WritePin(GPIOC, LED1_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOC, LED1_Pin, GPIO_PIN_RESET);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Composite continuous blink: toggle LED3 every 500ms when enabled */
    if (composite_blink_enabled)
    {
      uint32_t now = HAL_GetTick();
      if ((now - composite_blink_last) >= 500)
      {
        composite_blink_last = now;
        composite_led_state = !composite_led_state;
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, composite_led_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
      }
    }

    /* One-shot LED timers: turn LEDs off when their timer expires */
    {
      uint32_t now2 = HAL_GetTick();
      if (composite_led_one_shot_until && (now2 >= composite_led_one_shot_until))
      {
        composite_led_one_shot_until = 0;
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
      }
      if (cdc_led_one_shot_until && (now2 >= cdc_led_one_shot_until))
      {
        cdc_led_one_shot_until = 0;
        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
      }
      if (hid_led_one_shot_until && (now2 >= hid_led_one_shot_until))
      {
        hid_led_one_shot_until = 0;
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
      }
    }

#ifdef GPIO_TEST_MODE
    /* GPIO Diagnostic Mode - Test all pins and print to UART */
    GPIO_ContinuousTest();
    HAL_Delay(10);  /* Small delay to prevent UART overflow */
    
#elif defined(USE_KEYBOARD_MODE)
    /* USB HID Keyboard mode - High-speed button scanning with minimal latency */
    
    /* Scan buttons as fast as possible, but debounce handles timing */
    Arcade_ProcessButtons();
    
    /* Send HID report only if state changed (reduces USB traffic) */
    Arcade_SendKeyboardReport();
    
    /* No delay - run as fast as possible for minimal input latency!
     * USB will throttle automatically at 1ms intervals (1000Hz polling) */
     
#elif defined(USE_JOYSTICK_MODE)
    /* USB HID Joystick mode - Single joystick with 4 axes + 32 buttons */
    
    /* Scan all buttons and update joystick state */
    Joystick_ProcessButtons();
    
    /* Send combined joystick report (P1+P2) */
    Joystick_SendReport();
    
    /* No delay - USB polling handles timing (1000Hz) */
    
#elif defined(USE_JVS_MODE)
    /* JVS Protocol mode - RS485 communication */
    JVS_ProcessPackets();
    
    /* No delay needed, JVS_ProcessPackets has timeout handling */
#endif

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;  /* Explicitly disable LSE to free PC14/PC15 */
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
