/* Minimal CDC interface implementation (app side) */
#include "usbd_cdc_if.h"

int8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  UNUSED(Buf);
  UNUSED(Len);
  return 0;
}

void CDC_ReceiveCallback(uint8_t* Buf, uint32_t *Len)
{
  UNUSED(Buf);
  UNUSED(Len);
}
