/* Minimal composite class implementation that delegates to HID and CDC */
#include "usbd_composite.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_core.h"

/* Forward declarations */
static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t *USBD_COMPOSITE_GetFSConfigDescriptor(uint16_t *length);

/* Use the original config descriptor (two HID interfaces) for initial testing.
  This avoids the CDC block and PMA complexity while verifying composite registration. */
extern uint8_t USBD_FS_CfgDesc[];
extern uint16_t USBD_FS_CfgDescSize;


USBD_ClassTypeDef USBD_COMPOSITE =
{
  USBD_COMPOSITE_Init,
  USBD_COMPOSITE_DeInit,
  USBD_COMPOSITE_Setup,
  NULL, /* EP0_TxSent */
  NULL, /* EP0_RxReady */
  USBD_COMPOSITE_DataIn,
  USBD_COMPOSITE_DataOut,
  NULL, /* SOF */
  NULL, /* IsoINIncomplete */
  NULL, /* IsoOUTIncomplete */
  NULL, /* GetHSConfigDescriptor */
  USBD_COMPOSITE_GetFSConfigDescriptor,
  NULL, /* GetOtherSpeedConfigDescriptor */
  NULL, /* GetDeviceQualifierDescriptor */
};

static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  /* Initialize HID and CDC classes. Call CDC helper functions directly
     because the minimal CDC class object we added does not populate
     the function-pointer table. Calling USBD_CDC.Init (NULL) would crash. */
  if (USBD_HID.Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  if (USBD_CDC_Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  return ret;
}

static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  if (USBD_CDC_DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  return ret;
}

static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /* Let CDC try first, then HID. Use CDC helper function. */
  if (USBD_CDC_Setup(pdev, req) == USBD_OK) return USBD_OK;
  if (USBD_HID.Setup(pdev, req) == USBD_OK) return USBD_OK;
  return USBD_OK;
}

static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* Delegate to both — the handler will ignore unknown endpoints */
  USBD_CDC_DataIn(pdev, epnum);
  USBD_HID.DataIn(pdev, epnum);
  return USBD_OK;
}

static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_CDC_DataOut(pdev, epnum);
  USBD_HID.DataOut(pdev, epnum);
  return USBD_OK;
}

static uint8_t *USBD_COMPOSITE_GetFSConfigDescriptor(uint16_t *length)
{
  *length = USBD_FS_CfgDescSize;
  return USBD_FS_CfgDesc;
}
