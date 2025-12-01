/* Minimal CDC interface header (app side) */
#ifndef __USBD_CDC_IF_H__
#define __USBD_CDC_IF_H__

#include "usbd_cdc.h"

int8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
void CDC_ReceiveCallback(uint8_t* Buf, uint32_t *Len);

#endif /* __USBD_CDC_IF_H__ */
