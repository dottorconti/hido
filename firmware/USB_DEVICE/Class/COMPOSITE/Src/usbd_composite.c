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
  
  /* Initialize HID class - swap pClassData temporarily */
  pdev->pClassData = hcomp->pHIDClassData;
  if (USBD_HID.Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  hcomp->pHIDClassData = pdev->pClassData; /* Save HID's allocated data */
  
  /* HID RAW disabilitato */
  
  /* Initialize CDC class */
  pdev->pClassData = hcomp->pCDCClassData;
  if (USBD_CDC_Init(pdev, cfgidx) != USBD_OK) ret = USBD_FAIL;
  hcomp->pCDCClassData = pdev->pClassData;
  
  /* Restore composite class data pointer */
  pdev->pClassData = hcomp;
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
  
  /* Try CDC first - restore CDC class data */
  pdev->pClassData = hcomp->pCDCClassData;
  ret = USBD_CDC_Setup(pdev, req);
  hcomp->pCDCClassData = pdev->pClassData; /* Save any updates */
  pdev->pClassData = hcomp; /* Restore composite pointer */
  if (ret == USBD_OK) return USBD_OK;
  
  /* Nessun RAW */
  
  /* Try normal HID */
  pdev->pClassData = hcomp->pHIDClassData;
  ret = USBD_HID.Setup(pdev, req);
  hcomp->pHIDClassData = pdev->pClassData;
  pdev->pClassData = hcomp;
  
  return ret;
}

static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  
  /* Delegate to CDC */
  pdev->pClassData = hcomp->pCDCClassData;
  USBD_CDC_DataIn(pdev, epnum);
  hcomp->pCDCClassData = pdev->pClassData;
  
  /* Nessun RAW */
  
  /* Delegate to normal HID (handles EP 0x81) */
  pdev->pClassData = hcomp->pHIDClassData;
  USBD_HID.DataIn(pdev, epnum);
  hcomp->pHIDClassData = pdev->pClassData;
  
  pdev->pClassData = hcomp;
  return USBD_OK;
}

static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef *)pdev->pClassData;
  
  /* Delegate to CDC (handles EP 0x03) */
  pdev->pClassData = hcomp->pCDCClassData;
  USBD_CDC_DataOut(pdev, epnum);
  hcomp->pCDCClassData = pdev->pClassData;
  
  /* Nessun RAW */
  
  /* Delegate to normal HID */
  pdev->pClassData = hcomp->pHIDClassData;
  USBD_HID.DataOut(pdev, epnum);
  hcomp->pHIDClassData = pdev->pClassData;
  
  pdev->pClassData = hcomp;
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
