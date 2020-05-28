/*
 * USBComposite.c
 *
 *  Created on: May 27, 2020
 *      Author: epffpe
 */



#include <stdbool.h>

/* Header files */
#include <ti/display/Display.h>


/* POSIX Header files */
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/MutexP.h>
#include <ti/drivers/dpl/SemaphoreP.h>


/* driverlib Header files */
#include "ti/devices/msp432e4/driverlib/driverlib.h"


/* usblib Header files */
#include <ti/usblib/msp432e4/usb-ids.h>
#include <ti/usblib/msp432e4/usblib.h>
#include <ti/usblib/msp432e4/usbcdc.h>
#include <ti/usblib/msp432e4/usbhid.h>
#include <ti/usblib/msp432e4/device/usbdevice.h>
#include <ti/usblib/msp432e4/device/usbdcomp.h>
#include <ti/usblib/msp432e4/device/usbdcdc.h>
#include <ti/usblib/msp432e4/device/usbddfu-rt.h>
#include <ti/usblib/msp432e4/device/usbdhid.h>
#include <ti/usblib/msp432e4/device/usbdmsc.h>
#include <ti/usblib/msp432e4/device/usbdhidmouse.h>

/* Example/Board Header files */
#define __USB_USBCOMPOSITE_GLOBAL
#include "USB/USBComposite.h"
#include "ti_usblib_config.h"

#include "DISPLAY/SMCDisplay.h"

typedef uint32_t            USBCDCDEventType;

/* Defines */

/* Typedefs */
typedef volatile enum {
    USBCDCCOMPOSITE_CONNECTED = 0,
    USBCDCCOMPOSITE_DISCONNECTED
} USBCDCCOMPOSITE_USBState;

typedef volatile enum {
    USBCDCD_STATE_IDLE = 0,
    USBCDCD_STATE_INIT,
    USBCDCD_STATE_UNCONFIGURED
} USBCDCD_USBState;

/* Static variables and handles */
static volatile USBCDCCOMPOSITE_USBState    g_USBCompositeState;
static uint8_t                              descriptorData[DESCRIPTOR_DATA_SIZE_compositeDevice];


typedef struct {
    USBCDCD_USBState    state;
    MutexP_Handle       mutexTxSerial;
    MutexP_Handle       mutexRxSerial;
    MutexP_Handle       mutexUSBWait;
    SemaphoreP_Handle   semTxSerial;
    SemaphoreP_Handle   semRxSerial;
    SemaphoreP_Handle   semUSBConnected;
}USBCDCD_Object;

static volatile USBCDCD_Object g_USBCDCDObjects[USBCDCD_NUM_DEVICES];


/* Function prototypes */
static void USBCDCD_hwiHandler(uintptr_t arg0);
static unsigned int rxData(uint32_t index,
                           unsigned char *pStr,
                           unsigned int length,
                           unsigned int timeout);
static unsigned int txData(uint32_t index,
                           char *pStr,
                           int length, unsigned int timeout);

static tLineCoding g_sLineCoding[USBCDCD_NUM_DEVICES] = {
 [USBCDCD_RemoteControl] = {
                            115200,                     /* 115200 baud rate. */
                            1,                          /* 1 Stop Bit. */
                            0,                          /* No Parity. */
                            8                           /* 8 Bits of data. */
 },
#ifdef USBCDCD_CONSOLE_EN
 [USBCDCD_Console] = {115200, 1, 0, 8},
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
 [USBCDCD_ForteVerifier] = {115200, 1, 0, 8},
#endif
};

/*
 *  ======== cbCompositeHandler ========
 *  Callback handler for the USB stack.
 *
 *  Callback handler call by the USB stack to notify us on what has happened in
 *  regards to the keyboard.
 *
 *  @param(cbData)          A callback pointer provided by the client.
 *
 *  @param(event)           Identifies the event that occurred in regards to
 *                          this device.
 *
 *  @param(eventMsgData)    A data value associated with a particular event.
 *
 *  @param(eventMsgPtr)     A data pointer associated with a particular event.
 *
 */
