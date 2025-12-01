/* Minimal composite class implementation that delegates to HID and CDC */
#include "usbd_composite.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
/* HID RAW rimosso: nessun include aggiuntivo */

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
  USBD_COMPOSITE_HandleTypeDef *hcomp;
  
  /* Allocate composite class data structure */
  pdev->pClassData = USBD_malloc(sizeof(USBD_COMPOSITE_HandleTypeDef));
  if (pdev->pClassData == NULL)
  {
    return USBD_FAIL;
  }
  hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  hcomp->pHIDClassData = NULL;
  hcomp->pCDCClassData = NULL;
  /* Nessun HID RAW */
  
  /* Initialize HID class FIRST - swap pClassData temporarily */
  pdev->pClassData = hcomp->pHIDClassData;
  if (USBD_HID.Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  hcomp->pHIDClassData = pdev->pClassData; /* Save HID's allocated data */
  
  /* HID RAW disabilitato */
  
  /* Initialize CDC class SECOND (defers PrepareReceive) */
  pdev->pClassData = hcomp->pCDCClassData;
  if (USBD_CDC_Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  hcomp->pCDCClassData = pdev->pClassData;
  
  /* Restore composite class data pointer */
  pdev->pClassData = hcomp;
  
  /* CRITICAL POST-INIT: Now that all endpoints opened safely, arm CDC OUT.
     Calling USBD_CDC_StartReception after both HID and CDC Init completed
     prevents race condition where CDC PrepareReceive corrupts HID state. */
  USBD_CDC_StartReception(pdev, hcomp->pCDCClassData);
  
  return ret;
}

static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = USBD_OK;
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  
  if (hcomp != NULL)
  {
    /* DeInit HID - restore its class data */
    pdev->pClassData = hcomp->pHIDClassData;
    if (USBD_HID.DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
    
    /* DeInit HID RAW if present */
    /* Nessuna de-initializzazione RAW */
    
    /* DeInit CDC */
    pdev->pClassData = hcomp->pCDCClassData;
    if (USBD_CDC_DeInit(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
    
    /* Free composite structure */
    USBD_free(hcomp);
    pdev->pClassData = NULL;
  }
  return ret;
}

static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  uint8_t ret = USBD_OK;
  uint8_t recipient = req->bmRequest & 0x1FU;
  uint16_t wIndex = req->wIndex & 0xFFU;
  
  /* Route by interface number or EP address to avoid cross-class pollution */
  if (recipient == 0x01U) { /* INTERFACE */
    if (wIndex == 1U || wIndex == 2U) {
      /* IF1=CDC Control, IF2=CDC Data → route to CDC */
      pdev->pClassData = hcomp->pCDCClassData;
      ret = USBD_CDC_Setup(pdev, req);
      hcomp->pCDCClassData = pdev->pClassData;
      pdev->pClassData = hcomp;
      return ret;
    } else if (wIndex == 0U) {
      /* IF0=HID → route to HID */
      pdev->pClassData = hcomp->pHIDClassData;
      ret = USBD_HID.Setup(pdev, req);
      hcomp->pHIDClassData = pdev->pClassData;
      pdev->pClassData = hcomp;
      return ret;
    }
  }
  
  /* Fallback: try CDC first (handles class-specific on IF1/IF2) */
  pdev->pClassData = hcomp->pCDCClassData;
  ret = USBD_CDC_Setup(pdev, req);
  hcomp->pCDCClassData = pdev->pClassData;
  pdev->pClassData = hcomp;
  if (ret == USBD_OK) return USBD_OK;
  
  /* Then try HID */
  pdev->pClassData = hcomp->pHIDClassData;
  ret = USBD_HID.Setup(pdev, req);
  hcomp->pHIDClassData = pdev->pClassData;
  pdev->pClassData = hcomp;
  
  return ret;
}

static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  
  /* Route by EP number (without direction bit): 0x01=HID, 0x04=CDC Data IN, 0x03=CDC CMD */
  if (epnum == 0x01U) {
    /* HID IN endpoint (EP1) */
    pdev->pClassData = hcomp->pHIDClassData;
    USBD_HID.DataIn(pdev, epnum);
    hcomp->pHIDClassData = pdev->pClassData;
  } else if (epnum == 0x04U || epnum == 0x03U) {
    /* CDC endpoints (EP4 data IN, EP3 command/notification) */
    pdev->pClassData = hcomp->pCDCClassData;
    USBD_CDC_DataIn(pdev, epnum);
    hcomp->pCDCClassData = pdev->pClassData;
  }
  /* Ignore unknown EP */
  
  pdev->pClassData = hcomp;
  return USBD_OK;
}

static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  
  /* Route by EP: 0x03=CDC Data OUT */
  if (epnum == 0x03U) {
    /* CDC OUT endpoint */
    pdev->pClassData = hcomp->pCDCClassData;
    USBD_CDC_DataOut(pdev, epnum);
    hcomp->pCDCClassData = pdev->pClassData;
  }
  /* HID has no OUT endpoints in this config, ignore other EP */
  
  pdev->pClassData = hcomp;
  return USBD_OK;
}

static uint8_t *USBD_COMPOSITE_GetFSConfigDescriptor(uint16_t *length)
{
  /* Return full composite descriptor (HID + CDC) */
  if (USBD_FULL_FS_CfgDescSize != 0)
  {
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

/* Wrapper function for application to send HID reports through composite.
   CRITICAL: arcade_joystick.c calls USBD_HID_SendReport, but when composite
   is active, pdev->pClassData points to COMPOSITE handle, NOT HID handle.
   This causes hhid->state check to access garbage memory and block transmission.
   Solution: provide wrapper that switches pClassData temporarily. */
uint8_t USBD_COMPOSITE_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  if (hcomp == NULL || hcomp->pHIDClassData == NULL) return USBD_FAIL;
  
  /* Temporarily switch pClassData to HID handle */
  pdev->pClassData = hcomp->pHIDClassData;
  
  /* Call real HID SendReport (now hhid pointer will be valid) */
  extern uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);
  uint8_t ret = USBD_HID_SendReport(pdev, report, len);
  
  /* Restore composite handle */
  pdev->pClassData = hcomp;
  
  return ret;
}
