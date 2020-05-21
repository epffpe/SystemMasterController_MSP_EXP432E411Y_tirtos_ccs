/*
 * ForteManagerDevice.c
 *
 *  Created on: Sep 17, 2018
 *      Author: epenate
 */


#define __DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_GLOBAL
#include "includes.h"

#define FORTEMANAGER_APP_RXMSG_EVT                       Event_Id_04
#define FORTEMANAGER_APP_TXMSG_EVT                       Event_Id_05


#define FORTEMANAGERDEVICE_AMP_ROUTING_MAP_SIZE       8

//const uint32_t g_aForteManagerDeviceAmpRoutingMap[FORTEMANAGERDEVICE_AMP_ROUTING_MAP_SIZE] =
//{
// IF_SERIAL_0,
// IF_SERIAL_1,
// IF_SERIAL_2,
// IF_SERIAL_3,
// IF_SERIAL_4,
// IF_SERIAL_5,
// IF_SERIAL_6,
// IF_SERIAL_7,
//};



Void vForteManagerDeviceRX_taskFxn(UArg arg0, UArg arg1);
static void vForteManagerDevice_processAppRXMsgEvent(device_msg_t *pMsg, DeviceList_Handler devHandle, uint32_t ifIndex);

void vForteManagerDevice_setupHook(UArg arg0, UArg arg1)
{

    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;

//    GenericDevice_hookParams *pHooks;
//    DeviceList_Handler devHandle;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }
//    devHandle = (DeviceList_Handler)arg1;

//    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
//    pHooks = (GenericDevice_hookParams *)pArgs->hooks;


    /* Make sure Error_Block is initialized */
    Error_init(&eb);


    Task_Params_init(&taskParams);
    taskParams.stackSize = DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_TASK_STACK_SIZE;
    taskParams.priority = DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_TASK_PRIORITY + 1;
    taskParams.arg0 = arg0;
    taskParams.arg1 = arg1;
    taskHandle = Task_create((Task_FuncPtr)vForteManagerDeviceRX_taskFxn, &taskParams, &eb);
    if (taskHandle == NULL) {
        System_printf("Failed to create vForteManagerDeviceRX_task Task\n");
        System_flush();
    }
}

void vForteManagerDevice_periodicEventHook(UArg arg0, UArg arg1)
{
//    DeviceList_Handler devHandle;
//
//    devHandle = (DeviceList_Handler)arg1;
}



void vForteManagerDevice_appMsgEventHook(UArg arg0, UArg arg1)
{
    DeviceList_Handler devHandle;
    Queue_Handle msgQHandle;
    device_msg_t *pMsg;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }

    devHandle = (DeviceList_Handler)arg1;

    msgQHandle = devHandle->msgQHandle;

    while (!Queue_empty(msgQHandle))
    {
        pMsg = Queue_dequeue(msgQHandle);

        // Process application-layer message probably sent from ourselves.
//        vALTOAmp_processApplicationMessage(pMsg, myDeviceID, ifHandle, txbuff, rxbuff);


        // Free the received message.
        Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
//            Task_sleep(3000);
    }
}

void vForteManagerDevice_appRXMsgEventHook(UArg arg0, UArg arg1)
{
    DeviceList_Handler devHandle;
    Queue_Handle msgQHandle;
    device_msg_t *pMsg;
    uint32_t ifIndex;

    /*
     * Use arg0 to pass information about the Serial Port used by the AMP and the ALTO Multinet
     */
    ASSERT( (IF_SERIAL_0 <= (uint32_t)arg0) && ((uint32_t)arg0 < IF_COUNT));
    ASSERT(arg1 != NULL);

    if ( !((IF_SERIAL_0 <= (int32_t)arg0) && ((uint32_t)arg0 < IF_COUNT)) ) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }

    ifIndex = (uint32_t)arg0;
    devHandle = (DeviceList_Handler)arg1;

    msgQHandle = devHandle->msgQHandle;

    while (!Queue_empty(msgQHandle))
    {
        pMsg = Queue_dequeue(msgQHandle);

        // Process application-layer message probably sent from ourselves.
//        vALTOAmp_processApplicationMessage(pMsg, myDeviceID, ifHandle, txbuff, rxbuff);
        if (pMsg->type != APP_MSG_RAW) {
            Queue_enqueue(msgQHandle, &pMsg->_elem);
            break;
        }
        vForteManagerDevice_processAppRXMsgEvent(pMsg, devHandle, ifIndex);
        // Free the received message.
        Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
//            Task_sleep(3000);
    }
}


