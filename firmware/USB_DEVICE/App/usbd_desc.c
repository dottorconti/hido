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


// Endpoint e dimensioni per interfaccia RAW (seconda interfaccia)
#define HID_RAW_EPIN_ADDR             0x82
#define HID_RAW_EPOUT_ADDR            0x02
#define HID_RAW_EPIN_SIZE             0x40
#define HID_RAW_EPOUT_SIZE            0x40
#define HID_RAW_REPORT_DESC_SIZE      0x41 // 65 byte, vedi usbd_hid_raw.h

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

// Endpoint e dimensioni per interfaccia RAW (seconda interfaccia)
#define HID_RAW_EPIN_ADDR             0x82
#define HID_RAW_EPOUT_ADDR            0x02
#define HID_RAW_EPIN_SIZE             0x40
#define HID_RAW_EPOUT_SIZE            0x40
#define HID_RAW_REPORT_DESC_SIZE      0x41 // 65 byte, vedi usbd_hid_raw.h

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
  0x00,                       /*bDeviceClass*/
  0x00,                       /*bDeviceSubClass*/
  0x00,                       /*bDeviceProtocol*/
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
// Nuovo descrittore di configurazione con 2 interfacce HID (standard + RAW)
__ALIGN_BEGIN uint8_t USBD_FS_CfgDesc[]  __ALIGN_END =
{
  /* Configuration Descriptor */
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  0x8B, 0x00, /* wTotalLength: 139 bytes (config + 2 HID interfaces + CDC comm+data) */
  0x04,   /* bNumInterfaces: 4 interfaces (HID std, HID raw, CDC comm, CDC data) */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xE0,   /* bmAttributes: bus powered, remote wakeup */
  0x32,   /* MaxPower 100 mA: this current is used for detecting Vbus */

  /*---------------------------------------------------------------------------*/
  /* Interface 0: HID standard (joystick/keyboard) */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface descriptor type */
  0x00,   /* bInterfaceNumber: 0 */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints */
  0x03,   /* bInterfaceClass: HID */
  0x00,   /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,   /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0x00,   /* iInterface: */

  /* HID Descriptor (standard) */
  0x09,   /* bLength: HID Descriptor size */
  HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
  0x11,   /* bcdHID: HID Class Spec release number */
  0x01,
  0x00,   /* bCountryCode: Hardware target country */
  0x01,   /* bNumDescriptors: Number of HID class descriptors to follow */
  0x22,   /* bDescriptorType */
  HID_CUSTOM_REPORT_DESC_SIZE,/* wItemLength: Total length of Report descriptor */
  0x00,

  /* Endpoint Descriptor IN (standard) */
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
  HID_EPIN_ADDR,     /* bEndpointAddress: Endpoint Address (IN) */
  0x03,   /* bmAttributes: Interrupt endpoint */
  HID_EPIN_SIZE,     /* wMaxPacketSize: */
  0x00,
  HID_FS_BINTERVAL,          /* bInterval: */

  /* Endpoint Descriptor OUT (standard) */
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
  HID_EPOUT_ADDR,    /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,   /* bmAttributes: Interrupt endpoint */
  HID_EPOUT_SIZE,    /* wMaxPacketSize: */
  0x00,
  HID_FS_BINTERVAL,          /* bInterval: */

  /*---------------------------------------------------------------------------*/
  /* Interface 1: HID RAW (config) */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface descriptor type */
  0x01,   /* bInterfaceNumber: 1 */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints */
  0x03,   /* bInterfaceClass: HID */
  0x00,   /* bInterfaceSubClass : 0 */
  0x00,   /* nInterfaceProtocol : 0 */
  0x00,   /* iInterface: no string */

  /* HID Descriptor (RAW) */
  0x09,   /* bLength: HID Descriptor size */
  HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
  0x11,   /* bcdHID: HID Class Spec release number */
  0x01,
  0x00,   /* bCountryCode: Hardware target country */
  0x01,   /* bNumDescriptors: Number of HID class descriptors to follow */
  0x22,   /* bDescriptorType */
  HID_RAW_REPORT_DESC_SIZE,/* wItemLength: Total length of Report descriptor (RAW) */
  0x00,

  /* Endpoint Descriptor IN (RAW) */
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
  HID_RAW_EPIN_ADDR,     /* bEndpointAddress: Endpoint Address (IN) */
  0x03,   /* bmAttributes: Interrupt endpoint */
  HID_RAW_EPIN_SIZE,     /* wMaxPacketSize: */
  0x00,
  HID_FS_BINTERVAL,          /* bInterval: */

  /* Endpoint Descriptor OUT (RAW) */
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
  HID_RAW_EPOUT_ADDR,    /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,   /* bmAttributes: Interrupt endpoint */
  HID_RAW_EPOUT_SIZE,    /* wMaxPacketSize: */
  0x00,
  HID_FS_BINTERVAL,          /* bInterval: */

  /*---------------------------------------------------------------------------*/
  /* CDC Interface Association Descriptor (IAD) */
  0x08,   /* bLength */
  0x0B,   /* bDescriptorType: IAD */
  0x02,   /* bFirstInterface */
  0x02,   /* bInterfaceCount */
  0x02,   /* bFunctionClass: CDC */
  0x02,   /* bFunctionSubClass */
  0x01,   /* bFunctionProtocol */
  0x00,   /* iFunction */

  /* Interface 2: CDC Communication Interface */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface descriptor type */
  0x02,   /* bInterfaceNumber: 2 */
  0x00,   /* bAlternateSetting: 0 */
  0x01,   /* bNumEndpoints: 1 (notification) */
  0x02,   /* bInterfaceClass: CDC */
  0x02,   /* bInterfaceSubClass : Abstract Control Model */
  0x01,   /* bInterfaceProtocol : Common AT commands */
  0x00,   /* iInterface: */

  /* CDC Header Functional Descriptor */
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10, 0x01, /* bcdCDC: 1.10 */

  /* CDC ACM Functional Descriptor */
  0x04,
  0x24,
  0x02,
  0x02,

  /* CDC Union Functional Descriptor */
  0x05,
  0x24,
  0x06,
  0x02, /* bMasterInterface: comm (2) */
  0x03, /* bSlaveInterface0: data (3) */

  /* Endpoint Descriptor (Notification) */
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,
  0x83,   /* bEndpointAddress: IN 3 (interrupt) */
  0x03,   /* bmAttributes: Interrupt */
  0x08, 0x00, /* wMaxPacketSize: 8 */
  0xFF,   /* bInterval (unused placeholder) */

  /* Interface 3: CDC Data Interface */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface descriptor type */
  0x03,   /* bInterfaceNumber: 3 */
  0x00,   /* bAlternateSetting: 0 */
  0x02,   /* bNumEndpoints: 2 */
  0x0A,   /* bInterfaceClass: Data */
  0x00,   /* bInterfaceSubClass : */
  0x00,   /* bInterfaceProtocol : */
  0x00,   /* iInterface: */

  /* Endpoint Descriptor OUT (CDC Data) */
  0x07,
  USB_DESC_TYPE_ENDPOINT,
  0x04,   /* bEndpointAddress: OUT 4 */
  0x02,   /* bmAttributes: Bulk */
  0x40, 0x00, /* wMaxPacketSize: 64 */
  0x00,

  /* Endpoint Descriptor IN (CDC Data) */
  0x07,
  USB_DESC_TYPE_ENDPOINT,
  0x84,   /* bEndpointAddress: IN 4 */
  0x02,   /* bmAttributes: Bulk */
  0x40, 0x00, /* wMaxPacketSize: 64 */
  0x00,
};

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

