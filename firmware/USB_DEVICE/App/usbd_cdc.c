/* Minimal CDC class implementation (skeleton) */
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_ctlreq.h"

typedef struct
{
  uint8_t  state;
} USBD_CDC_HandleTypeDef;

static uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  /* NOTE: real implementation should open endpoints and allocate class data */
  pdev->pClassData = USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));
  if (pdev->pClassData == NULL) return USBD_FAIL;
  ((USBD_CDC_HandleTypeDef*)pdev->pClassData)->state = 0;
  return USBD_OK;
}

static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  if (pdev->pClassData) { USBD_free(pdev->pClassData); pdev->pClassData = NULL; }
  return USBD_OK;
}

static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /* Minimal: not handling class requests here; real implementation required for host control */
  USBD_CtlError(pdev, req);
  return USBD_FAIL;
}

static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  return USBD_OK;
}

static uint8_t *USBD_CDC_GetFSCfgDesc(uint16_t *length)
{
  /* Composite descriptor is provided by usbd_desc.c (GetFSConfigDescriptor) */
  *length = 0;
  return NULL;
}

static uint8_t *USBD_CDC_GetHSCfgDesc(uint16_t *length){return NULL;}
static uint8_t *USBD_CDC_GetOtherSpeedCfgDesc(uint16_t *length){return NULL;}
static uint8_t *USBD_CDC_GetDeviceQualifierDesc(uint16_t *length){return NULL;}

static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return USBD_OK;
}

static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return USBD_OK;
}

USBD_ClassTypeDef  USBD_CDC =
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
  USBD_CDC_GetHSCfgDesc,
  USBD_CDC_GetFSCfgDesc,
  USBD_CDC_GetOtherSpeedCfgDesc,
  USBD_CDC_GetDeviceQualifierDesc,
};

uint8_t USBD_CDC_SendPacket(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len)
{
  /* minimal stub */
  return USBD_OK;
}