void vForteManagerDevice_Params_init(Device_Params *params, uint32_t address, uint32_t ifIndex)
{
    uint32_t ui32EventId;
    Error_Block eb;
    UArg arg0 = NULL;

    ASSERT(params != NULL);

    if (params == NULL) {
        return;
    }

    Error_init(&eb);

    GenericDevice_hookParams *pHooks = Memory_alloc(NULL, sizeof(GenericDevice_hookParams), 0, &eb);
    if (pHooks != NULL)
    {
        vGeneticDevice_Hooks_init(pHooks);

        vGeneticDevice_registerHookFunctionForSetUp(pHooks, vForteManagerDevice_setupHook, arg0);

        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_PERIODIC_EVT, vForteManagerDevice_periodicEventHook, arg0);
        pHooks->orMask |= ui32EventId;
        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_APP_MSG_EVT, vForteManagerDevice_appMsgEventHook, arg0);
        pHooks->orMask |= ui32EventId;
        arg0 = (UArg)ifIndex;
        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, FORTEMANAGER_APP_RXMSG_EVT, vForteManagerDevice_appRXMsgEventHook, arg0);
        pHooks->orMask |= ui32EventId;

        vGeneticDevice_Params_init(params, address, pHooks);
        params->arg1 = (void *)ifIndex;
        params->priority = DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_TASK_PRIORITY;
    }
}

Void vForteManagerDeviceRX_taskFxn(UArg arg0, UArg arg1)
{
//    GenericDevice_hookParams *pHooks;
    DeviceList_Handler devHandle;
//    uint32_t myDeviceID;
    uint32_t ifIndex;
    uint32_t ui32retValue;

    IF_Handle ifHandle;
    IF_Params params;
    IF_Transaction ifTransaction;
//    bool transferOk;
    char rxbuff[70];

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }
    devHandle = (DeviceList_Handler)arg1;
//    myDeviceID = devHandle->deviceID;

    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
    ifIndex = pArgs->arg1; //IF_SERIAL_4
//    pHooks = (GenericDevice_hookParams *)pArgs->hooks;


    /*
     * Initialize interface
     */
    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
    ifHandle = hIF_open((uint32_t)ifIndex, &params);

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = sizeof(rxbuff);
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_NET;
    ifTransaction.writeBuf = NULL;
    ifTransaction.writeCount = 0;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_ALTO_FORTE_MANAGER; //IF_TRANSFER_TYPE_NONE;

    while(1) {
        ifTransaction.readCount = IF_ALTO_SERIAL_FRAME_SIZE;
        ifTransaction.writeCount = 0;
//        transferOk = bIF_transfer(ifHandle, &ifTransaction);
//        if (transferOk) {
//            vDevice_enqueueRawAppMsgAndEvent(devHandle,
//                                             APP_MSG_RAW,
//                                             (uint8_t *)rxbuff,
//                                             IF_ALTO_SERIAL_FRAME_SIZE,
//                                             FORTEMANAGER_APP_RXMSG_EVT);
//        }
        ui32retValue = 0;
        ui32retValue = xIFUART_receiveALTOFrame(ifHandle,
                                                (char *)rxbuff,
                                                BIOS_WAIT_FOREVER);
        if (ui32retValue == IF_ALTO_SERIAL_FRAME_SIZE) {
            vDevice_enqueueRawAppMsgAndEvent(devHandle,
                                             APP_MSG_RAW,
                                             (uint8_t *)rxbuff,
                                             IF_ALTO_SERIAL_FRAME_SIZE,
                                             FORTEMANAGER_APP_RXMSG_EVT);
        }
    }
}


/*
 * ***************************************************************************
 */

