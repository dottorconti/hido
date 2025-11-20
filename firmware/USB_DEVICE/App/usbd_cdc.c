/* Minimal CDC class implementation (skeleton) */
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_ctlreq.h"

/* Visual debug: toggle LED2 on CDC Setup calls */
#include "main.h"
#include "gpio.h"

typedef struct
{
  uint8_t  state;
  uint8_t  rxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];
  uint8_t  lineCoding[7];
  uint16_t controlLineState;
} USBD_CDC_HandleTypeDef;

static uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_CDC_HandleTypeDef *hcdc;

  /* Allocate class data */
  pdev->pClassData = USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));
  if (pdev->pClassData == NULL) return USBD_FAIL;
  hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;
  hcdc->state = 0;

  /* Open CDC endpoints (notification, data IN, data OUT) */
  USBD_LL_OpenEP(pdev, CDC_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, CDC_IN_EP,  USBD_EP_TYPE_BULK, CDC_DATA_FS_MAX_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_MAX_PACKET_SIZE);

  /* Indicate CDC Init reached: turn LED1 on */
  /* Use LED3 for CDC init one-shot */
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
  extern volatile uint32_t cdc_led_one_shot_until;
  cdc_led_one_shot_until = HAL_GetTick() + 200;

  /* Prepare Out endpoint to receive first packet */
  USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, hcdc->rxBuffer, CDC_DATA_FS_MAX_PACKET_SIZE);

  return USBD_OK;
}

static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /* Close endpoints */
  USBD_LL_CloseEP(pdev, CDC_CMD_EP);
  USBD_LL_CloseEP(pdev, CDC_IN_EP);
  USBD_LL_CloseEP(pdev, CDC_OUT_EP);

  /* Turn LED1 off to indicate CDC DeInit */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  if (pdev->pClassData) { USBD_free(pdev->pClassData); pdev->pClassData = NULL; }
  return USBD_OK;
}

static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;

  /* Toggle LED2 to indicate Setup was invoked by host */
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

  if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
  {
    switch (req->bRequest)
    {
      case 0x21: /* GET_LINE_CODING */
        USBD_CtlSendData(pdev, hcdc->lineCoding, MIN(req->wLength, 7U));
        return USBD_OK;

      case 0x20: /* SET_LINE_CODING */
        /* Prepare to receive 7 bytes */
        USBD_CtlPrepareRx(pdev, hcdc->lineCoding, MIN(req->wLength, 7U));
        return USBD_OK;

      case 0x22: /* SET_CONTROL_LINE_STATE */
        hcdc->controlLineState = (uint16_t)req->wValue;
        /* Acknowledge with ZLP */
        USBD_CtlSendStatus(pdev);
        return USBD_OK;

      default:
        USBD_CtlError(pdev, req);
        return USBD_FAIL;
    }
  }

  /* Not a class request - fail */
  USBD_CtlError(pdev, req);
  return USBD_FAIL;
}

static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  /* Data stage for SET_LINE_CODING completed - nothing else required here for now */
  return USBD_OK;
}

static uint8_t *USBD_CDC_GetHSCfgDesc(uint16_t *length){return NULL;}
static uint8_t *USBD_CDC_GetOtherSpeedCfgDesc(uint16_t *length){return NULL;}
static uint8_t *USBD_CDC_GetDeviceQualifierDesc(uint16_t *length){return NULL;}

/* Minimal CDC-only configuration descriptor for FS (for testing and visibility)
  This variant includes Header, Call Management, ACM and Union functional
  descriptors so the host can bind the CDC driver correctly.
  wTotalLength = 0x43 (67) bytes */
__ALIGN_BEGIN static uint8_t USBD_FS_CDC_CfgDesc[] __ALIGN_END = {
  /* Configuration Descriptor */
  0x09, USB_DESC_TYPE_CONFIGURATION,
  0x43, 0x00, /* wTotalLength = 67 bytes */
  0x02, /* bNumInterfaces */
  0x01, /* bConfigurationValue */
  0x00, /* iConfiguration */
  0x80, /* bmAttributes: bus powered */
  0x32, /* MaxPower 100mA */

  /* Interface 0: CDC Communication */
  0x09, USB_DESC_TYPE_INTERFACE,
  0x00, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00,
  /* CDC Header */
  0x05, 0x24, 0x00, 0x10, 0x01,
  /* CDC Call Management Functional Descriptor */
  0x05, 0x24, 0x01, 0x00, 0x01,
  /* CDC ACM */
  0x04, 0x24, 0x02, 0x02,
  /* CDC Union Functional Descriptor (master=0, slave=1) */
  0x05, 0x24, 0x06, 0x00, 0x01,
  /* Notification Endpoint */
  0x07, USB_DESC_TYPE_ENDPOINT, 0x82, 0x03, 0x08, 0x00, 0x10,

  /* Interface 1: CDC Data */
  0x09, USB_DESC_TYPE_INTERFACE,
  0x01, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,
  /* Data IN Endpoint */
  0x07, USB_DESC_TYPE_ENDPOINT, 0x83, 0x02, 0x40, 0x00, 0x00,
  /* Data OUT Endpoint */
  0x07, USB_DESC_TYPE_ENDPOINT, 0x03, 0x02, 0x40, 0x00, 0x00,
};

static uint8_t *USBD_CDC_GetFSCfgDesc(uint16_t *length)
{
  if (length) *length = sizeof(USBD_FS_CDC_CfgDesc);
  return USBD_FS_CDC_CfgDesc;
}

static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* Transmission complete */
  return USBD_OK;
}

static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)pdev->pClassData;
  uint32_t rx_len = USBD_LL_GetRxDataSize(pdev, epnum);

  /* For now, we simply re-arm the OUT endpoint for next packet */
  USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, hcdc->rxBuffer, CDC_DATA_FS_MAX_PACKET_SIZE);

  /* A real implementation would enqueue/process the received data here */
  (void)rx_len;
  return USBD_OK;
}

USBD_ClassTypeDef  USBD_CDC =
{
  USBD_CDC_Init,
  USBD_CDC_DeInit,
  USBD_CDC_Setup,
  NULL,
  USBD_CDC_EP0_RxReady,
  USBD_CDC_DataIn,
  USBD_CDC_DataOut,
  NULL,
  NULL,
  NULL,
  USBD_CDC_GetHSCfgDesc,
  USBD_CDC_GetFSCfgDesc,
  USBD_CDC_GetOtherSpeedCfgDesc,
  USBD_CDC_GetDeviceQualifierDesc,
};

uint8_t USBD_CDC_SendPacket(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len)
{
  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    return USBD_LL_Transmit(pdev, CDC_IN_EP, buf, len);
  }
  return USBD_FAIL;
}