USBCDCDEventType cbCompositeHandler(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg, void *eventMsgPtr)
{
    /* Determine what event has happened */
    switch (event) {
        case USB_EVENT_CONNECTED:
            if (g_USBCompositeState != USBCDCCOMPOSITE_CONNECTED) {
                g_USBCompositeState = USBCDCCOMPOSITE_CONNECTED;

//                SemaphoreP_post(semUSBConnected);
            }
            break;

        case USB_EVENT_DISCONNECTED:
            g_USBCompositeState = USBCDCCOMPOSITE_DISCONNECTED;
            break;

        default:
            break;
    }

    return (0);
}


/*
 *  ======== cbRxHandler ========
 *  Callback handler for the USB stack.
 *
 *  Callback handler call by the USB stack to notify us on what has happened in
 *  regards to the keyboard.
 *
 *  @param(cbData)          A callback pointer provided by the client.
 *
 *  @param(event)           Identifies the event that occurred in regards to
 *                          this device.
 *
 *  @param(eventMsgData)    A data value associated with a particular event.
 *
 *  @param(eventMsgPtr)     A data pointer associated with a particular event.
 *
 */
USBCDCDEventType cbRxHandler(uint32_t index, void *cbData, USBCDCDEventType event,
                             USBCDCDEventType eventMsg,
                             void *eventMsgPtr)
{
    if (index < USBCDCD_NUM_DEVICES) {
        switch (event) {
        case USB_EVENT_RX_AVAILABLE:
            SemaphoreP_post(g_USBCDCDObjects[index].semRxSerial);
            break;

        case USB_EVENT_DATA_REMAINING:
            break;

        case USB_EVENT_REQUEST_BUFFER:
            break;

        default:
            break;
        }
    }

    return (0);
}

/*
 *  ======== cbSerialHandler ========
 *  Callback handler for the USB stack.
 *
 *  Callback handler call by the USB stack to notify us on what has happened in
 *  regards to the keyboard.
 *
 *  @param(cbData)          A callback pointer provided by the client.
 *
 *  @param(event)           Identifies the event that occurred in regards to
 *                          this device.
 *
 *  @param(eventMsgData)    A data value associated with a particular event.
 *
 *  @param(eventMsgPtr)     A data pointer associated with a particular event.
 *
 */
USBCDCDEventType cbSerialHandler(uint32_t index, void *cbData, USBCDCDEventType event,
                                 USBCDCDEventType eventMsg,
                                 void *eventMsgPtr)
{
    tLineCoding *psLineCoding;
    if (index < USBCDCD_NUM_DEVICES) {
    /* Determine what event has happened */
        switch (event) {
        case USB_EVENT_CONNECTED:
            g_USBCDCDObjects[index].state = USBCDCD_STATE_INIT;
            SemaphoreP_post(g_USBCDCDObjects[index].semUSBConnected);
            break;

        case USB_EVENT_DISCONNECTED:
            g_USBCDCDObjects[index].state = USBCDCD_STATE_UNCONFIGURED;
            break;

        case USBD_CDC_EVENT_GET_LINE_CODING:
            /* Create a pointer to the line coding information. */
            psLineCoding = (tLineCoding *)eventMsgPtr;

            /* Copy the current line coding information into the structure. */
            *(psLineCoding) = g_sLineCoding[index];
            break;

        case USBD_CDC_EVENT_SET_LINE_CODING:
            /* Create a pointer to the line coding information. */
            psLineCoding = (tLineCoding *)eventMsgPtr;

            /*
             * Copy the line coding information into the current line coding
             * structure.
             */
            g_sLineCoding[index] = *(psLineCoding);
            break;

        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            break;

        case USBD_CDC_EVENT_SEND_BREAK:
            break;

        case USBD_CDC_EVENT_CLEAR_BREAK:
            break;

        case USB_EVENT_SUSPEND:
            break;

        case USB_EVENT_RESUME:
            break;

        default:
            break;
        }
    }

    return (0);
}

/*
 *  ======== cbTxHandler ========
 *  Callback handler for the USB stack.
 *
 *  Callback handler call by the USB stack to notify us on what has happened in
 *  regards to the keyboard.
 *
 *  @param(cbData)          A callback pointer provided by the client.
 *
 *  @param(event)           Identifies the event that occurred in regards to
 *                          this device.
 *
 *  @param(eventMsgData)    A data value associated with a particular event.
 *
 *  @param(eventMsgPtr)     A data pointer associated with a particular event.
 *
 */
