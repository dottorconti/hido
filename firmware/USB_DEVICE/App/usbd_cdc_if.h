/* Minimal CDC IF header for HIDO project */
#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#include <stdint.h>

void CDC_ReceiveCallback(uint8_t* Buf, uint32_t *Len);
void CDC_Process(void);
int CDC_Transmit(uint8_t* Buf, uint16_t Len);

#endif /* __USBD_CDC_IF_H */
