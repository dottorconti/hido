/* Minimal composite class implementation that delegates to HID and CDC */
#include "usbd_composite.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_hid_raw.h"

/* Forward declarations */
static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t *USBD_COMPOSITE_GetFSConfigDescriptor(uint16_t *length);
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
static uint8_t *USBD_COMPOSITE_GetUsrStrDescriptor(USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length);
#endif

/* If a combined/full configuration descriptor exists (HID + HID_RAW + CDC),
   prefer it so the composite class and low-level PMA allocations match the
   descriptor content. The full descriptor is defined in `usbd_full_desc.c`.
*/
extern uint8_t USBD_FULL_FS_CfgDesc[];
extern uint16_t USBD_FULL_FS_CfgDescSize;

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
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
  USBD_COMPOSITE_GetUsrStrDescriptor,
#endif
};

static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  /* Initialize HID and CDC classes. Call CDC helper functions directly
     because the minimal CDC class object we added does not populate
     the function-pointer table. Calling USBD_CDC.Init (NULL) would crash. */
  if (USBD_HID.Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  /* Initialize second HID (RAW) if present */
  if (USBD_HID_RAW.Init != NULL)
  {
    if (USBD_HID_RAW.Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  }
  if (USBD_CDC_Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  return ret;
}

static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  if (USBD_HID.DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  if (USBD_HID_RAW.DeInit != NULL)
  {
    if (USBD_HID_RAW.DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  }
  if (USBD_CDC_DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  return ret;
}

static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /* Let CDC try first, then HID. Use CDC helper function. */
  if (USBD_CDC_Setup(pdev, req) == USBD_OK) return USBD_OK;
  /* Try the RAW HID handler first (interface 1), then normal HID */
  if (USBD_HID_RAW.Setup != NULL)
  {
    if (USBD_HID_RAW.Setup(pdev, req) == USBD_OK) return USBD_OK;
  }
  if (USBD_HID.Setup(pdev, req) == USBD_OK) return USBD_OK;
  return USBD_OK;
}

static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  /* Delegate to both — the handler will ignore unknown endpoints */
  USBD_CDC_DataIn(pdev, epnum);
  /* RAW HID may handle epnum 0x82, normal HID handles 0x81 */
  if (USBD_HID_RAW.DataIn != NULL) USBD_HID_RAW.DataIn(pdev, epnum);
  USBD_HID.DataIn(pdev, epnum);
  return USBD_OK;
}

static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_CDC_DataOut(pdev, epnum);
  if (USBD_HID_RAW.DataOut != NULL) USBD_HID_RAW.DataOut(pdev, epnum);
  USBD_HID.DataOut(pdev, epnum);
  return USBD_OK;
}

static uint8_t *USBD_COMPOSITE_GetFSConfigDescriptor(uint16_t *length)
{
  /* If a full configuration descriptor with CDC is available, return it
     so the host sees HID + HID_RAW + CDC interfaces consistently with
     the PMA allocations and class initialization. Otherwise fall back
     to the original FS descriptor (HID-only variant). */
  if (USBD_FULL_FS_CfgDescSize != 0)
  {
    /* Ensure the wTotalLength field in the descriptor matches the actual
       size. The wTotalLength is at offset 2 (little-endian). Update it
       here so hard-coded descriptor arrays remain consistent with sizeof. */
    uint16_t total = USBD_FULL_FS_CfgDescSize;
    USBD_FULL_FS_CfgDesc[2] = (uint8_t)(total & 0xFFU);
    USBD_FULL_FS_CfgDesc[3] = (uint8_t)((total >> 8) & 0xFFU);
    *length = USBD_FULL_FS_CfgDescSize;
    return USBD_FULL_FS_CfgDesc;
  }

  *length = USBD_FS_CfgDescSize;
  return USBD_FS_CfgDesc;
}

#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
/* Provide per-interface strings for HID Player 1 and Player 2
   The host will request these as string descriptor indices (e.g., 0x06,0x07)
   and this function returns the unicode descriptor built into USBD_StrDesc. */
static uint8_t *USBD_COMPOSITE_GetUsrStrDescriptor(USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length)
{
  extern uint8_t USBD_StrDesc[]; /* buffer in usbd_desc.c */
  if (index == 0x06U)
  {
    /* Player 1 */
    (void)USBD_GetString((uint8_t *)"HIDO Arcade Joystick P1", USBD_StrDesc, length);
    return USBD_StrDesc;
  }
  else if (index == 0x07U)
  {
    /* Player 2 */
    (void)USBD_GetString((uint8_t *)"HIDO Arcade Joystick P2", USBD_StrDesc, length);
    return USBD_StrDesc;
  }
  /* Not handled here - let other handlers or default behavior respond */
  *length = 0U;
  return NULL;
}
#endif