USBCDCDEventType cbTxHandler(uint32_t index, void *cbData, USBCDCDEventType event,
                             USBCDCDEventType eventMsg,
                             void *eventMsgPtr)
{
    if (index < USBCDCD_NUM_DEVICES) {
        switch (event) {
        case USB_EVENT_TX_COMPLETE:
            /*
             * Data was sent, so there should be some space available on the
             * buffer
             */
            SemaphoreP_post(g_USBCDCDObjects[index].semTxSerial);
            break;

        default:
            break;
        }
    }
    return (0);
}



/*
 * USB CDC 0
 */

USBCDCDEventType cbRxHandler0(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg,
                                    void *eventMsgPtr)
{
    return cbRxHandler(USBCDCD_RemoteControl, cbData, event, eventMsg, eventMsgPtr);
}

USBCDCDEventType cbSerialHandler0(void *cbData, USBCDCDEventType event,
                                        USBCDCDEventType eventMsg,
                                        void *eventMsgPtr)
{
    return cbSerialHandler(USBCDCD_RemoteControl, cbData, event, eventMsg, eventMsgPtr);
}

USBCDCDEventType cbTxHandler0(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg,
                                    void *eventMsgPtr)
{
    return cbTxHandler(USBCDCD_RemoteControl, cbData, event, eventMsg, eventMsgPtr);
}

/*
 * USB CDC 1
 */
#ifdef USBCDCD_CONSOLE_EN
USBCDCDEventType cbRxHandler1(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg,
                                    void *eventMsgPtr)
{
    return cbRxHandler(USBCDCD_Console, cbData, event, eventMsg, eventMsgPtr);
}

USBCDCDEventType cbSerialHandler1(void *cbData, USBCDCDEventType event,
                                        USBCDCDEventType eventMsg,
                                        void *eventMsgPtr)
{
    return cbSerialHandler(USBCDCD_Console, cbData, event, eventMsg, eventMsgPtr);
}

USBCDCDEventType cbTxHandler1(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg,
                                    void *eventMsgPtr)
{
    return cbTxHandler(USBCDCD_Console, cbData, event, eventMsg, eventMsgPtr);
}
#endif
/*
 * USB CDC 2
 */
#ifdef USBCDCD_FORTEVERIFIER_EN
USBCDCDEventType cbRxHandler2(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg,
                                    void *eventMsgPtr)
{
    return cbRxHandler(USBCDCD_ForteVerifier, cbData, event, eventMsg, eventMsgPtr);
}

USBCDCDEventType cbSerialHandler2(void *cbData, USBCDCDEventType event,
                                        USBCDCDEventType eventMsg,
                                        void *eventMsgPtr)
{
    return cbSerialHandler(USBCDCD_ForteVerifier, cbData, event, eventMsg, eventMsgPtr);
}

USBCDCDEventType cbTxHandler2(void *cbData, USBCDCDEventType event,
                                    USBCDCDEventType eventMsg,
                                    void *eventMsgPtr)
{
    return cbTxHandler(USBCDCD_ForteVerifier, cbData, event, eventMsg, eventMsgPtr);
}
#endif
/*
 *  ======== USBCDCD_hwiHandler ========
 *  This function calls the USB library's device interrupt handler.
 */
static void USBCDCD_hwiHandler(uintptr_t arg0)
{
    USB0_IRQDeviceHandler();
}

/*
 *  ======== rxData ========
 */
