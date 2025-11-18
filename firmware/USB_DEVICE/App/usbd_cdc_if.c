/* Minimal CDC interface implementation (echo test) */
#include "usbd_cdc_if.h"
#include <string.h>

static uint8_t rx_buf[128];
static uint32_t rx_len = 0;

void CDC_ReceiveCallback(uint8_t* Buf, uint32_t *Len)
{
  uint32_t l = (*Len);
  if (l > 0 && l < sizeof(rx_buf)) {
    memcpy(rx_buf, Buf, l);
    rx_len = l;
  }
}

void CDC_Process(void)
{
  if (rx_len) {
    /* Echo back for test */
    CDC_Transmit(rx_buf, (uint16_t)rx_len);
    rx_len = 0;
  }
}

int CDC_Transmit(uint8_t* Buf, uint16_t Len)
{
  UNUSED(Buf); UNUSED(Len);
  /* No real transmit implemented here; placeholder returns OK */
  return 0;
}
