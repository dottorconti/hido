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
//#define USE_JOYSTICK_MODE      // Arcade Joystick mode
//#define USE_JVS_MODE         // JVS protocol mode

/* HID Configuration for Joystick Mode */
#ifdef USE_JOYSTICK_MODE
    /* Report size: per-report payload; set endpoint packet size to 64 bytes
      to accommodate the full joystick report descriptor on Full-Speed USB. */
    #define HID_EPIN_SIZE_CUSTOM          0x40U

    /* Override default HID_EPIN_SIZE from the middleware to ensure all
      descriptors and endpoint open calls use the 64-byte max packet size. */
  #ifdef HID_EPIN_SIZE
  #undef HID_EPIN_SIZE
  #endif
  #define HID_EPIN_SIZE HID_EPIN_SIZE_CUSTOM

  /* Split the combined report into two per-player descriptors so the
     device exposes two separate HID interfaces. Sizes below are the
     byte-lengths for each player's report descriptor. */
  #define HID_REPORT_DESC_SIZE_CUSTOM_P1   44U
  #define HID_REPORT_DESC_SIZE_CUSTOM_P2   44U

  /* Custom HID Report Descriptors - one array per player */
  extern const uint8_t HID_JOYSTICK_ReportDesc_P1[HID_REPORT_DESC_SIZE_CUSTOM_P1];
  extern const uint8_t HID_JOYSTICK_ReportDesc_P2[HID_REPORT_DESC_SIZE_CUSTOM_P2];
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USBD_HID_CUSTOM_H */