static unsigned int rxData(uint32_t index,
                           unsigned char *pStr,
                           unsigned int length,
                           unsigned int timeout)
{
    unsigned int read = 0;
    tUSBBuffer *psRxBuffer_serialDevice;
    if (index < USBCDCD_NUM_DEVICES) {
        switch (index) {
        case USBCDCD_RemoteControl:
            psRxBuffer_serialDevice = &g_sRxBuffer_serialDevice0;
            break;
#ifdef USBCDCD_CONSOLE_EN
        case USBCDCD_Console:
            psRxBuffer_serialDevice = &g_sRxBuffer_serialDevice1;
            break;
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
        case USBCDCD_ForteVerifier:
            psRxBuffer_serialDevice = &g_sRxBuffer_serialDevice2;
            break;
#endif
        default:
            psRxBuffer_serialDevice = &g_sRxBuffer_serialDevice0;
            break;
        }
        if (USBBufferDataAvailable(psRxBuffer_serialDevice) || !(SemaphoreP_pend(g_USBCDCDObjects[index].semRxSerial, timeout) == SemaphoreP_TIMEOUT)) {
            read = USBBufferRead(psRxBuffer_serialDevice, pStr, length);
        }
    }

    return (read);
}


/*
 *  ======== txData ========
 */
static unsigned int txData(uint32_t index,
                           char *pStr,
                           int length, unsigned int timeout)
{
    unsigned int buffAvailSize;
    unsigned int bufferedCount = 0;
    unsigned int sendCount = 0;
    unsigned char *sendPtr;
    tUSBBuffer *psTxBuffer_serialDevice;

    if (index < USBCDCD_NUM_DEVICES) {
        switch (index) {
        case USBCDCD_RemoteControl:
            psTxBuffer_serialDevice = &g_sTxBuffer_serialDevice0;
            break;
#ifdef USBCDCD_CONSOLE_EN
        case USBCDCD_Console:
            psTxBuffer_serialDevice = &g_sTxBuffer_serialDevice1;
            break;
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
        case USBCDCD_ForteVerifier:
            psTxBuffer_serialDevice = &g_sTxBuffer_serialDevice2;
            break;
#endif
        default:
            psTxBuffer_serialDevice = &g_sTxBuffer_serialDevice0;
            break;
        }
        while (bufferedCount != length) {
            /* Determine the buffer size available */
            buffAvailSize = USBBufferSpaceAvailable(psTxBuffer_serialDevice);

            /* Determine how much needs to be sent */
            if ((length - bufferedCount) > buffAvailSize) {
                sendCount = buffAvailSize;
            }
            else {
                sendCount = length - bufferedCount;
            }

            /* Adjust the pointer to the data */
            sendPtr = (unsigned char *)pStr + bufferedCount;

            /* Place the contents into the USB BUffer */
            bufferedCount += USBBufferWrite(psTxBuffer_serialDevice, sendPtr, sendCount);

            /* Pend until some data was sent through the USB*/
            if (SemaphoreP_pend(g_USBCDCDObjects[index].semTxSerial, timeout) == SemaphoreP_TIMEOUT) {
                break;
            }
        }

    }

    return (bufferedCount);
}


/*
 *  ======== USBCDCD_receiveData ========
 */
unsigned int USBCDCD_receiveData(uint32_t index,
                                 unsigned char *pStr,
                                 unsigned int length,
                                 unsigned int timeout)
{
    unsigned int retValue = 0;
    unsigned int key;

    if (index < USBCDCD_NUM_DEVICES) {
        switch (g_USBCDCDObjects[index].state) {
        case USBCDCD_STATE_UNCONFIGURED:
            USBCDCD_waitForConnect(index, timeout);
            break;

        case USBCDCD_STATE_INIT:
            /* Acquire lock */
            key = MutexP_lock(g_USBCDCDObjects[index].mutexRxSerial);
            switch(index) {
            case USBCDCD_RemoteControl:
                USBBufferInit(&g_sTxBuffer_serialDevice0);
                USBBufferInit(&g_sRxBuffer_serialDevice0);
                break;
#ifdef USBCDCD_CONSOLE_EN
            case USBCDCD_Console:
                USBBufferInit(&g_sTxBuffer_serialDevice1);
                USBBufferInit(&g_sRxBuffer_serialDevice1);
                break;
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
            case USBCDCD_ForteVerifier:
                USBBufferInit(&g_sTxBuffer_serialDevice2);
                USBBufferInit(&g_sRxBuffer_serialDevice2);
                break;
#endif
            default:
                USBBufferInit(&g_sTxBuffer_serialDevice0);
                USBBufferInit(&g_sRxBuffer_serialDevice0);
#ifdef USBCDCD_CONSOLE_EN
                USBBufferInit(&g_sTxBuffer_serialDevice1);
                USBBufferInit(&g_sRxBuffer_serialDevice1);
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
                USBBufferInit(&g_sTxBuffer_serialDevice2);
                USBBufferInit(&g_sRxBuffer_serialDevice2);
#endif
                break;

            }

            g_USBCDCDObjects[index].state = USBCDCD_STATE_IDLE;

            retValue = rxData(index, pStr, length, timeout);

            /* Release lock */
            MutexP_unlock(g_USBCDCDObjects[index].mutexRxSerial, key);
            break;

        case USBCDCD_STATE_IDLE:
            /* Acquire lock */
            key = MutexP_lock(g_USBCDCDObjects[index].mutexRxSerial);

            retValue = rxData(index, pStr, length, timeout);

            /* Release lock */
            MutexP_unlock(g_USBCDCDObjects[index].mutexRxSerial, key);
            break;

        default:
            break;
        }
    }

    return (retValue);
}

