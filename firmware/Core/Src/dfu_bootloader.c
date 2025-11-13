/**
  ******************************************************************************
  * @file    dfu_bootloader.c
  * @brief   DFU Bootloader Entry Control Implementation
  ******************************************************************************
  */

#include "dfu_bootloader.h"
#include "stm32f1xx_hal.h"

/* RAM variable to persist across reset (place in no-init section) */
__attribute__((section(".noinit")))
static uint32_t dfu_magic;

/**
  * @brief  Initialize bootloader control GPIO
  */
void DFU_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIO clock */
    BOOT_CONTROL_CLK_ENABLE();
    
    /* Configure PD2 as output, initially LOW */
    GPIO_InitStruct.Pin = BOOT_CONTROL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BOOT_CONTROL_PORT, &GPIO_InitStruct);
    
    /* Ensure BOOT0 control is LOW (normal operation) */
    HAL_GPIO_WritePin(BOOT_CONTROL_PORT, BOOT_CONTROL_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Check if DFU bootloader was requested
  */
bool DFU_WasRequested(void)
{
    if (dfu_magic == DFU_MAGIC_VALUE) {
        dfu_magic = 0; /* Clear flag */
        return true;
    }
    return false;
}

/**
  * @brief  Enter STM32 USB DFU bootloader
  * @note   Steps:
  *         1. Set magic value in RAM (persists across reset)
  *         2. Set PD2 HIGH -> current flows through 10kΩ -> charges capacitor
  *         3. Wait for capacitor charge (RC time constant)
  *         4. Trigger system reset via NVIC
  *         5. Capacitor holds BOOT0 HIGH during reset
  *         6. STM32 samples BOOT0 at boot -> sees HIGH -> enters DFU mode
  */
void DFU_EnterBootloader(uint32_t delay_ms)
{
    /* Disable interrupts to prevent interference */
    __disable_irq();
    
    /* Set magic value to indicate intentional bootloader entry */
    dfu_magic = DFU_MAGIC_VALUE;
    
    /* Set BOOT0 control pin HIGH to charge capacitor */
    HAL_GPIO_WritePin(BOOT_CONTROL_PORT, BOOT_CONTROL_PIN, GPIO_PIN_SET);
    
    /* Wait for capacitor to charge and BOOT0 to reach HIGH level */
    HAL_Delay(delay_ms + BOOT_CHARGE_DELAY_MS);
    
    /* Trigger system reset via NVIC */
    /* When STM32 resets with BOOT0=HIGH, it enters USB DFU bootloader */
    NVIC_SystemReset();
    
    /* Never reached - system resets above */
    while(1);
}

/**
  * @brief  Alternative: Jump directly to bootloader without reset
  * @note   More complex - requires disabling all peripherals
  *         Not recommended for STM32F1 - use DFU_EnterBootloader() instead
  */
#if 0
void DFU_JumpToBootloader(void)
{
    /* STM32F102 System Memory (bootloader) start address */
    #define BOOTLOADER_ADDRESS  0x1FFFF000
    
    /* Function pointer for bootloader entry */
    void (*bootloader)(void) = (void (*)(void))(*((uint32_t*)(BOOTLOADER_ADDRESS + 4)));
    
    /* Disable all interrupts */
    __disable_irq();
    
    /* Disable SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    
    /* Remap system memory to 0x0000 0000 */
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    
    /* Set main stack pointer to bootloader's MSP */
    __set_MSP(*((uint32_t*)BOOTLOADER_ADDRESS));
    
    /* Jump to bootloader */
    bootloader();
    
    /* Never reached */
    while(1);
}
#endif
