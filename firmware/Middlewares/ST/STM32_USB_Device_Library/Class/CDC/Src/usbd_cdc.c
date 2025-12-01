/* Minimal CDC class implementation (functional subset) */
#include "usbd_cdc.h"
#include "usbd_core.h"

/* Class-specific request codes (CDC ACM) */
#define CDC_SET_LINE_CODING        0x20U
#define CDC_GET_LINE_CODING        0x21U
#define CDC_SET_CONTROL_LINE_STATE 0x22U

typedef struct {
  USBD_CDC_LineCodingTypeDef LineCoding;
  uint8_t TxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];
  uint8_t RxBuffer[CDC_DATA_FS_MAX_PACKET_SIZE];
  uint8_t TxState;
  uint16_t RxLength;
} USBD_CDC_HandleTypeDef;

USBD_ClassTypeDef USBD_CDC = {0};

uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));
  if (hcdc == NULL) return USBD_FAIL;

  hcdc->LineCoding.dwDTERate   = 115200;
  hcdc->LineCoding.bCharFormat = 0;
  hcdc->LineCoding.bParityType = 0;
  hcdc->LineCoding.bDataBits   = 8;
  hcdc->TxState = 0;
  hcdc->RxLength = 0;

  pdev->pClassData = hcdc;

  /* Open CDC endpoints ONLY - do NOT touch HID EP 0x81 */
  USBD_LL_OpenEP(pdev, CDC_IN_EP,  USBD_EP_TYPE_BULK, CDC_DATA_FS_MAX_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_MAX_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, CDC_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);

  /* CRITICAL: PrepareReceive arms OUT endpoint interrupt.
     In composite scenario, must be called AFTER all classes initialized.
     Defer to composite wrapper's post-init or first Setup event. */
  /* USBD_LL_PrepareReceive deferred */
  return USBD_OK;
}

uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_LL_CloseEP(pdev, CDC_IN_EP);
  USBD_LL_CloseEP(pdev, CDC_OUT_EP);
  USBD_LL_CloseEP(pdev, CDC_CMD_EP);
  if (pdev->pClassData) {
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  return USBD_OK;
}

uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassData;
  if (hcdc == NULL) return USBD_FAIL;

  uint8_t recipient = req->bmRequest & USB_REQ_RECIPIENT_MASK;
  uint8_t reqType   = req->bmRequest & USB_REQ_TYPE_MASK;
  uint16_t wIndex   = req->wIndex & 0xFFU;

  if (recipient == USB_REQ_RECIPIENT_INTERFACE) {
    if (wIndex != 1U && wIndex != 2U) return USBD_FAIL;
  } else if (recipient == USB_REQ_RECIPIENT_ENDPOINT) {
    uint8_t ep = (uint8_t)(req->wIndex & 0xFFU);
    if (!(ep == CDC_CMD_EP || ep == CDC_OUT_EP || ep == CDC_IN_EP)) return USBD_FAIL;
  } else {
    return USBD_FAIL;
  }

  if (reqType == USB_REQ_TYPE_CLASS) {
    switch (req->bRequest) {
      case CDC_SET_LINE_CODING:
        if (req->wLength == 7U) {
          USBD_CtlPrepareRx(pdev, (uint8_t *)&hcdc->LineCoding, 7U);
          return USBD_OK;
        }
        break;
      case CDC_GET_LINE_CODING:
        USBD_CtlSendData(pdev, (uint8_t *)&hcdc->LineCoding, 7U);
        return USBD_OK;
      case CDC_SET_CONTROL_LINE_STATE:
        return USBD_OK;
      default:
        return USBD_OK;
    }
    return USBD_FAIL;
  }
  return USBD_FAIL;
}

uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassData;
  if (hcdc == NULL) return USBD_FAIL;
  hcdc->TxState = 0U;
  return USBD_OK;
}

uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* Strict EP filter: only handle CDC_OUT_EP (0x03) */
  if (epnum != (CDC_OUT_EP & 0x0FU)) return USBD_OK;
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassData;
  if (hcdc == NULL) {
    /* If CDC not initialized yet (e.g., composite still loading), prepare anyway */
    return USBD_OK;
  }
  
  hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);
  if (hcdc->RxLength > 0 && hcdc->RxLength <= CDC_DATA_FS_MAX_PACKET_SIZE) {
    if (hcdc->TxState == 0U) {
      memcpy(hcdc->TxBuffer, hcdc->RxBuffer, hcdc->RxLength);
      hcdc->TxState = 1U;
      USBD_LL_Transmit(pdev, CDC_IN_EP, hcdc->TxBuffer, hcdc->RxLength);
    }
  }
  USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_MAX_PACKET_SIZE);
  return USBD_OK;
}

/* Helper function for composite to arm CDC OUT after all classes initialized.
   Avoids race condition with HID EP during Init sequence. */
void USBD_CDC_StartReception(USBD_HandleTypeDef *pdev, void *pCDCClassData)
{
  if (pCDCClassData == NULL) return;
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pCDCClassData;
  USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, hcdc->RxBuffer, CDC_DATA_FS_MAX_PACKET_SIZE);
}
