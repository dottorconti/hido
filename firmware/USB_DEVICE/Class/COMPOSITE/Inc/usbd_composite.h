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

#endif /* __USBD_COMPOSITE_H__ */
