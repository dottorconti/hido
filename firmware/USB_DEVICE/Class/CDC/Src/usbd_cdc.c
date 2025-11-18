/* Minimal CDC Class implementation (stubs) adapted for HIDO project
 * This provides basic symbols so the CDC class can be included in the build.
 * Full CDC behavior (line coding, notifications, buffering) can be added later.
 */

#include "usbd_cdc.h"
#include "usbd_core.h"

static uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  /* No endpoints opened here; real implementation should open CDC endpoints */
  pdev->pClassData = USBD_malloc(1);
  return (pdev->pClassData != NULL) ? USBD_OK : USBD_FAIL;
}

static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  if (pdev->pClassData) {
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  return USBD_OK;
}

static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  UNUSED(pdev); UNUSED(req);
  return USBD_OK;
}

static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  UNUSED(pdev);
  return USBD_OK;
}

static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(pdev); UNUSED(epnum);
  return USBD_OK;
}

static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(pdev); UNUSED(epnum);
  return USBD_OK;
}

USBD_ClassTypeDef USBD_CDC =
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
  NULL,
  NULL,
  NULL,
  NULL
};

uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev, void *fops)
{
  UNUSED(pdev); UNUSED(fops);
  /* In full ST implementation this wires callbacks. Here we simply return OK. */
  return USBD_OK;
}
