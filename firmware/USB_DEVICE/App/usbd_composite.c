/* Composite class: forwards calls to HID and CDC sub-classes */
#include "usbd_composite.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_core.h"

static uint8_t COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /* Initialize HID first */
  if (USBD_HID.Init(pdev, cfgidx) != USBD_OK) return USBD_FAIL;
  /* Then CDC */
  if (USBD_CDC.Init(pdev, cfgidx) != USBD_OK) return USBD_FAIL;
  return USBD_OK;
}

static uint8_t COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_HID.DeInit(pdev, cfgidx);
  USBD_CDC.DeInit(pdev, cfgidx);
  return USBD_OK;
}

static uint8_t COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /* Dispatch to HID or CDC depending on request type/recipient */
  if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
  {
    /* Try HID then CDC */
    if (USBD_HID.Setup && (USBD_HID.Setup(pdev, req) == USBD_OK)) return USBD_OK;
    if (USBD_CDC.Setup && (USBD_CDC.Setup(pdev, req) == USBD_OK)) return USBD_OK;
  }
  return USBD_FAIL;
}

static uint8_t COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  if (USBD_HID.EP0_RxReady) USBD_HID.EP0_RxReady(pdev);
  if (USBD_CDC.EP0_RxReady) USBD_CDC.EP0_RxReady(pdev);
  return USBD_OK;
}

static uint8_t COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* Let HID and CDC handle their endpoints */
  if (USBD_HID.DataIn) USBD_HID.DataIn(pdev, epnum);
  if (USBD_CDC.DataIn) USBD_CDC.DataIn(pdev, epnum);
  return USBD_OK;
}

static uint8_t COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (USBD_HID.DataOut) USBD_HID.DataOut(pdev, epnum);
  if (USBD_CDC.DataOut) USBD_CDC.DataOut(pdev, epnum);
  return USBD_OK;
}

static uint8_t *COMPOSITE_GetFSCfgDesc(uint16_t *length)
{
  /* Forward to HID descriptor provider (usbd_desc.c) */
  return USBD_HID.GetFSConfigDescriptor(length);
}

USBD_ClassTypeDef USBD_COMPOSITE = {
  COMPOSITE_Init,
  COMPOSITE_DeInit,
  COMPOSITE_Setup,
  NULL,
  COMPOSITE_EP0_RxReady,
  COMPOSITE_DataIn,
  COMPOSITE_DataOut,
  NULL,
  NULL,
  NULL,
  NULL,
  COMPOSITE_GetFSCfgDesc,
  NULL,
  NULL,
};
