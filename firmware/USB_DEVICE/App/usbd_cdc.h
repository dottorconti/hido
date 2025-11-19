/* Minimal CDC class header adapted for HIDO composite */
#ifndef __USBD_CDC_H
#define __USBD_CDC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"

extern USBD_ClassTypeDef  USBD_CDC;

/* CDC endpoints */
#define CDC_CMD_EP                 0x82U  /* EP for CDC notifications */
#define CDC_IN_EP                  0x83U  /* EP for data IN */
#define CDC_OUT_EP                 0x03U  /* EP for data OUT */

#define CDC_DATA_FS_MAX_PACKET_SIZE    64U
#define CDC_CMD_PACKET_SIZE            8U

uint8_t USBD_CDC_SendPacket(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
 }
#endif

#endif /* __USBD_CDC_H */
