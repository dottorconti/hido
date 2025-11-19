#include "usbd_hid_raw.h"
#include "usbd_hid.h"
#include "usbd_ctlreq.h"
#include "usbd_hid_custom.h"
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

/* Forward declarations */
static uint8_t USBD_HID_RAW_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_RAW_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_RAW_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_HID_RAW_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

/* HID RAW class object */
USBD_ClassTypeDef USBD_HID_RAW = {
    USBD_HID_RAW_Init,
    USBD_HID_RAW_DeInit,
    USBD_HID_RAW_Setup,
    NULL, // EP0_TxSent
    NULL, // EP0_RxReady
    USBD_HID_RAW_DataIn, // DataIn
    NULL, // DataOut
    NULL, // SOF
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    NULL, // GetHSConfigDescriptor
    USBD_HID_RAW_GetFSConfigDescriptor, // GetFSConfigDescriptor
    NULL, // GetOtherSpeedConfigDescriptor
    NULL  // GetDeviceQualifierDescriptor
};

/* Simple HID descriptor for RAW interface (bNumDescriptors = 1, report desc size follows) */
__ALIGN_BEGIN static uint8_t USBD_HID_RAW_Desc[9] __ALIGN_END = {
    0x09, /* bLength */
    HID_DESCRIPTOR_TYPE,
    0x11, 0x01, /* bcdHID */
    0x00, /* bCountryCode */
    0x01, /* bNumDescriptors */
    0x22, /* bDescriptorType (Report) */
    0x00, /* placeholder LSB for wDescriptorLength */
    0x00  /* placeholder MSB */
};

/* Basic class data struct allocation helper */
static uint8_t raw_class_mem[sizeof(USBD_HID_HandleTypeDef)];

// API di base (stub)
uint8_t USBD_HID_RAW_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len) {
    // TODO: implementare invio report RAW
    return 0;
}

uint8_t USBD_HID_RAW_ReceivePacket(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len) {
    // TODO: implementare ricezione RAW
    return 0;
}

/* Minimal Init: open IN endpoint for RAW HID */
static uint8_t USBD_HID_RAW_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    UNUSED(cfgidx);
    USBD_LL_OpenEP(pdev, HID_RAW_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_RAW_EPIN_SIZE);
    pdev->ep_in[HID_RAW_EPIN_ADDR & 0xFU].is_used = 1U;

    /* Initialize local class state */
    ((USBD_HID_HandleTypeDef *)raw_class_mem)->state = HID_IDLE;
    return USBD_OK;
}

static uint8_t USBD_HID_RAW_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    UNUSED(cfgidx);
    USBD_LL_CloseEP(pdev, HID_RAW_EPIN_ADDR);
    pdev->ep_in[HID_RAW_EPIN_ADDR & 0xFU].is_used = 0U;
    return USBD_OK;
}

/* Minimal Setup handler to serve Report and HID descriptors */
static uint8_t USBD_HID_RAW_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    uint8_t *pbuf = NULL;
    uint16_t len = 0U;

    if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
        if (req->bRequest == USB_REQ_GET_DESCRIPTOR)
        {
            if (req->wValue >> 8 == HID_REPORT_DESC)
            {
                /* If the host requests the report descriptor for interface 1
                   (player 2), return the P2 joystick report descriptor. */
                if (req->wIndex == 1)
                {
                    len = MIN((uint16_t)HID_REPORT_DESC_SIZE_CUSTOM_P2, req->wLength);
                    pbuf = (uint8_t *)HID_JOYSTICK_ReportDesc_P2;
                    USBD_CtlSendData(pdev, pbuf, len);
                    return USBD_OK;
                }
                /* Fallback: serve the raw report descriptor */
                len = MIN(HID_RAW_REPORT_DESC_SIZE, req->wLength);
                pbuf = (uint8_t *)HID_RAW_ReportDesc;
                USBD_CtlSendData(pdev, pbuf, len);
                return USBD_OK;
            }
            else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
            {
                /* Build a HID descriptor on the stack with the correct report length
                   depending on the interface index requested by the host. */
                uint8_t hid_desc[9];
                memcpy(hid_desc, USBD_HID_RAW_Desc, sizeof(hid_desc));
                if (req->wIndex == 1)
                {
                    uint16_t rlen = HID_REPORT_DESC_SIZE_CUSTOM_P2;
                    hid_desc[7] = (uint8_t)(rlen & 0xFF);
                    hid_desc[8] = (uint8_t)((rlen >> 8) & 0xFF);
                }
                else
                {
                    uint16_t rlen = HID_RAW_REPORT_DESC_SIZE;
                    hid_desc[7] = (uint8_t)(rlen & 0xFF);
                    hid_desc[8] = (uint8_t)((rlen >> 8) & 0xFF);
                }
                pbuf = hid_desc;
                len = MIN((uint16_t)sizeof(hid_desc), req->wLength);
                USBD_CtlSendData(pdev, pbuf, len);
                return USBD_OK;
            }
            /* handled above */
        }
    }
    /* Not handled: return fail so other classes may try */
    return USBD_FAIL;
}

static uint8_t USBD_HID_RAW_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    /* Simple: mark state idle */
    ((USBD_HID_HandleTypeDef *)raw_class_mem)->state = HID_IDLE;
    return USBD_OK;
}
