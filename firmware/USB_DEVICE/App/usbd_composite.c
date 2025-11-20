/* Composite class: forwards calls to HID and CDC sub-classes */
#include "usbd_composite.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_desc.h"

/* Use board LED to indicate composite init ran */
#include "main.h"

typedef struct {
  void *hidData;
  void *cdcData;
} USBD_COMPOSITE_HandleTypeDef;

static uint8_t COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp;

  /* Allocate composite handle */
  hcomp = (USBD_COMPOSITE_HandleTypeDef*)USBD_malloc(sizeof(USBD_COMPOSITE_HandleTypeDef));
  if (hcomp == NULL) return USBD_FAIL;

  /* Ensure pClassData is NULL before calling sub-inits */
  pdev->pClassData = NULL;

  /* Initialize HID first - HID will set pdev->pClassData to its own handle */
  /* Indicate we're about to call HID Init (short pulse) */
  {
    extern volatile uint32_t hid_led_one_shot_until;
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    hid_led_one_shot_until = HAL_GetTick() + 80;
  }
  if (USBD_HID.Init(pdev, cfgidx) != USBD_OK)
  {
    USBD_free(hcomp);
    return USBD_FAIL;
  }
  /* Indicate HID Init returned (longer pulse) */
  {
    extern volatile uint32_t hid_led_one_shot_until;
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    hid_led_one_shot_until = HAL_GetTick() + 300;
  }
  hcomp->hidData = pdev->pClassData;

  /* Initialize CDC - this will set pdev->pClassData to CDC handle */
  /* Indicate we're about to call CDC Init (short pulse) */
  {
    extern volatile uint32_t cdc_led_one_shot_until;
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    cdc_led_one_shot_until = HAL_GetTick() + 80;
  }
  if (USBD_CDC.Init(pdev, cfgidx) != USBD_OK)
  {
    /* Deinit HID and cleanup */
    pdev->pClassData = hcomp->hidData;
    USBD_HID.DeInit(pdev, cfgidx);
    USBD_free(hcomp);
    return USBD_FAIL;
  }
  /* Indicate CDC Init returned (longer pulse) */
  {
    extern volatile uint32_t cdc_led_one_shot_until;
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    cdc_led_one_shot_until = HAL_GetTick() + 300;
  }
  hcomp->cdcData = pdev->pClassData;

  /* Store composite handle in pClassData for the composite class */
  pdev->pClassData = hcomp;

  /* Enable continuous blink for diagnostics */
  composite_blink_enabled = 1;
  /* Turn LED1 on for a short one-shot to indicate composite init */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
  extern volatile uint32_t composite_led_one_shot_until;
  composite_led_one_shot_until = HAL_GetTick() + 500;
  return USBD_OK;
}

static uint8_t COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef*)pdev->pClassData;
  if (hcomp == NULL) return USBD_OK;

  /* Call HID DeInit with its stored context */
  pdev->pClassData = hcomp->hidData;
  USBD_HID.DeInit(pdev, cfgidx);

  /* Call CDC DeInit with its stored context */
  pdev->pClassData = hcomp->cdcData;
  USBD_CDC.DeInit(pdev, cfgidx);

  /* Free composite handle and clear pClassData */
  USBD_free(hcomp);
  pdev->pClassData = NULL;
  return USBD_OK;
}

static uint8_t COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef*)pdev->pClassData;
  uint8_t res = USBD_FAIL;

  if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
  {
    /* Try HID */
    if (hcomp && hcomp->hidData)
    {
      pdev->pClassData = hcomp->hidData;
      if (USBD_HID.Setup && (USBD_HID.Setup(pdev, req) == USBD_OK)) res = USBD_OK;
    }

    /* Try CDC if HID didn't handle it */
    if (res != USBD_OK && hcomp && hcomp->cdcData)
    {
      pdev->pClassData = hcomp->cdcData;
      if (USBD_CDC.Setup && (USBD_CDC.Setup(pdev, req) == USBD_OK)) res = USBD_OK;
    }

    /* Restore composite handle pointer */
    pdev->pClassData = hcomp;
  }

  return res;
}

static uint8_t COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef*)pdev->pClassData;

  if (hcomp && hcomp->hidData && USBD_HID.EP0_RxReady)
  {
    pdev->pClassData = hcomp->hidData;
    USBD_HID.EP0_RxReady(pdev);
  }

  if (hcomp && hcomp->cdcData && USBD_CDC.EP0_RxReady)
  {
    pdev->pClassData = hcomp->cdcData;
    USBD_CDC.EP0_RxReady(pdev);
  }

  /* Restore composite handle */
  pdev->pClassData = hcomp;
  return USBD_OK;
}

static uint8_t COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef*)pdev->pClassData;

  if (hcomp && hcomp->hidData && USBD_HID.DataIn)
  {
    pdev->pClassData = hcomp->hidData;
    USBD_HID.DataIn(pdev, epnum);
  }

  if (hcomp && hcomp->cdcData && USBD_CDC.DataIn)
  {
    pdev->pClassData = hcomp->cdcData;
    USBD_CDC.DataIn(pdev, epnum);
  }

  /* Restore composite handle */
  pdev->pClassData = hcomp;
  return USBD_OK;
}

static uint8_t COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_COMPOSITE_HandleTypeDef *hcomp = (USBD_COMPOSITE_HandleTypeDef*)pdev->pClassData;

  if (hcomp && hcomp->hidData && USBD_HID.DataOut)
  {
    pdev->pClassData = hcomp->hidData;
    USBD_HID.DataOut(pdev, epnum);
  }

  if (hcomp && hcomp->cdcData && USBD_CDC.DataOut)
  {
    pdev->pClassData = hcomp->cdcData;
    USBD_CDC.DataOut(pdev, epnum);
  }

  /* Restore composite handle */
  pdev->pClassData = hcomp;
  return USBD_OK;
}

static uint8_t *COMPOSITE_GetFSCfgDesc(uint16_t *length)
{
  /* Return the composite FS configuration descriptor defined in usbd_desc.c */
  if (length) *length = USBD_FS_CfgDesc_len;
  return USBD_FS_CfgDesc;
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
