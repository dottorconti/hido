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
/* Minimal HID report descriptor for testing SET_CONFIGURATION / EP0 handling.
   This is a small gamepad descriptor padded to the expected custom size to
   exercise GET_DESCRIPTOR REPORT without changing the descriptor length
   macros. If this allows the host to complete SET_CONFIGURATION then the
   original complex descriptor can be investigated for multi-packet issues. */
const uint8_t HID_JOYSTICK_ReportDesc_Custom[HID_REPORT_DESC_SIZE_CUSTOM] = {
  /* Minimal gamepad: X,Y axes */
  0x05, 0x01,      // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,      // USAGE (Game Pad)
  0xA1, 0x01,      // COLLECTION (Application)
  0x09, 0x30,      //   USAGE (X)
  0x09, 0x31,      //   USAGE (Y)
  0x15, 0x81,      //   LOGICAL_MINIMUM (-127)
  0x25, 0x7F,      //   LOGICAL_MAXIMUM (127)
  0x75, 0x08,      //   REPORT_SIZE (8)
  0x95, 0x02,      //   REPORT_COUNT (2)
  0x81, 0x02,      //   INPUT (Data,Var,Abs)
  0xC0,            // END_COLLECTION
  /* Padding bytes to reach HID_REPORT_DESC_SIZE_CUSTOM */
    
};

/* Ensure the array is fully initialized to the expected size: append zeros if compiler
   didn't fill remaining elements (some toolchains allow partial initfills with zeros).
   The declaration above leaves the remaining bytes zero-initialized by C standard. */

#endif /* USE_JOYSTICK_MODE */
