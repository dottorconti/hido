/**
  ******************************************************************************
  * @file    usbd_hid_custom.c
  * @author  HIDO Project
  * @brief   Custom HID descriptor implementation for dual arcade joystick
  *          This file contains custom HID report descriptors
  ******************************************************************************
  */

#include "usbd_hid_custom.h"

#ifdef USE_JOYSTICK_MODE

/**
  * @brief  Custom HID Report Descriptor for Dual Arcade Joystick
  *         - TWO separate Application Collections = 2 devices in Windows
  *         - Player 1: Report ID 1, 2 axes + 13 buttons
  *         - Player 2: Report ID 2, 2 axes + 13 buttons
  *         - Report structure: 5 bytes [ReportID][X][Y][BtnLow][BtnHigh]
  *         - Windows sees: 2 separate joystick devices
  *         - Descriptor size: 98 bytes
  */
/* Combined HID Report Descriptor:
   - Player 1: Report ID = 1, layout [ReportID][X][Y][BtnLow][BtnHigh]
       (2 axes 8-bit + 13 buttons packed into 2 bytes)
   - Player 2: Report ID = 2, same layout as Player 1
   - Arduino-compatible device: Report ID = 3, 30 buttons packed into 4 bytes

   The array length is HID_REPORT_DESC_SIZE_CUSTOM; remaining bytes are
   zero-padded by C initialization rules. Keep descriptor order consistent
   with how `JoystickReport_t` and helper send functions build report buffers.
*/
const uint8_t HID_JOYSTICK_ReportDesc_Custom[HID_REPORT_DESC_SIZE_CUSTOM] = {
  /* --- Player 1 (Report ID 1): X, Y (8-bit), 13 buttons (bits 0-12) --- */
  0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,       // USAGE (Game Pad)
  0xA1, 0x01,       // COLLECTION (Application)
    0x85, 0x01,     //   REPORT_ID (1)
    /* Axes X,Y */
    0x09, 0x30,     //   USAGE (X)
    0x09, 0x31,     //   USAGE (Y)
    0x15, 0x00,     //   LOGICAL_MINIMUM (0)
    0x25, 0xFF,     //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,     //   REPORT_SIZE (8)
    0x95, 0x02,     //   REPORT_COUNT (2)
    0x81, 0x02,     //   INPUT (Data,Var,Abs)
    /* Buttons (13 bits) */
    0x05, 0x09,     //   USAGE_PAGE (Button)
    0x19, 0x01,     //   USAGE_MINIMUM (Button 1)
    0x29, 0x0D,     //   USAGE_MAXIMUM (Button 13)
    0x15, 0x00,     //   LOGICAL_MINIMUM (0)
    0x25, 0x01,     //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,     //   REPORT_SIZE (1)
    0x95, 0x0D,     //   REPORT_COUNT (13)
    0x81, 0x02,     //   INPUT (Data,Var,Abs)
    /* Padding bits to align to 16-bit button field (3 bits) */
    0x95, 0x03,     //   REPORT_COUNT (3)
    0x75, 0x01,     //   REPORT_SIZE (1)
    0x81, 0x03,     //   INPUT (Const)
  0xC0,             // END_COLLECTION

  /* --- Player 2 (Report ID 2): same layout --- */
  0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,       // USAGE (Game Pad)
  0xA1, 0x01,       // COLLECTION (Application)
    0x85, 0x02,     //   REPORT_ID (2)
    /* Axes X,Y */
    0x09, 0x30,     //   USAGE (X)
    0x09, 0x31,     //   USAGE (Y)
    0x15, 0x00,     //   LOGICAL_MINIMUM (0)
    0x25, 0xFF,     //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,     //   REPORT_SIZE (8)
    0x95, 0x02,     //   REPORT_COUNT (2)
    0x81, 0x02,     //   INPUT (Data,Var,Abs)
    /* Buttons (13 bits) */
    0x05, 0x09,     //   USAGE_PAGE (Button)
    0x19, 0x01,     //   USAGE_MINIMUM (Button 1)
    0x29, 0x0D,     //   USAGE_MAXIMUM (Button 13)
    0x15, 0x00,     //   LOGICAL_MINIMUM (0)
    0x25, 0x01,     //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,     //   REPORT_SIZE (1)
    0x95, 0x0D,     //   REPORT_COUNT (13)
    0x81, 0x02,     //   INPUT (Data,Var,Abs)
    /* Padding bits to align to 16-bit button field (3 bits) */
    0x95, 0x03,     //   REPORT_COUNT (3)
    0x75, 0x01,     //   REPORT_SIZE (1)
    0x81, 0x03,     //   INPUT (Const)
  0xC0,             // END_COLLECTION

  /* --- Arduino-compatible 30-button device (Report ID 3) --- */
  0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,       // USAGE (Game Pad)
  0xA1, 0x01,       // COLLECTION (Application)
    0x85, 0x03,     //   REPORT_ID (3)
    0x05, 0x09,     //   USAGE_PAGE (Button)
    0x19, 0x01,     //   USAGE_MINIMUM (Button 1)
    0x29, 0x1E,     //   USAGE_MAXIMUM (Button 30)
    0x15, 0x00,     //   LOGICAL_MINIMUM (0)
    0x25, 0x01,     //   LOGICAL_MAXIMUM (1)
    0x95, 0x1E,     //   REPORT_COUNT (30)
    0x75, 0x01,     //   REPORT_SIZE (1)
    0x81, 0x02,     //   INPUT (Data,Var,Abs)
    0x95, 0x02,     //   REPORT_COUNT (2) - padding bits
    0x75, 0x01,     //   REPORT_SIZE (1)
    0x81, 0x03,     //   INPUT (Const) - padding
  0xC0,             // END_COLLECTION

  /* Remaining bytes (up to HID_REPORT_DESC_SIZE_CUSTOM) are zero */
};

/* Ensure the array is fully initialized to the expected size: append zeros if compiler
   didn't fill remaining elements (some toolchains allow partial initfills with zeros).
   The declaration above leaves the remaining bytes zero-initialized by C standard. */

#endif /* USE_JOYSTICK_MODE */
