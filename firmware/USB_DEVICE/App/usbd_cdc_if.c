/* Minimal CDC IF implementation: simple buffers, control and receive handling */
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "main.h"

/* Extern USB device handle (provided by MX generated code) */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Simple RX ring buffer used to queue bytes received from host */
#define CDC_RX_RING_SIZE 1024
static uint8_t cdc_rx_ring[CDC_RX_RING_SIZE];
static volatile uint32_t cdc_rx_head = 0;
static volatile uint32_t cdc_rx_tail = 0;

/* Local TX/RX working buffers */
static uint8_t UserRxBufferFS[CDC_DATA_FS_MAX_PACKET_SIZE];
static uint8_t UserTxBufferFS[CDC_DATA_FS_MAX_PACKET_SIZE];

/* Line coding default: 115200, 1 stop bit, no parity, 8 data bits */
static uint8_t linecoding[7] = { 0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08 };
static uint16_t controlLineState = 0;

/* Forward declarations required by the class wrapper pattern */
static int8_t CDC_Init(void);
static int8_t CDC_DeInit(void);
static int8_t CDC_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive(uint8_t* Buf, uint32_t *Len);

/* Expose the interface structure so a CDC class can register it if needed. */
typedef struct
{
  int8_t (*Init)(void);
  int8_t (*DeInit)(void);
  int8_t (*Control)(uint8_t, uint8_t*, uint16_t);
  int8_t (*Receive)(uint8_t*, uint32_t*);
} USBD_CDC_ItfTypeDef;

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init,
  CDC_DeInit,
  CDC_Control,
  CDC_Receive
};

/* Implementation ----------------------------------------------------------*/
static int8_t CDC_Init(void)
{
  /* Set application Rx/Tx buffers (used by higher-level helpers) */
  /* If the stack provides USBD_CDC_SetTxBuffer/SetRxBuffer these can be
     called by name; otherwise we arm the OUT endpoint directly. */
#if 0
  /* Example if SetTx/SetRx are available:
     USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
     USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  */
#endif

  /* Arm the OUT endpoint to receive first packet */
  (void)USBD_LL_PrepareReceive(&hUsbDeviceFS, CDC_OUT_EP, UserRxBufferFS, CDC_DATA_FS_MAX_PACKET_SIZE);

  /* Indicate CDC IF Init reached via LED (short one-shot managed elsewhere) */
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
  extern volatile uint32_t cdc_led_one_shot_until;
  cdc_led_one_shot_until = HAL_GetTick() + 150;

  return 0;
}

static int8_t CDC_DeInit(void)
{
  /* nothing to free in this minimal implementation */
  return 0;
}

/* CDC class control requests handling (SET/GET LINE CODING etc.) */
static int8_t CDC_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  switch (cmd)
  {
    case 0x20: /* SET_LINE_CODING */
      if (pbuf != NULL && length >= 7)
      {
        for (uint8_t i = 0; i < 7; ++i) linecoding[i] = pbuf[i];
      }
      break;

    case 0x21: /* GET_LINE_CODING */
      if (pbuf != NULL && length >= 7)
      {
        for (uint8_t i = 0; i < 7; ++i) pbuf[i] = linecoding[i];
      }
      break;

    case 0x22: /* SET_CONTROL_LINE_STATE */
      /* pbuf may be NULL; length ignored */
      controlLineState = (uint16_t) ( (uint16_t) pbuf[0] | ((uint16_t)pbuf[1] << 8) );
      (void)controlLineState;
      break;

    default:
      /* Not handled here */
      break;
  }

  return 0;
}

/* Copy received bytes into a simple ring buffer and re-arm OUT endpoint */
static int8_t CDC_Receive(uint8_t* Buf, uint32_t *Len)
{
  uint32_t n = (Len) ? *Len : 0;
  for (uint32_t i = 0; i < n; ++i)
  {
    uint32_t next = (cdc_rx_head + 1) & (CDC_RX_RING_SIZE - 1);
    if (next == cdc_rx_tail)
    {
      /* ring full: drop oldest byte to make room */
      cdc_rx_tail = (cdc_rx_tail + 1) & (CDC_RX_RING_SIZE - 1);
    }
    cdc_rx_ring[cdc_rx_head] = Buf[i];
    cdc_rx_head = next;
  }

  /* Re-arm the OUT endpoint to receive next packet */
  (void)USBD_LL_PrepareReceive(&hUsbDeviceFS, CDC_OUT_EP, UserRxBufferFS, CDC_DATA_FS_MAX_PACKET_SIZE);

  return 0;
}

/* Compatibility wrappers -----------------------------------------------------------------*/
uint8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* Called by older helpers in some stacks — forward to our CDC_Receive */
  CDC_Receive(Buf, Len);
  return 0;
}

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  /* Transmit via class helper if configured */
  if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
  {
    return (uint8_t)USBD_CDC_SendPacket(&hUsbDeviceFS, Buf, Len);
  }
  return USBD_FAIL;
}
