/*
 * Devices.c
 *
 *  Created on: Mar 19, 2018
 *      Author: epenate
 */



#define __DEVICES_GLOBAL
#include "includes.h"
/*
 * The blocksize needs to be a multiple of the worst-case structure alignment size.
 * And bufSize should be equal to blockSize * numBlocks. The worst-case structure alignment
 * is target dependent. On C6x and ARM devices, this value is 8. The base address of the
 * buffer should also be aligned to this same size.
 */
static char *g_aDevicesListBuf[DEVICES_MAX_NUM_LIST * DEVICES_LIST_BLOCK_SIZE];
HeapBuf_Handle g_hDevicesListHeap;




const char *g_Device_nameByType[DEVICE_TYPE_COUNT] =
{
 [DEVICE_TYPE_TEST] = "DEVICE_TYPE_TEST",
 [DEVICE_TYPE_ALTO_AMP] = "DEVICE_TYPE_ALTO_AMP",
 [DEVICE_TYPE_ALTO_MULTINET] = "DEVICE_TYPE_ALTO_MULTINET",
 [DEVICE_TYPE_TCP_REMOTE_CONTROLLER_BINARY] = "DEVICE_TYPE_TCP_REMOTE_CONTROLLER_BINARY",
 [DEVICE_TYPE_TCP_REMOTE_CONTROLLER_ASCII] = "DEVICE_TYPE_TCP_REMOTE_CONTROLLER_ASCII",
 [DEVICE_TYPE_GENERIC] = "DEVICE_TYPE_GENERIC",
};


//Queue_Handle g_hDevicesListQ;

//const uint32_t g_aDevice_MsgSizeTable[] = {
//                                   [APP_MSG_SERVICE_WRITE] = sizeof(device_msg_t) + sizeof(button_state_t),
//                                   [APP_MSG_SERVICE_CFG] = sizeof(device_msg_t),
//                                   [2 ... 10] = 55,
//};
//
//#define DEVICE_MSG_TABLE_SIZE   (sizeof(g_aDevice_MsgSizeTable)/sizeof(g_aDevice_MsgSizeTable[0]))

void vDevice_init()
{
    HeapBuf_Params prms;
    Error_Block eb;

    Error_init(&eb);
    HeapBuf_Params_init(&prms);
    prms.blockSize = DEVICES_LIST_BLOCK_SIZE;
    prms.numBlocks = DEVICES_MAX_NUM_LIST;
    prms.buf = g_aDevicesListBuf;
    prms.bufSize = DEVICES_MAX_NUM_LIST * DEVICES_LIST_BLOCK_SIZE;
    g_hDevicesListHeap = HeapBuf_create(&prms, &eb);
    if (g_hDevicesListHeap == NULL) {
        System_abort("HeapBuf create failed");
    }else {
        g_hDevicesListQ = Queue_create(NULL, NULL);
    }
}

/*
 * Needs to replace fxn, params and address by Device_Params
 * Device_Params will include:
 *      address
 *      events
 *      pointer to table of functions that will know how to interact with this device
 *          Send message
 *          add device
 *          remove device
 */
//DeviceList_Handler xDevices_add(Task_FuncPtr fxn, Task_Params *params, uint32_t address, Error_Block *eb)
DeviceList_Handler xDevice_add(Device_Params *params, Error_Block *eb)
{
    DevicesList_t *pDeviceList;

    pDeviceList = (DevicesList_t *) HeapBuf_alloc(g_hDevicesListHeap, DEVICES_LIST_BLOCK_SIZE, 0, eb);
    if (pDeviceList) {
        Queue_put(g_hDevicesListQ, &pDeviceList->elem);
//        pDeviceList->addr = params->address;
//        params->arg1 = (UArg)pDeviceList;
        pDeviceList->fxnTablePtr = params->fxnTablePtr;
        pDeviceList->deviceType = params->deviceType;
        pDeviceList->deviceID = params->deviceID;
        if (params->fxnTablePtr->openFxn) {
            params->fxnTablePtr->openFxn(pDeviceList, params);
        }
//        pDeviceList->taskHandle = Task_create(fxn, params, eb);

    }
    return pDeviceList;
}

void vDevice_remove(DeviceList_Handler handle)
{
//    unsigned int key;
    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }

    if (handle->fxnTablePtr->closeFxn) {
        handle->fxnTablePtr->closeFxn(handle);
    }
