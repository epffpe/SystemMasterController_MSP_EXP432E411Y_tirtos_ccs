/*
 * TestDevice.c
 *
 *  Created on: Mar 26, 2018
 *      Author: epenate
 */


#define __DEVICES_TESTDEVICE_TESTDEVICE_GLOBAL
#include "includes.h"


void vTestDevice_close(DeviceList_Handler handle);
DeviceList_Handler hTestDevice_open(DeviceList_Handler handle, void *params);



const Device_FxnTable g_TestDevice_fxnTable =
{
 .closeFxn = vTestDevice_close,
 .openFxn = hTestDevice_open,
 .sendMsgFxn = NULL,
};

void vTestDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_TEST;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_TestDevice_fxnTable;
}

//Void testFxn(UArg arg0, UArg arg1)
//{
//    IF_Handle handle;
//    IF_Params params;
//    IF_Transaction ifTransaction;
//    bool transferOk;
//    char buf[] = "Hello World\n\r";
//    char readBuf[64];
//
//    GPIO_write(SMC_UART_DEBUG, 0);
//
//    vIF_Params_init(&params, IF_Params_Type_Uart);
//    params.uartParams.writeDataMode = UART_DATA_BINARY;
//    params.uartParams.readDataMode = UART_DATA_BINARY;
//    params.uartParams.readReturnMode = UART_RETURN_FULL;
//    params.uartParams.readEcho = UART_ECHO_OFF;
//    params.uartParams.baudRate = (unsigned int)115200;
//    handle = hIF_open(IF_SERIAL_5, &params);
//
//    memset(&ifTransaction, 0, sizeof(IF_Transaction));
//    ifTransaction.readCount = 5;
//    ifTransaction.readBuf = readBuf;
//    ifTransaction.readTimeout = BIOS_WAIT_FOREVER;
////    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_NONE;
//    ifTransaction.writeBuf = buf;
//    ifTransaction.writeCount = sizeof(buf);
//    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
//    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;
//    while (1) {
//        transferOk = bIF_transfer(handle, &ifTransaction);
//        if (!transferOk) {
//            ifTransaction.writeCount = sizeof(buf);
//        }
//        Task_sleep(1000);
//    }
//}


Void vTestDeviceFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    DeviceList_Handler devHandle;
    Queue_Handle msgQHandle;
    Event_Handle eventHandle;
    Clock_Handle clockHandle;
    IF_Handle ifHandle;
    IF_Params params;
    IF_Transaction ifTransaction;
    device_msg_t *pMsg;
    bool transferOk;
//    char buf[] = "Hello World\n\r";
//    char readBuf[64];
    ALTO_Frame_t tFrameTx;
    char rxbuff[70];
    char txbuff[70];

//    GPIO_write(SMC_UART_DEBUG, 0);

//    vIF_init();

    ASSERT(arg1 != NULL);

    if (arg1 == NULL) {
        return;
    }
    devHandle = (DeviceList_Handler)arg1;
    eventHandle = devHandle->eventHandle;
    msgQHandle = devHandle->msgQHandle;
    clockHandle = devHandle->clockHandle;

    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
    ifHandle = hIF_open(IF_SERIAL_6, &params);


    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.functionID = ALTO_Function_ManufacturingInformation;
    tFrameTx.length = 0x00;
    tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
    tFrameTx.ui4address = 1 & 0x1F;
    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = sizeof(rxbuff);
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 50;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = sizeof(txbuff);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;
    while (1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;
            ifTransaction.readCount = 64;
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (!transferOk) {
                ifTransaction.writeCount = sizeof(txbuff);
            }
        }

        if (events & DEVICE_APP_KILL_EVT) {
            events &= ~DEVICE_APP_KILL_EVT;

            Clock_stop(clockHandle);
            Clock_delete(&clockHandle);

            /*
             * Needs to flush the messages in the queue before remove it
             */
            while (!Queue_empty(msgQHandle)) {
                pMsg = Queue_get(msgQHandle);
                Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
            }
            Queue_delete(&msgQHandle);
            Event_delete(&eventHandle);
            Task_exit();
        }
    }
}

void vTestDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}


