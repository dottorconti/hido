/**
  ******************************************************************************
  * @file           : gpio_test.c
  * @brief          : GPIO Pin Diagnostic Tool
  ******************************************************************************
  * Questo file serve per testare la lettura dei pin GPIO
  * Stampa lo stato di tutti i pin sulla UART per debug
  ******************************************************************************
  */

#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/* Buffer per printf via UART */
static char uart_buffer[256];

/**
  * @brief  Invia stringa via UART1
  */
static void UART_Print(const char* str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/**
  * @brief  Test diagnostico di tutti i pin dei player
  */
void GPIO_TestAllPins(void)
{
    UART_Print("\r\n========== GPIO PIN TEST ==========\r\n");
    
    /* Test Player 1 */
    UART_Print("\r\n--- PLAYER 1 ---\r\n");
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_UP (PA1):     %s\r\n", 
             HAL_GPIO_ReadPin(P1_UP_GPIO_Port, P1_UP_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_DOWN (PA0):   %s\r\n", 
             HAL_GPIO_ReadPin(P1_DOWN_GPIO_Port, P1_DOWN_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN1 (PC1):   %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN1_GPIO_Port, P1_BTN1_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN2 (PC0):   %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN2_GPIO_Port, P1_BTN2_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN3 (PC15):  %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN3_GPIO_Port, P1_BTN3_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN4 (PC14):  %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN4_GPIO_Port, P1_BTN4_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN5 (PC13):  %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN5_GPIO_Port, P1_BTN5_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN6 (PB9):   %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN6_GPIO_Port, P1_BTN6_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN7 (PB8):   %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN7_GPIO_Port, P1_BTN7_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_BTN8 (PB7):   %s\r\n", 
             HAL_GPIO_ReadPin(P1_BTN8_GPIO_Port, P1_BTN8_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P1_START (PA15): %s\r\n", 
             HAL_GPIO_ReadPin(P1_START_GPIO_Port, P1_START_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    /* Test Player 2 */
    UART_Print("\r\n--- PLAYER 2 ---\r\n");
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P2_UP (PC6):     %s\r\n", 
             HAL_GPIO_ReadPin(P2_UP_GPIO_Port, P2_UP_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P2_DOWN (PC7):   %s\r\n", 
             HAL_GPIO_ReadPin(P2_DOWN_GPIO_Port, P2_DOWN_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P2_BTN1 (PA7):   %s\r\n", 
             HAL_GPIO_ReadPin(P2_BTN1_GPIO_Port, P2_BTN1_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P2_BTN2 (PC4):   %s\r\n", 
             HAL_GPIO_ReadPin(P2_BTN2_GPIO_Port, P2_BTN2_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P2_BTN3 (PC5):   %s\r\n", 
             HAL_GPIO_ReadPin(P2_BTN3_GPIO_Port, P2_BTN3_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    snprintf(uart_buffer, sizeof(uart_buffer), "P2_START (PA6):  %s\r\n", 
             HAL_GPIO_ReadPin(P2_START_GPIO_Port, P2_START_Pin) == GPIO_PIN_RESET ? "LOW (Pressed)" : "HIGH");
    UART_Print(uart_buffer);
    
    UART_Print("\r\n===================================\r\n\r\n");
}

/**
  * @brief  Test continuo - legge i pin in loop
  */
void GPIO_ContinuousTest(void)
{
    static uint32_t last_test_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    /* Test ogni 500ms */
    if ((current_time - last_test_time) >= 500) {
        GPIO_TestAllPins();
        last_test_time = current_time;
    }
}
