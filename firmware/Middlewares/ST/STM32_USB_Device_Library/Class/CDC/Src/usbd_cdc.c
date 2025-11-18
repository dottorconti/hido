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
  UNUSED(pdev);
  UNUSED(req);
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