//    key = Hwi_disable();
//    Queue_remove(&handle->elem);
//    Hwi_restore(key);
    HeapBuf_free(g_hDevicesListHeap, handle, DEVICES_LIST_BLOCK_SIZE);
}




/******************************************************************************
 *****************************************************************************
 *
 *  Utility functions
 *
 ****************************************************************************
 *****************************************************************************/

/*
 * @brief  Generic message constructor for characteristic data.
 *
 *         Sends a message to the application for handling in Task context where
 *         the message payload is a char_data_t struct.
 *
 *         From service callbacks the appMsgType is APP_MSG_SERVICE_WRITE or
 *         APP_MSG_SERVICE_CFG, and functions running in another context than
 *         the Task itself, can set the type to APP_MSG_UPDATE_CHARVAL to
 *         make the user Task loop invoke user_updateCharVal function for them.
 *
 * @param  appMsgType    Enumerated type of message being sent.
 * @param  connHandle    GAP Connection handle of the relevant connection
 * @param  serviceUUID   16-bit part of the relevant service UUID
 * @param  paramID       Index of the characteristic in the service
 * @oaram  *pValue       Pointer to characteristic value
 * @param  len           Length of characteristic data
 */
void vDevice_enqueueCharDataMsg( DeviceList_Handler devHandle,
                                 device_msg_types_t appMsgType,
                                 int32_t connHandle,
                                 uint16_t serviceUUID, uint8_t paramID,
                                 uint32_t retDeviceID,
                                 uint16_t retServiceUUID, uint8_t retParamID,
                                 uint8_t *pValue, uint16_t len )
{
    Error_Block eb;

    ASSERT(devHandle != NULL);

    if (devHandle == NULL) {
        return;
    }

    Error_init(&eb);
    // Called in Stack's Task context, so can't do processing here.
    // Send message to application message queue about received data.
    uint16_t readLen = len; // How much data was written to the attribute

    // Allocate memory for the message.
    // Note: The pCharData message doesn't have to contain the data itself, as
    //       that's stored in a variable in the service implementation.
    //
    //       However, to prevent data loss if a new value is received before the
    //       service's container is read out via the GetParameter API is called,
    //       we copy the characteristic's data now.
//    device_msg_t *pMsg = ICall_malloc( sizeof(device_msg_t) + sizeof(char_data_t) +
//                                       readLen );
    device_msg_t *pMsg = Memory_alloc(NULL, sizeof(device_msg_t) + sizeof(char_data_t) + readLen, 0, &eb);

    if (pMsg != NULL)
    {
        pMsg->heap = NULL;
        pMsg->type = appMsgType;
        pMsg->pduLen = sizeof(device_msg_t) + sizeof(char_data_t) + readLen;

        char_data_t *pCharData = (char_data_t *)pMsg->pdu;
        pCharData->svcUUID = serviceUUID; // Use 16-bit part of UUID.
        pCharData->paramID = paramID;
        pCharData->retSvcUUID = retServiceUUID;
        pCharData->retParamID = retParamID;
        pCharData->retDeviceID = retDeviceID;
        pCharData->connHandle = connHandle;
        // Copy data from service now.
        memcpy(pCharData->data, pValue, readLen);
        // Update pCharData with how much data we received.
        pCharData->dataLen = readLen;
        // Enqueue the message using pointer to queue node element.
        Queue_enqueue(devHandle->msgQHandle, &pMsg->_elem);
        // Let application know there's a message.
        Event_post(devHandle->eventHandle, DEVICE_APP_MSG_EVT);
    }
}

/*
 * @brief  Generic message constructor for application messages.
 *
 *         Sends a message to the application for handling in Task context.
 *
 * @param  appMsgType    Enumerated type of message being sent.
 * @oaram  *pValue       Pointer to characteristic value
 * @param  len           Length of characteristic data
 */
void vDevice_enqueueRawAppMsg(DeviceList_Handler devHandle,
                                      device_msg_types_t appMsgType,
                                      uint8_t *pData,
                                      uint16_t len)
{
    Error_Block eb;

    ASSERT(devHandle != NULL);

    if (devHandle == NULL) {
        return;
    }

    Error_init(&eb);
    // Allocate memory for the message.
//    device_msg_t *pMsg = ICall_malloc( sizeof(device_msg_t) + len );
    device_msg_t *pMsg = Memory_alloc(NULL, sizeof(device_msg_t) + len, 0, &eb);

    if (pMsg != NULL)
    {
        pMsg->heap = NULL;
        pMsg->type = appMsgType;
        pMsg->pduLen = sizeof(device_msg_t) + len;

        // Copy data into message
        memcpy(pMsg->pdu, pData, len);

        // Enqueue the message using pointer to queue node element.
        Queue_enqueue(devHandle->msgQHandle, &pMsg->_elem);
        //    // Let application know there's a message.
        Event_post(devHandle->eventHandle, DEVICE_APP_MSG_EVT);
    }
}


