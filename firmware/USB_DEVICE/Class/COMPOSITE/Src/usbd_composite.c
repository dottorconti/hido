/* Minimal composite class that delegates to HID and CDC class implementations.
 * This allows the device to expose HID (2 interfaces) and CDC (2 interfaces)
 * using a single USBD class object required by the ST stack.
 */

#include "usbd_composite.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"

static uint8_t  Composite_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.Init) ret = (USBD_HID.Init(pdev, cfgidx) == USBD_OK) ? ret : USBD_FAIL;
  if (USBD_CDC.Init) ret = (USBD_CDC.Init(pdev, cfgidx) == USBD_OK) ? ret : USBD_FAIL;
  return ret;
}

static uint8_t  Composite_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.DeInit) ret = (USBD_HID.DeInit(pdev, cfgidx) == USBD_OK) ? ret : USBD_FAIL;
  if (USBD_CDC.DeInit) ret = (USBD_CDC.DeInit(pdev, cfgidx) == USBD_OK) ? ret : USBD_FAIL;
  return ret;
}

static uint8_t  Composite_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.Setup) ret = (USBD_HID.Setup(pdev, req) == USBD_OK) ? ret : USBD_FAIL;
  if (USBD_CDC.Setup) ret = (USBD_CDC.Setup(pdev, req) == USBD_OK) ? ret : USBD_FAIL;
  return ret;
}

static uint8_t Composite_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.EP0_RxReady) ret = (USBD_HID.EP0_RxReady(pdev) == USBD_OK) ? ret : USBD_FAIL;
  if (USBD_CDC.EP0_RxReady) ret = (USBD_CDC.EP0_RxReady(pdev) == USBD_OK) ? ret : USBD_FAIL;
  return ret;
}

static uint8_t Composite_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.DataIn) ret = (USBD_HID.DataIn(pdev, epnum) == USBD_OK) ? ret : USBD_FAIL;
  if (USBD_CDC.DataIn) ret = (USBD_CDC.DataIn(pdev, epnum) == USBD_OK) ? ret : USBD_FAIL;
  return ret;
}

static uint8_t Composite_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.DataOut) ret = (USBD_HID.DataOut(pdev, epnum) == USBD_OK) ? ret : USBD_FAIL;
  if (USBD_CDC.DataOut) ret = (USBD_CDC.DataOut(pdev, epnum) == USBD_OK) ? ret : USBD_FAIL;
  return ret;
}

/* Descriptor getters: return the combined config descriptor defined in usbd_desc.c */
extern uint8_t USBD_FS_CfgDesc[];
static uint8_t *Composite_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_FS_CfgDesc);
  return USBD_FS_CfgDesc;
}

static uint8_t *Composite_GetHSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_FS_CfgDesc);
  return USBD_FS_CfgDesc; /* Full-speed only device for STM32F1 */
}

USBD_ClassTypeDef USBD_Composite =
{
  Composite_Init,
  Composite_DeInit,
  Composite_Setup,
  NULL,
  Composite_EP0_RxReady,
  Composite_DataIn,
  Composite_DataOut,
  NULL,
  NULL,
  NULL,
  Composite_GetHSCfgDesc,
  Composite_GetFSCfgDesc,
  NULL,
  NULL,
};