static void vForteManagerDevice_processAppRXMsgEvent(device_msg_t *pMsg, DeviceList_Handler devHandle, uint32_t ifIndex)
{
    ALTO_Frame_t tALTOFrameTx;
    ALTO_Frame_t tALTOFrameRx;
    uint32_t ui32retValue;
    int8_t i8bcc;
//    int n;

//    uint32_t ifIndex;

    IF_Handle ifHandle;
    IF_Handle ifHandleFM;
    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
    char rxbuff[70];
    char txbuff[70];

//    uint32_t ui32PCSerialPort;
    uint32_t ui32DeviceSerialPort;
    uint32_t ui32AmpSerialPort;

    ASSERT(pMsg != NULL);
    ASSERT(devHandle != NULL);



//    ifIndex = IF_MULTINET;
//    pHooks = (GenericDevice_hookParams *)pArgs->hooks;


    /*
     * Initialize interface
     */
    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
//    ifHandle = hIF_open((uint32_t)ifIndex, &params);
    ifHandleFM = hIF_open((uint32_t)ifIndex, &params);

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = IF_ALTO_SERIAL_FRAME_SIZE;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 50;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_NET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = 70; //IF_ALTO_SERIAL_FRAME_SIZE;
    ifTransaction.writeTimeout = 50;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;

    xALTOFrame_convert_ASCII_to_binary((char *)pMsg->pdu, &tALTOFrameTx);
    i8bcc = cALTOFrame_BCC_check(&tALTOFrameTx);

    if (!i8bcc) {
        switch (tALTOFrameTx.uin4unitType) {
//        case 0:
        case ALTO_ALTONet_Amp:
//            ui32AmpSerialPort = g_aForteManagerDeviceAmpRoutingMap[tALTOFrameTx.ui4address];
            ui32AmpSerialPort = tALTOFrameTx.ui4address;
//            ui32AmpSerialPort = IF_SERIAL_3;
            if ((ui32AmpSerialPort < IF_COUNT) && (ui32AmpSerialPort != ifIndex)) {
                ifHandle = hIF_open((uint32_t)ui32AmpSerialPort, &params);
                vALTOFrame_create_ASCII(txbuff, &tALTOFrameTx);
                if (!((tALTOFrameTx.classID == ALTO_Class_Diagnostic) && (tALTOFrameTx.functionID == ALTO_Function_TuningDatabaseInfo || tALTOFrameTx.functionID == ALTO_Function_TuningDBRecordInfo))) {
                    transferOk = bIF_transfer(ifHandle, &ifTransaction);
                    if (transferOk) {
                        xALTOFrame_convert_ASCII_to_binary((char *)rxbuff, &tALTOFrameRx);
                        vALTOFrame_create_ASCII(txbuff, &tALTOFrameRx);
                        ui32retValue = xIFUART_sendData(ifHandleFM,
                                         (const char *)txbuff,
                                         sizeof(txbuff),
                                         BIOS_WAIT_FOREVER);
                        ui32retValue = ui32retValue;
                    }
                }else{
                    char rxbuff2[70];
                    char rxbuff3[3*70];
                    uint32_t ui32retValue2;
                    uint32_t ui32retValue3;
                    ui32retValue2 = 0;

                    ifTransaction.readCount = 0;
//                    ifTransaction.readBuf = rxbuff;
                    transferOk = bIF_transfer(ifHandle, &ifTransaction);
//                    ui32retValue = xIFUART_receiveALTOFrameFSMData(ifHandle,
//                                                                   (char *)rxbuff,
//                                                                   70,
//                                                                   50);
//                    ui32retValue2 = xIFUART_receiveALTOFrameFSMData(ifHandle,
//                                                                    (char *)rxbuff2,
//                                                                    70,
//                                                                    50);
//                    ui32retValue3 = xIFUART_receiveALTOFrameFSMData(ifHandle,
//                                                                    (char *)rxbuff3,
//                                                                    70,
//                                                                    50);
////                    ui32retValue2 = xIFUART_receiveALTOFrame(ifHandle,
////                                                             (char *)rxbuff2,
////                                                             50);
////                    ui32retValue3 = xIFUART_receiveALTOFrame(ifHandle,
////                                                             (char *)rxbuff3,
////                                                             50);
//
//                    if (transferOk) {
//                        if (ui32retValue2 == 70) {
//                            //                        xALTOFrame_convert_ASCII_to_binary((char *)rxbuff, &tALTOFrameRx);
//                            //                        vALTOFrame_create_ASCII(txbuff, &tALTOFrameRx);
//                            //                        ui32retValue = xIFUART_sendData(ifHandleFM,
//                            //                                                        (const char *)txbuff,
//                            //                                                        sizeof(txbuff),
//                            //                                                        BIOS_WAIT_FOREVER);
//                            //                        Task_sleep((unsigned int)6);
//                            //                        ui32retValue = ui32retValue;
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)"AA55",
//                                                            4,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)rxbuff,
//                                                            64,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)"\r\n",
//                                                            2,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                        }
//
//                        //                    if (ui32retValue2 == IF_ALTO_SERIAL_FRAME_SIZE) {
//                        if (ui32retValue2 == 70) {
//                            //                        xALTOFrame_convert_ASCII_to_binary((char *)rxbuff2, &tALTOFrameRx);
//                            //                        vALTOFrame_create_ASCII(txbuff, &tALTOFrameRx);
//                            //                        ui32retValue = xIFUART_sendData(ifHandleFM,
//                            //                                                        (const char *)txbuff,
//                            //                                                        sizeof(txbuff),
//                            //                                                        BIOS_WAIT_FOREVER);
//                            //                        Task_sleep((unsigned int)6);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)"AA55",
//                                                            4,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)rxbuff2,
//                                                            64,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)"\r\n",
//                                                            2,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                        }
//
//                        //                    if (ui32retValue3 == IF_ALTO_SERIAL_FRAME_SIZE) {
//                        if (ui32retValue3 == 70) {
//                            //                        xALTOFrame_convert_ASCII_to_binary((char *)rxbuff3, &tALTOFrameRx);
//                            //                        vALTOFrame_create_ASCII(txbuff, &tALTOFrameRx);
//                            //                        ui32retValue = xIFUART_sendData(ifHandleFM,
//                            //                                                        (const char *)txbuff,
//                            //                                                        sizeof(txbuff),
//                            //                                                        BIOS_WAIT_FOREVER);
//                            //                        Task_sleep((unsigned int)6);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)"AA55",
//                                                            4,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)rxbuff3,
//                                                            64,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                            ui32retValue = xIFUART_sendData(ifHandleFM,
//                                                            (const char *)"\r\n",
//                                                            2,
//                                                            BIOS_WAIT_FOREVER);
//                            Task_sleep((unsigned int)4);
//                        }
//                    }

                    ui32retValue3 = xIFUART_receiveDataSimple(ifHandle,
                                                              (char *)rxbuff3,
                                                              3*70,
                                                              200);
//                    if (ui32retValue3 == 3*70) {
                        ui32retValue = xIFUART_sendData(ifHandleFM,
                                                        (const char *)rxbuff3,
                                                        ui32retValue3,
                                                        BIOS_WAIT_FOREVER);
//                    }
                }
            }
            break;
        case ALTO_Multinet_RelayBox:
        case ALTO_Multinet_TemperatureBox:
            ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
            ui32DeviceSerialPort = IF_MULTINET;
            if ((ui32DeviceSerialPort < IF_COUNT) && (ui32DeviceSerialPort != ifIndex)) {
                ifHandle = hIF_open((uint32_t)ui32DeviceSerialPort, &params);
                vALTOFrame_create_ASCII(txbuff, &tALTOFrameTx);
                transferOk = bIF_transfer(ifHandle, &ifTransaction);
                if (transferOk) {
                    xALTOFrame_convert_ASCII_to_binary((char *)rxbuff, &tALTOFrameRx);
                    vALTOFrame_create_ASCII(txbuff, &tALTOFrameRx);
                    ui32retValue = xIFUART_sendData(ifHandleFM,
                                     (const char *)txbuff,
                                     sizeof(txbuff),
                                     BIOS_WAIT_FOREVER);
                }
            }
            break;
        default:
            break;
        }
    }else{

    }
}