/*
 * @brief  Generic message constructor for application messages.
 *
 *         Sends a message to the application for handling in Task context.
 *
 * @param  appMsgType    Enumerated type of message being sent.
 * @oaram  *pValue       Pointer to characteristic value
 * @param  len           Length of characteristic data
 */
void vDevice_enqueueRawAppMsgAndEvent(DeviceList_Handler devHandle,
                                      device_msg_types_t appMsgType,
                                      uint8_t *pData,
                                      uint16_t len,
                                      UInt eventMask)
{
    Error_Block eb;

    ASSERT(devHandle != NULL);

    if (devHandle == NULL) {
        return;
    }

    Error_init(&eb);
    // Allocate memory for the message.
//    device_msg_t *pMsg = ICall_malloc( sizeof(device_msg_t) + len );
    device_msg_t *pMsg = Memory_alloc(NULL, sizeof(device_msg_t) + len, 0, &eb);

    if (pMsg != NULL)
    {
        pMsg->heap = NULL;
        pMsg->type = appMsgType;
        pMsg->pduLen = sizeof(device_msg_t) + len;

        // Copy data into message
        memcpy(pMsg->pdu, pData, len);

        // Enqueue the message using pointer to queue node element.
        Queue_enqueue(devHandle->msgQHandle, &pMsg->_elem);
        //    // Let application know there's a message.
        Event_post(devHandle->eventHandle, eventMask);
    }
}

void vDevice_sendCharDataMsg( uint32_t deviceID,
                              device_msg_types_t appMsgType,
                              int32_t connHandle,
                              uint16_t serviceUUID, uint8_t paramID,
                              uint32_t retDeviceID,
                              uint16_t retServiceUUID, uint8_t retParamID,
                              uint8_t *pValue, uint16_t len )
{
    DevicesList_t *pDeviceList;
    Queue_Elem *elem;

    for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
        pDeviceList = (DevicesList_t *)elem;
        if (pDeviceList->deviceID == deviceID) {
            vDevice_enqueueCharDataMsg( pDeviceList,
                                        appMsgType,
                                        connHandle,
                                        serviceUUID, paramID,
                                        retDeviceID,
                                        retServiceUUID, retParamID,
                                        pValue, len );
        }
    }

}


uint32_t xDevice_sendCharDataMsg( uint32_t deviceID,
                              device_msg_types_t appMsgType,
                              int32_t connHandle,
                              uint16_t serviceUUID, uint8_t paramID,
                              uint32_t retDeviceID,
                              uint16_t retServiceUUID, uint8_t retParamID,
                              uint8_t *pValue, uint16_t len )
{
    uint32_t retVal;
    DevicesList_t *pDeviceList;
    Queue_Elem *elem;

    retVal = 0;
    for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
        pDeviceList = (DevicesList_t *)elem;
        if (pDeviceList->deviceID == deviceID) {
            retVal++;
            vDevice_enqueueCharDataMsg( pDeviceList,
                                        appMsgType,
                                        connHandle,
                                        serviceUUID, paramID,
                                        retDeviceID,
                                        retServiceUUID, retParamID,
                                        pValue, len );
        }
    }
    return retVal;
}


uint32_t xDevice_sendErrorMsg( uint32_t deviceID,
                               int32_t connHandle,
                               uint16_t serviceUUID, uint8_t paramID,
                               uint32_t retDeviceID,
                               uint16_t retServiceUUID, uint8_t retParamID,
                               device_msg_error_t error )
{
    uint32_t errorDescription = error;
    return xDevice_sendCharDataMsg (deviceID,
                                    APP_MSG_ERROR_HANDLER,
                                    connHandle,
                                    serviceUUID, paramID,
                                    retDeviceID,
                                    retServiceUUID, retParamID,
                                    (uint8_t *)&errorDescription, sizeof(uint32_t));
}


