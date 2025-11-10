/**
  ******************************************************************************
  * @file    usbd_hid_custom.h
  * @author  HIDO Project
  * @brief   Custom HID descriptor definitions for arcade joystick
  *          This file overrides default HID middleware configuration
  ******************************************************************************
  */

#ifndef __USBD_HID_CUSTOM_H
#define __USBD_HID_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Mode selection - uncomment only ONE */
//#define USE_KEYBOARD_MODE    // NKRO Keyboard mode
#define USE_JOYSTICK_MODE      // Arcade Joystick mode
//#define USE_JVS_MODE         // JVS protocol mode

/* HID Configuration for Joystick Mode */
#ifdef USE_JOYSTICK_MODE
  /* Report size: 1 Report ID + 2 axes + 2 bytes buttons (14 buttons + 2 padding) */
  #define HID_EPIN_SIZE_CUSTOM          0x05U
  
  /* Descriptor size: Dual joystick with Report ID (2 joysticks, 14 buttons each) */
  #define HID_REPORT_DESC_SIZE_CUSTOM   55U

  /* Custom HID Report Descriptor - Dual Joystick with 14 buttons each */
  extern const uint8_t HID_JOYSTICK_ReportDesc_Custom[HID_REPORT_DESC_SIZE_CUSTOM];
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USBD_HID_CUSTOM_H */
