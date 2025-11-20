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
/* HID report descriptor compatible with the Arduino Joystick library
   (Report ID = 0x03) for a single gamepad with 30 buttons and no axes.
   The array is sized to HID_REPORT_DESC_SIZE_CUSTOM; remaining bytes are
   zero-padded by C initialization rules so the middleware can continue to
   reference the same descriptor length macro. */
const uint8_t HID_JOYSTICK_ReportDesc_Custom[HID_REPORT_DESC_SIZE_CUSTOM] = {
  /* Collection: Gamepad */
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