/*
 *  ======== USBCDCD_sendData ========
 */
unsigned int USBCDCD_sendData(uint32_t index,
                              char *pStr,
                              unsigned int length,
                              unsigned int timeout)
{
    unsigned int retValue = 0;
    unsigned int key;

    if (index < USBCDCD_NUM_DEVICES) {
        switch (g_USBCDCDObjects[index].state) {
        case USBCDCD_STATE_UNCONFIGURED:
            USBCDCD_waitForConnect(index, timeout);
            break;

        case USBCDCD_STATE_INIT:
            /* Acquire lock */
            key = MutexP_lock(g_USBCDCDObjects[index].mutexTxSerial);
            switch(index) {
            case USBCDCD_RemoteControl:
                USBBufferInit(&g_sTxBuffer_serialDevice0);
                USBBufferInit(&g_sRxBuffer_serialDevice0);
                break;
#ifdef USBCDCD_CONSOLE_EN
            case USBCDCD_Console:
                USBBufferInit(&g_sTxBuffer_serialDevice1);
                USBBufferInit(&g_sRxBuffer_serialDevice1);
#endif
                break;
#ifdef USBCDCD_FORTEVERIFIER_EN
            case USBCDCD_ForteVerifier:
                USBBufferInit(&g_sTxBuffer_serialDevice2);
                USBBufferInit(&g_sRxBuffer_serialDevice2);
                break;
#endif
            default:
                USBBufferInit(&g_sTxBuffer_serialDevice0);
                USBBufferInit(&g_sRxBuffer_serialDevice0);
#ifdef USBCDCD_CONSOLE_EN
                USBBufferInit(&g_sTxBuffer_serialDevice1);
                USBBufferInit(&g_sRxBuffer_serialDevice1);
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
                USBBufferInit(&g_sTxBuffer_serialDevice2);
                USBBufferInit(&g_sRxBuffer_serialDevice2);
#endif
                break;
            }

            g_USBCDCDObjects[index].state = USBCDCD_STATE_IDLE;

            retValue = txData(index, pStr, length, timeout);

            /* Release lock */
            MutexP_unlock(g_USBCDCDObjects[index].mutexTxSerial, key);
            break;

        case USBCDCD_STATE_IDLE:
            /* Acquire lock */
            key = MutexP_lock(g_USBCDCDObjects[index].mutexTxSerial);

            retValue = txData(index, pStr, length, timeout);

            /* Release lock */
            MutexP_unlock(g_USBCDCDObjects[index].mutexTxSerial, key);
            break;

        default:
            break;
        }
    }

    return (retValue);
}

/*
 *  ======== USBCDCD_waitForConnect ========
 */
bool USBCDCD_waitForConnect(uint32_t index, unsigned int timeout)
{
    bool ret = true;
    unsigned int key;

    if (index < USBCDCD_NUM_DEVICES) {
        /* Need exclusive access to prevent a race condition */
        key = MutexP_lock(g_USBCDCDObjects[index].mutexUSBWait);

        if (g_USBCDCDObjects[index].state == USBCDCD_STATE_UNCONFIGURED) {
            if (SemaphoreP_pend(g_USBCDCDObjects[index].semUSBConnected, timeout)== SemaphoreP_TIMEOUT) {
                ret = false;
            }
        }

        MutexP_unlock(g_USBCDCDObjects[index].mutexUSBWait, key);
    }else {
        ret = false;
    }
    return (ret);
}


