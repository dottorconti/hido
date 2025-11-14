#ifndef __USBD_HID_RAW_H
#define __USBD_HID_RAW_H

#include <stdint.h>

#define HID_RAW_EPIN_ADDR      0x83U
#define HID_RAW_EPOUT_ADDR     0x03U
#define HID_RAW_EPIN_SIZE      64U
#define HID_RAW_EPOUT_SIZE     64U
#define HID_RAW_POLLING_INTERVAL 0x01U


#define HID_RAW_REPORT_DESC_SIZE 0x41U
extern const uint8_t HID_RAW_ReportDesc[HID_RAW_REPORT_DESC_SIZE];

#include "usbd_def.h"

// Dichiarazione della classe per la seconda interfaccia HID RAW
extern USBD_ClassTypeDef USBD_HID_RAW;

// API di base per la RAW HID
uint8_t USBD_HID_RAW_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);
uint8_t USBD_HID_RAW_ReceivePacket(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len);

#endif /* __USBD_HID_RAW_H */
