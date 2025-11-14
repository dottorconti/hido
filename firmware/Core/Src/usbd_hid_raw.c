#include "usbd_hid_raw.h"
#include <string.h>

/* reference to the full FS configuration descriptor defined in usbd_desc.c */
extern uint8_t USBD_FS_CfgDesc[];

/* Forward: return full FS configuration descriptor to the USB core */
static uint8_t *USBD_HID_RAW_GetFSConfigDescriptor(uint16_t *length)
{
    /* wTotalLength for FS configuration: 73 bytes (0x49) matching USBD_FS_CfgDesc */
    *length = 0x49;
    return USBD_FS_CfgDesc;
}

// HID RAW Report Descriptor (Usage Page 0xFF00, 64 bytes)
const uint8_t HID_RAW_ReportDesc[HID_RAW_REPORT_DESC_SIZE] = {
    0x06, 0x00, 0xFF,      // USAGE_PAGE (Vendor Defined 0xFF00)
    0x09, 0x01,            // USAGE (Vendor Usage 1)
    0xA1, 0x01,            // COLLECTION (Application)
    0x15, 0x00,            //   LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,      //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,            //   REPORT_SIZE (8)
    0x95, 0x40,            //   REPORT_COUNT (64)
    0x09, 0x01,            //   USAGE (Vendor Usage 1)
    0x81, 0x02,            //   INPUT (Data,Var,Abs)
    0x09, 0x01,            //   USAGE (Vendor Usage 1)
    0x91, 0x02,            //   OUTPUT (Data,Var,Abs)
    0xC0                   // END_COLLECTION
};

// Stub della classe USBD_HID_RAW (deve essere globale, non static)
USBD_ClassTypeDef USBD_HID_RAW = {
    NULL, // Init
    NULL, // DeInit
    NULL, // Setup
    NULL, // EP0_TxSent
    NULL, // EP0_RxReady
    NULL, // DataIn
    NULL, // DataOut
    NULL, // SOF
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    NULL, // GetHSConfigDescriptor
    USBD_HID_RAW_GetFSConfigDescriptor, // GetFSConfigDescriptor
    NULL, // GetOtherSpeedConfigDescriptor
    NULL  // GetDeviceQualifierDescriptor
};

// API di base (stub)
uint8_t USBD_HID_RAW_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len) {
    // TODO: implementare invio report RAW
    return 0;
}

uint8_t USBD_HID_RAW_ReceivePacket(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len) {
    // TODO: implementare ricezione RAW
    return 0;
}
