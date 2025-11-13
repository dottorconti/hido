/**
  ******************************************************************************
  * @file    usb_commands.c
  * @brief   USB Vendor-Specific Commands Implementation
  ******************************************************************************
  */

#include "usb_commands.h"
#include "dfu_bootloader.h"
#include "flash_config.h"
#include "usbd_ctlreq.h"
#include "usbd_core.h"
#include <string.h>

/* Firmware version */
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  0
#define FIRMWARE_VERSION_PATCH  0

/* Buffer for config data transfer (large enough for both keyboard and joystick configs) */
static uint8_t config_buffer[1024];

/**
  * @brief  Process vendor-specific USB control transfer
  * @param  pdev: Device handle
  * @param  req: Setup request structure
  * @retval USBD_OK if handled, USBD_FAIL otherwise
  */
uint8_t USB_ProcessVendorCommand(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    uint8_t version_data[3];
    
    switch (req->bRequest)
    {
        case USB_REQ_ENTER_BOOTLOADER:
            /* Verify magic value for safety */
            if (req->wValue == BOOTLOADER_MAGIC)
            {
                /* Send ZLP (Zero Length Packet) to acknowledge */
                USBD_CtlSendData(pdev, NULL, 0);
                
                /* Small delay to ensure USB transaction completes */
                HAL_Delay(100);
                
                /* Enter DFU bootloader - this function does not return */
                DFU_EnterBootloader(100);
                
                /* Never reached */
                return USBD_OK;
            }
            else
            {
                /* Wrong magic value - reject request */
                USBD_CtlError(pdev, req);
                return USBD_FAIL;
            }
            break;
            
        case USB_REQ_GET_VERSION:
            /* Return firmware version as 3 bytes: [major][minor][patch] */
            version_data[0] = FIRMWARE_VERSION_MAJOR;
            version_data[1] = FIRMWARE_VERSION_MINOR;
            version_data[2] = FIRMWARE_VERSION_PATCH;
            USBD_CtlSendData(pdev, version_data, 3);
            return USBD_OK;
            break;
            
        case USB_REQ_RESET_DEVICE:
            /* Soft reset device */
            USBD_CtlSendData(pdev, NULL, 0);
            HAL_Delay(100);
            NVIC_SystemReset();
            /* Never reached */
            return USBD_OK;
            break;
            
        case USB_REQ_CONFIG_READ:
            /* Read current configuration and send to host */
            #ifdef USE_KEYBOARD_MODE
                KeyboardConfig_t *kb_config = FlashConfig_Get();
                memcpy(config_buffer, kb_config, sizeof(KeyboardConfig_t));
                USBD_CtlSendData(pdev, config_buffer, sizeof(KeyboardConfig_t));
            #else
                JoystickConfig_t *joy_config = FlashConfig_Get();
                memcpy(config_buffer, joy_config, sizeof(JoystickConfig_t));
                USBD_CtlSendData(pdev, config_buffer, sizeof(JoystickConfig_t));
            #endif
            return USBD_OK;
            break;
            
        case USB_REQ_CONFIG_WRITE:
            /* Receive new configuration from host */
            #ifdef USE_KEYBOARD_MODE
                if (req->wLength == sizeof(KeyboardConfig_t))
                {
                    /* Prepare to receive data */
                    USBD_CtlPrepareRx(pdev, config_buffer, req->wLength);
                    return USBD_OK;
                }
            #else
                if (req->wLength == sizeof(JoystickConfig_t))
                {
                    /* Prepare to receive data */
                    USBD_CtlPrepareRx(pdev, config_buffer, req->wLength);
                    return USBD_OK;
                }
            #endif
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
            break;
            
        case USB_REQ_CONFIG_RESET:
            /* Reset configuration to defaults */
            if (FlashConfig_Reset() == HAL_OK)
            {
                USBD_CtlSendData(pdev, NULL, 0);
                return USBD_OK;
            }
            else
            {
                USBD_CtlError(pdev, req);
                return USBD_FAIL;
            }
            break;
            
        default:
            /* Unknown vendor command */
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
    }
    
    return USBD_FAIL;
}

/**
  * @brief  Handle control transfer data OUT stage (for config write)
  * @param  pdev: Device handle
  * @retval USBD_OK if handled, USBD_FAIL otherwise
  */
uint8_t USB_ProcessVendorData(USBD_HandleTypeDef *pdev)
{
    /* Data received in config_buffer, now save to flash */
    #ifdef USE_KEYBOARD_MODE
        KeyboardConfig_t *kb_config = FlashConfig_Get();
        memcpy(kb_config, config_buffer, sizeof(KeyboardConfig_t));
    #else
        JoystickConfig_t *joy_config = FlashConfig_Get();
        memcpy(joy_config, config_buffer, sizeof(JoystickConfig_t));
    #endif
    
    /* Save to flash */
    if (FlashConfig_Save() == HAL_OK)
    {
        return USBD_OK;
    }
    
    return USBD_FAIL;
}
