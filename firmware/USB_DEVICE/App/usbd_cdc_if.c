/* Minimal CDC IF implementation: stubs for transmit/receive */
#include "usbd_cdc_if.h"
#include "usbd_core.h"

uint8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* In real implementation buffer data for processing */
  (void)Buf; (void)Len;
  return 0;
}

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  /* Transmit via USBD_CDC_SendPacket */
  extern USBD_HandleTypeDef hUsbDeviceFS;
  USBD_CDC_SendPacket(&hUsbDeviceFS, Buf, Len);
  return 0;
}
