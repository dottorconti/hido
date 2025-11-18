/* Minimal CDC class header adapted for HIDO project */
#ifndef __USB_CDC_H
#define __USB_CDC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_ioreq.h"

#define CDC_CMD_EP                 0x83U  /* EP for CDC commands (interrupt IN) */
#define CDC_IN_EP                  0x84U  /* EP for data IN (bulk) */
#define CDC_OUT_EP                 0x04U  /* EP for data OUT (bulk) */

#define CDC_DATA_HS_MAX_PACKET_SIZE    512U
#define CDC_DATA_FS_MAX_PACKET_SIZE    64U
#define CDC_CMD_PACKET_SIZE            8U

extern USBD_ClassTypeDef  USBD_CDC;

uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev, void *fops);

#ifdef __cplusplus
 }
#endif

#endif /* __USB_CDC_H */
