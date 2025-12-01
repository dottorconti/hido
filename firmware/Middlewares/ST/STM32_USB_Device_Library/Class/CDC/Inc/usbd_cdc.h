/* Minimal CDC header for integration */
#ifndef __USBD_CDC_H__
#define __USBD_CDC_H__

#include "usbd_def.h"

extern USBD_ClassTypeDef USBD_CDC;

/* Exported functions used by composite */
uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

#endif /* __USBD_CDC_H__ */
