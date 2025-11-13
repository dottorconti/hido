/**
  ******************************************************************************
  * @file    usb_commands.c
  * @brief   USB Vendor-Specific Commands Implementation
  ******************************************************************************
  */

#include "usb_commands.h"
#include "dfu_bootloader.h"
#include "usbd_ctlreq.h"
#include "usbd_core.h"

/* Firmware version */
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  0
#define FIRMWARE_VERSION_PATCH  0

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
            
        default:
            /* Unknown vendor command */
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
    }
    
    return USBD_FAIL;
}
