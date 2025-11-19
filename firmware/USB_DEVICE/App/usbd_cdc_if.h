#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#include "usbd_cdc.h"

uint8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len);
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

#endif
