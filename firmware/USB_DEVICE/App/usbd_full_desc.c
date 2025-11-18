/* Combined configuration descriptor (HID custom + HID raw + CDC) */
#include "usbd_desc.h"
#include "usbd_hid.h"
#include <stdint.h>

/* Fallback defines in case the include macros are not available in this TU */
#ifndef HID_CUSTOM_REPORT_DESC_SIZE
#define HID_CUSTOM_REPORT_DESC_SIZE   0x3FU
#endif
#ifndef HID_EPOUT_ADDR
#define HID_EPOUT_ADDR                0x01U
#endif
#ifndef HID_EPOUT_SIZE
#define HID_EPOUT_SIZE                0x40U
#endif
#ifndef HID_RAW_REPORT_DESC_SIZE
#define HID_RAW_REPORT_DESC_SIZE      26U
#endif
#ifndef HID_RAW_EPIN_ADDR
#define HID_RAW_EPIN_ADDR             0x82U
#endif
#ifndef HID_RAW_EPIN_SIZE
#define HID_RAW_EPIN_SIZE             0x40U
#endif
#ifndef HID_RAW_EPOUT_ADDR
#define HID_RAW_EPOUT_ADDR            0x02U
#endif
#ifndef HID_RAW_EPOUT_SIZE
#define HID_RAW_EPOUT_SIZE            0x40U
#endif

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t USBD_FULL_FS_CfgDesc[] __ALIGN_END =
{
  /* Configuration Descriptor */
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  0x8B, 0x00, /* wTotalLength: 139 bytes (HID+HID_RAW+CDC) */
  0x04,   /* bNumInterfaces: 4 interfaces (HID0,HID1,CDC ctrl,CDC data) */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xE0,   /* bmAttributes: bus powered, remote wakeup */
  0x32,   /* MaxPower 100 mA */

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
  /* CDC IAD + Control Interface (2) + Data Interface (3) */
  /* IAD */
  0x08, /* bLength */
  0x0B, /* bDescriptorType: IAD */
  0x02, /* bFirstInterface: interface 2 */
  0x02, /* bInterfaceCount: 2 (control + data) */
  0x02, /* bFunctionClass: CDC */
  0x02, /* bFunctionSubClass: ACM */
  0x01, /* bFunctionProtocol: AT cmds */
  0x00, /* iFunction */

  /* Interface 2: CDC Control */
  0x09, /* bLength: Interface Descriptor size */
  0x04, /* bDescriptorType: Interface */
  0x02, /* bInterfaceNumber: 2 */
  0x00, /* bAlternateSetting */
  0x01, /* bNumEndpoints: 1 (notification) */
  0x02, /* bInterfaceClass: Communication Interface Class */
  0x02, /* bInterfaceSubClass: Abstract Control Model */
  0x01, /* bInterfaceProtocol: Common AT commands */
  0x00, /* iInterface */

  /* CDC Header Functional Descriptor */
  0x05, 0x24, 0x00, 0x10, 0x01,
  /* CDC Call Management Functional Descriptor */
  0x05, 0x24, 0x01, 0x00, 0x03,
  /* CDC ACM Functional Descriptor */
  0x04, 0x24, 0x02, 0x02,
  /* CDC Union Functional Descriptor */
  0x05, 0x24, 0x06, 0x02, 0x03,

  /* Notification Endpoint (Interrupt IN) */
  0x07, /* bLength: Endpoint Descriptor size */
  0x05, /* bDescriptorType: Endpoint */
  0x83, /* bEndpointAddress: IN endpoint 3 */
  0x03, /* bmAttributes: Interrupt */
  0x10, 0x00, /* wMaxPacketSize: 16 */
  0x10, /* bInterval */

  /* Interface 3: CDC Data */
  0x09, /* bLength: Interface Descriptor size */
  0x04, /* bDescriptorType: Interface */
  0x03, /* bInterfaceNumber: 3 */
  0x00, /* bAlternateSetting */
  0x02, /* bNumEndpoints: 2 (bulk) */
  0x0A, /* bInterfaceClass: Data Interface Class */
  0x00, /* bInterfaceSubClass */
  0x00, /* bInterfaceProtocol */
  0x00, /* iInterface */

  /* Endpoint OUT (Bulk) */
  0x07, /* bLength: Endpoint Descriptor size */
  0x05, /* bDescriptorType: Endpoint */
  0x03, /* bEndpointAddress: OUT endpoint 3 */
  0x02, /* bmAttributes: Bulk */
  0x40, 0x00, /* wMaxPacketSize: 64 */
  0x00, /* bInterval */

  /* Endpoint IN (Bulk) */
  0x07, /* bLength: Endpoint Descriptor size */
  0x05, /* bDescriptorType: Endpoint */
  0x84, /* bEndpointAddress: IN endpoint 4 */
  0x02, /* bmAttributes: Bulk */
  0x40, 0x00, /* wMaxPacketSize: 64 */
  0x00 /* bInterval */
};

uint16_t USBD_FULL_FS_CfgDescSize = sizeof(USBD_FULL_FS_CfgDesc);
