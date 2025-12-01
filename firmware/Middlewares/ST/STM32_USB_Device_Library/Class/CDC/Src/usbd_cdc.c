/* Minimal CDC class implementation (stub) */
#include "usbd_cdc.h"
#include "usbd_core.h"

USBD_ClassTypeDef USBD_CDC = {0};

uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(pdev);
  UNUSED(cfgidx);
  return USBD_OK;
}

uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(pdev);
  UNUSED(cfgidx);
  return USBD_OK;
}

uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /* Only claim CDC-specific requests for the CDC interfaces (IF1/IF2). */
  uint8_t recipient = req->bmRequest & USB_REQ_RECIPIENT_MASK; /* DEVICE/INTERFACE/ENDPOINT */
  uint16_t wIndex = req->wIndex & 0xFFU; /* interface or endpoint index */

  /* Our CDC Control is IF1, CDC Data is IF2. If the request targets other interfaces, do not handle. */
  if (recipient == USB_REQ_RECIPIENT_INTERFACE)
  {
    if (wIndex != 1U && wIndex != 2U)
    {
      return USBD_FAIL; /* Let other classes handle (e.g., HID on IF0) */
    }
  }
  else if (recipient == USB_REQ_RECIPIENT_ENDPOINT)
  {
    /* Endpoints 0x83, 0x03, 0x84 belong to CDC; others should be ignored */
    uint8_t ep = (uint8_t)(wIndex & 0xFFU);
    if (!(ep == 0x83U || ep == 0x03U || ep == 0x84U))
    {
      return USBD_FAIL;
    }
  }
  else
  {
    /* DEVICE or other recipients -> not CDC's responsibility here */
    return USBD_FAIL;
  }

  /* Minimal stub: acknowledge valid CDC requests without data handling. */
  return USBD_OK;
}

uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(pdev);
  UNUSED(epnum);
  return USBD_OK;
}

uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(pdev);
  UNUSED(epnum);
  return USBD_OK;
}