//#define DESCRIPTOR_BUFFER_SIZE  (COMPOSITE_DDFU_SIZE + COMPOSITE_DHID_SIZE)


//*****************************************************************************
//
// This is the callback from the USB DFU runtime interface driver.
//
// \param pvCBData is ignored by this function.
// \param ui32Event is one of the valid events for a DFU device.
// \param ui32MsgParam is defined by the event that occurs.
// \param pvMsgData is a pointer to data that is defined by the event that
// occurs.
//
// This function will be called to inform the application when a change occurs
// during operation as a DFU device.  Currently, the only event passed to this
// callback is USBD_DFU_EVENT_DETACH which tells the recipient that they should
// pass control to the boot loader at the earliest, non-interrupt context
// point.
//
// \return This function will return 0.
//
//*****************************************************************************
uint32_t DFUDetachCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData,
                           void *pvMsgData)
{
    if(ui32Event == USBD_DFU_EVENT_DETACH)
    {
        //
        // Set the flag that the main loop uses to determine when it is time
        // to transfer control back to the boot loader.  Note that we
        // absolutely DO NOT call USBDDFUUpdateBegin() here since we are
        // currently in interrupt context and this would cause bad things to
        // happen (and the boot loader to not work).
        //
//        g_bUpdateSignalled = true;
    }

    return(0);
}

//*****************************************************************************
//
// The DFU runtime interface initialization and customization structures
//
//*****************************************************************************
tUSBDDFUDevice g_sDFUDevice =
{
    DFUDetachCallback,
    (void *)&g_sDFUDevice
};


uint32_t USBDMSCEventCallback(void *pvCBData, uint32_t ui32Event,
                              uint32_t ui32MsgParam, void *pvMsgData)
{
    switch(ui32Event)
    {
        //
        // Writing to the device.
        //
        case USBD_MSC_EVENT_WRITING:
        {
            //
            //  Handle write case.
            //


            break;
        }

        //
        // Reading from the device.
        //
        case USBD_MSC_EVENT_READING:
        {
            //
            //  Handle read case.
            //


            break;
        }
        case USBD_MSC_EVENT_IDLE:
        {
            //
            //  Handle idle case.
            //

        }
        default:
        {
            break;
        }
    }
    return(0);
}



/*
 *  ======== USBComposite_init ========
 */
