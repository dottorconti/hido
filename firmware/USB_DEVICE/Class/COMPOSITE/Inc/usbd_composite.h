/* Minimal composite class header */
#ifndef __USBD_COMPOSITE_H__
#define __USBD_COMPOSITE_H__

#include "usbd_def.h"

/* Composite class needs to manage separate class data for each sub-class */
typedef struct
{
  void *pHIDClassData;   /* HID class data */
  void *pCDCClassData;   /* CDC class data */
} USBD_COMPOSITE_HandleTypeDef;

extern USBD_ClassTypeDef USBD_COMPOSITE;

/* Wrapper to send HID report through composite (switches pClassData correctly) */
uint8_t USBD_COMPOSITE_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);

#endif /* __USBD_COMPOSITE_H__ */
