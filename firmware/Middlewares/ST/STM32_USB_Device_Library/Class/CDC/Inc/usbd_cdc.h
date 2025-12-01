/* Minimal CDC header for integration (enhanced) */
#ifndef __USBD_CDC_H__
#define __USBD_CDC_H__

#include "usbd_def.h"

/* Endpoint addresses must match composite descriptor */
#define CDC_CMD_EP               0x83U  /* Notification IN */
#define CDC_OUT_EP               0x03U  /* Data OUT */
#define CDC_IN_EP                0x84U  /* Data IN */

#define CDC_CMD_PACKET_SIZE      16U
#define CDC_DATA_FS_MAX_PACKET_SIZE 64U

/* CDC line coding structure (7 bytes per USB CDC spec) */
typedef struct {
	uint32_t dwDTERate;   /* Baud rate */
	uint8_t  bCharFormat; /* Stop bits: 0=1,1=1.5,2=2 */
	uint8_t  bParityType; /* 0=None,1=Odd,2=Even,3=Mark,4=Space */
	uint8_t  bDataBits;   /* Data bits (5,6,7,8,16) */
} USBD_CDC_LineCodingTypeDef;

extern USBD_ClassTypeDef USBD_CDC;

/* Exported functions used by composite */
uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
/* Explicit post-init to arm CDC OUT EP (called after all composite classes init) */
void USBD_CDC_StartReception(USBD_HandleTypeDef *pdev, void *pCDCClassData);

#endif /* __USBD_CDC_H__ */
