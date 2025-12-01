/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : App/usbd_desc.c
  * @version        : v2.0_Cube
  * @brief          : This file implements the USB device descriptors.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>

 
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : App/usbd_desc.c
  * @version        : v2.0_Cube
  * @brief          : This file implements the USB device descriptors.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

/* USER CODE BEGIN INCLUDE */
#include "usbd_hid.h"  /* For mode selection defines */
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/


/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {0};
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @addtogroup USBD_DESC
  * @{
  */

/** @defgroup USBD_DESC_Private_TypesDefinitions USBD_DESC_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_DESC_Private_Defines USBD_DESC_Private_Defines
  * @brief Private defines.
  * @{
  */

#define USBD_VID     1155
#define USBD_LANGID_STRING     1040
#define USBD_MANUFACTURER_STRING     "HIDO Project"
#define USBD_PID_FS     22315

#ifdef USE_KEYBOARD_MODE
  #define USBD_PRODUCT_STRING_FS     "HIDO Arcade Keyboard"
#elif defined(USE_JOYSTICK_MODE)
  #define USBD_PRODUCT_STRING_FS     "HIDO Arcade Joystick"
#elif defined(USE_JVS_MODE)
  #define USBD_PRODUCT_STRING_FS     "HIDO JVS Interface"
#else
  #define USBD_PRODUCT_STRING_FS     "HIDO HID Device"
#endif

#define USBD_CONFIGURATION_STRING_FS     "HID Config"
#define USBD_INTERFACE_STRING_FS     "HID Interface"
#define USBD_INTERFACE2_STRING_FS    "HIDO Config"


/* HID RAW rimosso */

// Assicurati che le macro per la prima interfaccia (custom) esistano
#ifndef HID_CUSTOM_REPORT_DESC_SIZE
#define HID_CUSTOM_REPORT_DESC_SIZE   0x3F
#endif
#ifndef HID_EPOUT_ADDR
#define HID_EPOUT_ADDR                0x01
#endif
#ifndef HID_EPOUT_SIZE
#define HID_EPOUT_SIZE                0x40
#endif


// Le define per la prima interfaccia (custom) sono già presenti in usbd_hid.h
// Non ridefinire qui per evitare conflitti
// Include per tipi standard
#include <stdint.h>

/* HID RAW rimosso duplicato */

// Buffer per le stringhe USB
// Endpoint e dimensioni per interfaccia custom (prima interfaccia)
// #define HID_EPIN_ADDR                 0x81
// #define HID_EPOUT_ADDR                0x01
// #define HID_EPIN_SIZE                 0x40
// #define HID_EPOUT_SIZE                0x40
// #define HID_CUSTOM_REPORT_DESC_SIZE   0x3F

// Buffer per le stringhe USB
#define USB_MAX_STR_DESC_SIZ 64
__ALIGN_BEGIN uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ] __ALIGN_END = {0};

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/** @defgroup USBD_DESC_Private_Macros USBD_DESC_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_DESC_Private_FunctionPrototypes USBD_DESC_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static void Get_SerialNum(void);
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len);

/**
  * @}
  */

/** @defgroup USBD_DESC_Private_FunctionPrototypes USBD_DESC_Private_FunctionPrototypes
  * @brief Private functions declaration for FS.
  * @{
  */

uint8_t * USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

/**
  * @}
  */

/** @defgroup USBD_DESC_Private_Variables USBD_DESC_Private_Variables
  * @brief Private variables.
  * @{
  */

USBD_DescriptorsTypeDef FS_Desc =
{
  USBD_FS_DeviceDescriptor
, USBD_FS_LangIDStrDescriptor
, USBD_FS_ManufacturerStrDescriptor
, USBD_FS_ProductStrDescriptor
, USBD_FS_SerialStrDescriptor
, USBD_FS_ConfigStrDescriptor
, USBD_FS_InterfaceStrDescriptor
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
/** USB standard device descriptor. */
__ALIGN_BEGIN uint8_t USBD_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
  0x12,                       /*bLength */
  USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
  0x00,                       /*bcdUSB */
  0x02,
  0xEF,                       /*bDeviceClass: Composite (IAD)*/
  0x02,                       /*bDeviceSubClass: Common Class*/
  0x01,                       /*bDeviceProtocol: IAD Protocol*/
  USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
  LOBYTE(USBD_VID),           /*idVendor*/
  HIBYTE(USBD_VID),           /*idVendor*/
  LOBYTE(USBD_PID_FS),        /*idProduct*/
  HIBYTE(USBD_PID_FS),        /*idProduct*/
  0x00,                       /*bcdDevice rel. 2.00*/
  0x02,
  USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
  USBD_IDX_PRODUCT_STR,       /*Index of product string*/
  USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
  USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
};