void vTestDevice_close(DeviceList_Handler handle)
{
    unsigned int key;

//    static const int price_lookup[] = {
//                                       [APP_MSG_SERVICE_WRITE] = 6,
//                                       [APP_MSG_SERVICE_CFG] = 10,
//                                       [2 ... 10] = 55
//    };

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }

    /* Need exclusive access to prevent a race condition */
    key = Hwi_disable();
    if(handle->state.opened == true) {
        handle->state.opened = false;
        Hwi_restore(key);

        Event_post(handle->eventHandle, DEVICE_APP_KILL_EVT);
        return;
    }
    Hwi_restore(key);
}
DeviceList_Handler hTestDevice_open(DeviceList_Handler handle, void *params)
{
    unsigned int key;
    Error_Block eb;
    Device_Params *deviceParams;

    union {
        Task_Params        taskParams;
        Event_Params       eventParams;
        Clock_Params       clockParams;
    } paramsUnion;

    ASSERT(handle != NULL);
    ASSERT(params != NULL);


    if (handle == NULL) {
        return (NULL);
    }

    /* Need exclusive access to prevent a race condition */
    key = Hwi_disable();
    if(handle->state.opened == true) {
        Hwi_restore(key);
        Log_warning1("Device:(%p) already in use.", handle->deviceID);
        return handle;
    }
    handle->state.opened = true;
    Hwi_restore(key);

    Error_init(&eb);

    deviceParams = (Device_Params *)params;
    handle->deviceID = deviceParams->deviceID;

//    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


    Clock_Params_init(&paramsUnion.clockParams);
    paramsUnion.clockParams.period = TEST_DEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vTestDevice_clockHandler, TEST_DEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = 2048;
    paramsUnion.taskParams.priority = 2;
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vTestDeviceFxn, &paramsUnion.taskParams, &eb);

    return handle;
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
//static void user_enqueueCharDataMsg( app_msg_types_t appMsgType,
//                                     uint16_t connHandle,
//                                     uint16_t serviceUUID, uint8_t paramID,
//                                     uint8_t *pValue, uint16_t len )
//{
//  // Called in Stack's Task context, so can't do processing here.
//  // Send message to application message queue about received data.
//  uint16_t readLen = len; // How much data was written to the attribute
//
//  // Allocate memory for the message.
//  // Note: The pCharData message doesn't have to contain the data itself, as
//  //       that's stored in a variable in the service implementation.
//  //
//  //       However, to prevent data loss if a new value is received before the
//  //       service's container is read out via the GetParameter API is called,
//  //       we copy the characteristic's data now.
//  app_msg_t *pMsg = ICall_malloc( sizeof(app_msg_t) + sizeof(char_data_t) +
//                                  readLen );
//
//  if (pMsg != NULL)
//  {
//    pMsg->type = appMsgType;
//
//    char_data_t *pCharData = (char_data_t *)pMsg->pdu;
//    pCharData->svcUUID = serviceUUID; // Use 16-bit part of UUID.
//    pCharData->paramID = paramID;
//    // Copy data from service now.
//    memcpy(pCharData->data, pValue, readLen);
//    // Update pCharData with how much data we received.
//    pCharData->dataLen = readLen;
//    // Enqueue the message using pointer to queue node element.
//    Queue_enqueue(hApplicationMsgQ, &pMsg->_elem);
//  // Let application know there's a message.
//  Event_post(syncEvent, PRZ_APP_MSG_EVT);
//  }
//}

/*
 * @brief  Generic message constructor for application messages.
 *
 *         Sends a message to the application for handling in Task context.
 *
 * @param  appMsgType    Enumerated type of message being sent.
 * @oaram  *pValue       Pointer to characteristic value
 * @param  len           Length of characteristic data
 */
//static void user_enqueueRawAppMsg(app_msg_types_t appMsgType, uint8_t *pData,
//                                  uint16_t len)
//{
//  // Allocate memory for the message.
//  app_msg_t *pMsg = ICall_malloc( sizeof(app_msg_t) + len );
//
//  if (pMsg != NULL)
//  {
//    pMsg->type = appMsgType;
//
//    // Copy data into message
//    memcpy(pMsg->pdu, pData, len);
//
//    // Enqueue the message using pointer to queue node element.
//    Queue_enqueue(hApplicationMsgQ, &pMsg->_elem);
////    // Let application know there's a message.
//    Event_post(syncEvent, PRZ_APP_MSG_EVT);
//  }
//}

// Process messages sent from another task or another context.
// Initialize application
//SimpleBLEPeripheral_init();
//
//// Application main loop
//for (;;)
//{
//  uint32_t events;
//
//  // Waits for an event to be posted associated with the calling thread.
//  // Note that an event associated with a thread is posted when a
//  // message is queued to the message receive queue of the thread
//  events = Event_pend(syncEvent, Event_Id_NONE, SBP_ALL_EVENTS,
//                      ICALL_TIMEOUT_FOREVER);
//
//  if (events)
//  {
//    ICall_EntityID dest;
//    ICall_ServiceEnum src;
//    ICall_HciExtEvt *pMsg = NULL;
//
//    // Fetch any available messages that might have been sent from the stack
//    if (ICall_fetchServiceMsg(&src, &dest,
//                              (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
//    {
//      uint8 safeToDealloc = TRUE;
//
//      if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
//      {
//        ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;
//
//        // Check for BLE stack events first
//        if (pEvt->signature == 0xffff)
//        {
//          // The GATT server might have returned a blePending as it was trying
//          // to process an ATT Response. Now that we finished with this
//          // connection event, let's try sending any remaining ATT Responses
//          // on the next connection event.
//          if (pEvt->event_flag & SBP_HCI_CONN_EVT_END_EVT)
//          {
//            // Try to retransmit pending ATT Response (if any)
//            SimpleBLEPeripheral_sendAttRsp();
//          }
//        }
//        else
//        {
//          // Process inter-task message
//          safeToDealloc = SimpleBLEPeripheral_processStackMsg((ICall_Hdr *)pMsg);
//        }
//      }
//
//      if (pMsg && safeToDealloc)
//      {
//        ICall_freeMsg(pMsg);
//      }
//    }
//
//    // If RTOS queue is not empty, process app message.
//    if (events & SBP_QUEUE_EVT)
//    {
//      while (!Queue_empty(appMsgQueue))
//      {
//        sbpEvt_t *pMsg = (sbpEvt_t *)Util_dequeueMsg(appMsgQueue);
//        if (pMsg)
//        {
//          // Process message.
//          SimpleBLEPeripheral_processAppMsg(pMsg);
//
//          // Free the space from the message.
//          ICall_free(pMsg);
//        }
//      }
//    }
//
//    if (events & SBP_PERIODIC_EVT)
//    {
//      Util_startClock(&periodicClock);
//
//      // Perform periodic application task
//      SimpleBLEPeripheral_performPeriodicTask();
//    }
//  }
//}

