/**
  ******************************************************************************
  * @file    dfu_bootloader.h
  * @brief   DFU Bootloader Entry Control
  * @author  HIDO Project
  * @date    November 13, 2025
  ******************************************************************************
  * @attention
  *
  * This module controls entry into STM32 USB DFU bootloader for firmware updates.
  * 
  * Hardware:
  * - PD2 (output) -> R (1k-2.2kΩ recommended) -> BOOT0 + capacitor (1-4.7µF) to GND
  * - When PD2=HIGH, capacitor charges through R and holds BOOT0 HIGH during reset
  * - BOOT0 pin held HIGH during reset -> enters DFU bootloader
  * - Lower R = faster charge, higher current (use 1kΩ for ~3.3mA, safe for GPIO)
  * 
  * Usage:
  * 1. Call DFU_EnterBootloader() from USB command or button press
  * 2. Firmware sets PD2 HIGH, waits for capacitor charge
  * 3. System reset with BOOT0 HIGH -> STM32 DFU mode
  * 4. PC detects DFU device (VID:0483 PID:DF11)
  * 5. Flash new firmware using dfu-util or STM32CubeProgrammer
  * 
  ******************************************************************************
  */

#ifndef __DFU_BOOTLOADER_H
#define __DFU_BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdbool.h>

/* Boot control pin configuration */
#define BOOT_CONTROL_PIN        GPIO_PIN_2
#define BOOT_CONTROL_PORT       GPIOD
#define BOOT_CONTROL_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

/* Capacitor charge delay (milliseconds) 
 * RC circuit: PD2 -> R -> BOOT0 + Cap to GND
 * 
 * Recommended component values:
 *   R = 1kΩ - 2.2kΩ (limits current to ~1.5-3.3mA)
 *   C = 1µF - 4.7µF (holds BOOT0 during reset)
 * 
 * Time constant τ = R × C
 * Charge time = 5τ (99% charge)
 * 
 * Examples:
 *   R=1kΩ,   C=1µF   → τ=1ms    → 5τ=5ms   (recommended)
 *   R=2.2kΩ, C=1µF   → τ=2.2ms  → 5τ=11ms  (safer)
 *   R=10kΩ,  C=1µF   → τ=10ms   → 5τ=50ms  (too slow)
 * 
 * Current PCB: R=1kΩ, C=1µF → τ=1ms → 5τ=5ms charge time
 */
#define BOOT_CHARGE_DELAY_MS    10  // 2× charge time (5ms) for safety margin

/* Magic value stored in RAM to indicate intentional bootloader entry */
#define DFU_MAGIC_VALUE         0xDEADBEEF

/**
  * @brief  Initialize bootloader control GPIO
  * @note   Call this in main initialization, after GPIO clock enable
  * @retval None
  */
void DFU_Init(void);

/**
  * @brief  Enter STM32 USB DFU bootloader
  * @note   This function does not return - system resets
  * @param  delay_ms: Delay before reset (for USB cleanup)
  * @retval None (system resets)
  */
void DFU_EnterBootloader(uint32_t delay_ms);

/**
  * @brief  Check if reset was caused by bootloader entry request
  * @note   Call early in main() to detect DFU entry attempts
  * @retval true if DFU was requested, false otherwise
  */
bool DFU_WasRequested(void);

#ifdef __cplusplus
}
#endif

#endif /* __DFU_BOOTLOADER_H */