/* USB_DeviceDescriptor */

/**
  * @}
  */

/** @defgroup USBD_DESC_Private_Variables USBD_DESC_Private_Variables
  * @brief Private variables.
  * @{
  */

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */

/** USB lang identifier descriptor. */
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
     USB_LEN_LANGID_STR_DESC,
     USB_DESC_TYPE_STRING,
     LOBYTE(USBD_LANGID_STRING),
     HIBYTE(USBD_LANGID_STRING)
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
/* Internal string descriptor. */
/* Fallback descriptor HID-only (senza RAW) usato solo se full composite non disponibile */
__ALIGN_BEGIN uint8_t USBD_FS_CfgDesc[]  __ALIGN_END =
{
  /* Configuration Descriptor */
  0x09,
  USB_DESC_TYPE_CONFIGURATION,
  0x22, 0x00, /* lunghezza totale minima */
  0x01, /* 1 interfaccia */
  0x01,
  0x00,
  0xE0,
  0x32,
  /* Interface 0 HID */
  0x09, USB_DESC_TYPE_INTERFACE,
  0x00, 0x00, 0x01,
  0x03, 0x00, 0x00,
  0x00,
  /* HID Descriptor */
  0x09, HID_DESCRIPTOR_TYPE,
  0x11, 0x01, 0x00,
  0x01, 0x22,
  0x01, 0x00, /* placeholder report len =1 */
  /* Endpoint IN */
  0x07, USB_DESC_TYPE_ENDPOINT,
  HID_EPIN_ADDR, 0x03,
  HID_EPIN_SIZE, 0x00,
  HID_FS_BINTERVAL
};

/* Provide the size of the FS configuration descriptor for composite fallback */
uint16_t USBD_FS_CfgDescSize = sizeof(USBD_FS_CfgDesc);

/**
  * @brief  Return the manufacturer string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/**
  * @brief  Return the serial number string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = USB_SIZ_STRING_SERIAL;

  /* Update the serial number string descriptor with the data from the unique
   * ID */
  Get_SerialNum();
  /* USER CODE BEGIN USBD_FS_SerialStrDescriptor */
  
  /* USER CODE END USBD_FS_SerialStrDescriptor */
  return (uint8_t *) USBD_StringSerial;
}

/**
  * @brief  Return the configuration string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t * USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if(speed == USBD_SPEED_HIGH)
  {
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING_FS, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING_FS, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

/**
  * @brief  Return the interface string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */

// Return interface string descriptor (index 0 = main, 1 = config)
uint8_t * USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  // Default: interface 0
  USBD_GetString((uint8_t *)USBD_INTERFACE_STRING_FS, USBD_StrDesc, length);
  return USBD_StrDesc;
}

// New: interface 1 (HIDO Config)
uint8_t * USBD_FS_Interface2StrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  USBD_GetString((uint8_t *)USBD_INTERFACE2_STRING_FS, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/**
  * @brief  Create the serial number string descriptor
  * @param  None
  * @retval None
  */
static void Get_SerialNum(void)
{
  uint32_t deviceserial0;
  uint32_t deviceserial1;
  uint32_t deviceserial2;

  /* Initialize string descriptor header */
  USBD_StringSerial[0] = USB_SIZ_STRING_SERIAL;
  USBD_StringSerial[1] = USB_DESC_TYPE_STRING;

  deviceserial0 = *(uint32_t *) DEVICE_ID1;
  deviceserial1 = *(uint32_t *) DEVICE_ID2;
  deviceserial2 = *(uint32_t *) DEVICE_ID3;

  deviceserial0 += deviceserial2;

  if (deviceserial0 != 0)
  {
    IntToUnicode(deviceserial0, &USBD_StringSerial[2], 8);
    IntToUnicode(deviceserial1, &USBD_StringSerial[18], 4);
  }
}

/**
  * @brief  Convert Hex 32Bits value into char
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len)
{
  uint8_t idx = 0;

  for (idx = 0; idx < len; idx++)
  {
    if (((value >> 28)) < 0xA)
    {
      pbuf[2 * idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[2 * idx + 1] = 0;
  }
}

/* Implementazioni richieste dal linker */
uint8_t * USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_FS_DeviceDesc);
  return (uint8_t*)USBD_FS_DeviceDesc;
}

uint8_t * USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_LangIDDesc);
  return (uint8_t*)USBD_LangIDDesc;
}

uint8_t * USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  USBD_GetString((uint8_t *)USBD_PRODUCT_STRING_FS, USBD_StrDesc, length);
  return USBD_StrDesc;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