void USBComposite_init(bool usbInternal)
{
    HwiP_Handle hwi;
    uint32_t ui32ULPI;
    uint32_t ui32PLLRate;
    uint32_t index;

//    Display_Handle display;
//
//    Display_init();
//
//    /* Open the display for output */
//    display = Display_open(Display_Type_UART, NULL);

//    if (display == NULL) {
//        /* Failed to open display driver */
//        while (1);
//    }

    /* Install interrupt handler */
    hwi = HwiP_create(INT_USB0, USBCDCD_hwiHandler, NULL);
    if (hwi == NULL) {
        Display_printf(g_SMCDisplay, 0, 0, "Can't create USB Hwi.\n");
        while(1);
    }

    for (index = 0; index < USBCDCD_NUM_DEVICES; index++) {
        /* RTOS primitives */
        g_USBCDCDObjects[0].semTxSerial = SemaphoreP_createBinary(0);
        if (g_USBCDCDObjects[0].semTxSerial == NULL) {
            Display_printf(g_SMCDisplay, 0, 0, "Can't create TX semaphore.\n");
            while(1);
        }

        g_USBCDCDObjects[0].semRxSerial = SemaphoreP_createBinary(0);
        if (g_USBCDCDObjects[0].semRxSerial == NULL) {
            Display_printf(g_SMCDisplay, 0, 0, "Can't create RX semaphore.\n");
            while(1);
        }

        g_USBCDCDObjects[0].semUSBConnected = SemaphoreP_createBinary(0);
        if (g_USBCDCDObjects[0].semUSBConnected == NULL) {
            Display_printf(g_SMCDisplay, 0, 0, "Can't create USB semaphore.\n");
            while(1);
        }

        g_USBCDCDObjects[0].mutexTxSerial = MutexP_create(NULL);
        if (g_USBCDCDObjects[0].mutexTxSerial == NULL) {
            Display_printf(g_SMCDisplay, 0, 0, "Can't create TX mutex.\n");
            while(1);
        }

        g_USBCDCDObjects[0].mutexRxSerial = MutexP_create(NULL);
        if (g_USBCDCDObjects[0].mutexRxSerial == NULL) {
            Display_printf(g_SMCDisplay, 0, 0, "Can't create RX mutex.\n");
            while(1);
        }

        g_USBCDCDObjects[0].mutexUSBWait = MutexP_create(NULL);
        if (g_USBCDCDObjects[0].mutexUSBWait == NULL) {
            Display_printf(g_SMCDisplay, 0, 0, "Could not create USB Wait mutex.\n");
            while(1);
        }
        /* State specific variables */
        g_USBCDCDObjects[0].state = USBCDCD_STATE_UNCONFIGURED;
    }

    /* State specific variables */
    g_USBCompositeState = USBCDCCOMPOSITE_DISCONNECTED;

    /* Check if the ULPI mode is to be used or not */
    if(!(usbInternal)) {
        ui32ULPI = USBLIB_FEATURE_ULPI_HS;
        USBDCDFeatureSet(0, USBLIB_FEATURE_USBULPI, &ui32ULPI);
    }

    /* Set the USB stack mode to Device mode with VBUS monitoring */
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    /*Gets the VCO frequency of 120MHZ */
    SysCtlVCOGet(SYSCTL_XTAL_25MHZ, &ui32PLLRate);

    /*Set the PLL*/
    USBDCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &ui32PLLRate);

//    /*
//     * Pass our device information to the USB HID device class driver,
//     * initialize the USB controller and connect the device to the bus.
//     */
//    if (!USBDCDCInit(0, &serialDevice0)) {
//        Display_printf(g_SMCDisplay, 0, 0, "Error initializing the serial device.\n");
//        while(1);
//    }
////    Display_close(g_SMCDisplay);

    /* Install the composite instances */

//    if (!USBDCDCCompositeInit(0, &serialDevice0, &compositeDevice_entries[1]))
    if (!USBDCDCCompositeInit(0, &serialDevice0, &compositeDevice_entries[0]))
    {
        //Can't initialize CDC composite component
        Display_printf(g_SMCDisplay, 0, 0, "Can't initialize CDC composite component.\n");
        while(1);
    }
#ifdef USBCDCD_CONSOLE_EN
    if (!USBDCDCCompositeInit(0, &serialDevice1, &compositeDevice_entries[1]))
    {
        //Can't initialize CDC composite component
        Display_printf(g_SMCDisplay, 0, 0, "Can't initialize CDC composite component.\n");
        while(1);
    }
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
    if (!USBDCDCCompositeInit(0, &serialDevice2, &compositeDevice_entries[2]))
    {
        //Can't initialize CDC composite component
        Display_printf(g_SMCDisplay, 0, 0, "Can't initialize CDC composite component.\n");
        while(1);
    }
#endif

//    USBDMSCCompositeInit(0, &g_sMSCDevice, &compositeDevice_entries[1]);
//    USBDDFUCompositeInit(0, &g_sDFUDevice, &compositeDevice_entries[2]);

    if (!compositeDevice_entries[0].pvInstance
#ifdef USBCDCD_CONSOLE_EN
            || !compositeDevice_entries[1].pvInstance
#endif
#ifdef USBCDCD_FORTEVERIFIER_EN
            || !compositeDevice_entries[2].pvInstance
#endif
            )
//    if (!compositeDevice_entries[0].pvInstance)
    {
        //Can't initialize Error initializing the composite device
        Display_printf(g_SMCDisplay, 0, 0, "Error initializing the composite device.\n");
        while(1);
    }

//    Display_close(display);

    /* Initialize the USB stack with the composite device */
    USBDCompositeInit(0, (tUSBDCompositeDevice *) &compositeDevice,
                      DESCRIPTOR_DATA_SIZE_compositeDevice, descriptorData);

}


