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
const uint8_t HID_JOYSTICK_ReportDesc_Custom[HID_REPORT_DESC_SIZE_CUSTOM] = {
    /* ===== PLAYER 1 - Separate Application Collection ===== */
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,        // USAGE (Joystick)
    0xA1, 0x01,        // COLLECTION (Application)
    0x85, 0x01,        //   REPORT_ID (1)
    
    /* Player 1 Axes */
    0x09, 0x01,        //   USAGE (Pointer)
    0xA1, 0x00,        //   COLLECTION (Physical)
    0x09, 0x30,        //     USAGE (X)
    0x09, 0x31,        //     USAGE (Y)
    0x15, 0x00,        //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x02,        //     REPORT_COUNT (2)
    0x81, 0x02,        //     INPUT (Data,Var,Abs)
    0xC0,              //   END_COLLECTION (Physical)
    
    /* Player 1 Buttons */
    0x05, 0x09,        //   USAGE_PAGE (Button)
    0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
    0x29, 0x0D,        //   USAGE_MAXIMUM (Button 13)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x0D,        //   REPORT_COUNT (13)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    
    /* Padding */
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x03,        //   REPORT_COUNT (3)
    0x81, 0x01,        //   INPUT (Const)
    
    0xC0,              // END_COLLECTION (Application)
    
    /* ===== PLAYER 2 - Separate Application Collection ===== */
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,        // USAGE (Joystick)
    0xA1, 0x01,        // COLLECTION (Application)
    0x85, 0x02,        //   REPORT_ID (2)
    
    /* Player 2 Axes */
    0x09, 0x01,        //   USAGE (Pointer)
    0xA1, 0x00,        //   COLLECTION (Physical)
    0x09, 0x30,        //     USAGE (X)
    0x09, 0x31,        //     USAGE (Y)
    0x15, 0x00,        //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x02,        //     REPORT_COUNT (2)
    0x81, 0x02,        //     INPUT (Data,Var,Abs)
    0xC0,              //   END_COLLECTION (Physical)
    
    /* Player 2 Buttons */
    0x05, 0x09,        //   USAGE_PAGE (Button)
    0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
    0x29, 0x0D,        //   USAGE_MAXIMUM (Button 13)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x0D,        //   REPORT_COUNT (13)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    
    /* Padding */
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x03,        //   REPORT_COUNT (3)
    0x81, 0x01,        //   INPUT (Const)
    
    0xC0               // END_COLLECTION (Application)
};

#endif /* USE_JOYSTICK_MODE */
