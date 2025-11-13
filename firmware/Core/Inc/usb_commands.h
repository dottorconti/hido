/**
  ******************************************************************************
  * @file    usb_commands.h
  * @brief   USB Vendor-Specific Commands for HIDO
  * @author  HIDO Project
  * @date    November 13, 2025
  ******************************************************************************
  * @attention
  *
  * This module handles custom USB commands sent from PC tools.
  * 
  * Supported Commands:
  * - BOOTLOADER_ENTRY: Reset device into DFU bootloader mode
  * 
  * Protocol: USB Vendor-Specific Control Transfer
  * - bmRequestType: 0x40 (Host-to-Device, Vendor, Device)
  * - bRequest: Command code
  * - wValue: Command-specific parameter
  * - wIndex: 0
  * - Data: None
  * 
  ******************************************************************************
  */

#ifndef __USB_COMMANDS_H
#define __USB_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "usbd_def.h"

/* USB Vendor Request Codes */
#define USB_REQ_ENTER_BOOTLOADER    0xBB    /* Request DFU bootloader entry */
#define USB_REQ_GET_VERSION         0xAA    /* Get firmware version */
#define USB_REQ_RESET_DEVICE        0xCC    /* Soft reset device */
#define USB_REQ_CONFIG_READ         0xC0    /* Read configuration */
#define USB_REQ_CONFIG_WRITE        0xC1    /* Write configuration */
#define USB_REQ_CONFIG_RESET        0xC2    /* Reset configuration to defaults */

/* Magic value for bootloader entry confirmation */
#define BOOTLOADER_MAGIC            0xB007  /* wValue must match this */

/**
  * @brief  Process vendor-specific USB control transfer
  * @param  pdev: Device handle
  * @param  req: Setup request structure
  * @retval USBD_OK if handled, USBD_FAIL otherwise
  */
uint8_t USB_ProcessVendorCommand(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

/**
  * @brief  Handle control transfer data OUT stage
  * @param  pdev: Device handle
  * @retval USBD_OK if handled, USBD_FAIL otherwise
  */
uint8_t USB_ProcessVendorData(USBD_HandleTypeDef *pdev);

#ifdef __cplusplus
}
#endif

#endif /* __USB_COMMANDS_H */
