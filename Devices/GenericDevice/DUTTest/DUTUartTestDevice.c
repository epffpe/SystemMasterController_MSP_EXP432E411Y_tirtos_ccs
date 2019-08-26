/*
 * DUTTestDevice.c
 *
 *  Created on: Aug 23, 2019
 *      Author: epenate
 */

#define __DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_GLOBAL
#include "includes.h"


void vDUTUartTestDevice_setupHook(UArg arg0, UArg arg1);
void vDUTUartTestDevice_periodicEventHook(UArg arg0, UArg arg1);
void vDUTUartTestDevice_appMsgEventHook(UArg arg0, UArg arg1);
Void vDUTUartTestDevice_taskFxn(UArg arg0, UArg arg1);


void vDUTUartTestDevice_Params_init(Device_Params *params, uint32_t address, uint32_t ifIndex)
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

        arg0 = (UArg)ifIndex;
        vGeneticDevice_registerHookFunctionForSetUp(pHooks, vDUTUartTestDevice_setupHook, arg0);

        /*
         * Needs to change vGenericTemplateDevice_periodicEventHook
         */
//        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_PERIODIC_EVT, vDUTUartTestDevice_periodicEventHook, arg0);
        /*
         * Needs to change vGenericTemplateDevice_appMsgEventHook
         */

        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_APP_MSG_EVT, vDUTUartTestDevice_appMsgEventHook, arg0);

        vGeneticDevice_Params_init(params, address, pHooks);
        params->arg1 = (void *)ifIndex;
        params->deviceType = DEVICE_TYPE_DUT_UART_TEST;
        params->period = DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_CLOCK_PERIOD;
        params->timeout = DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_CLOCK_TIMEOUT;
        params->priority = DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_TASK_PRIORITY;
        params->stackSize = DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_TASK_STACK_SIZE;
    }
    (void)ui32EventId;
}


void vDUTUartTestDevice_setupHook(UArg arg0, UArg arg1)
{
    GenericDevice_hookParams *pHooks;
//    DeviceList_Handler devHandle;
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }
//    devHandle = (DeviceList_Handler)arg1;

    /*
     * arg0 is a pointer to GenericDevice_hookParams struct so it can be used to share data with the other hooks functions
     */
    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
    pHooks = (GenericDevice_hookParams *)pArgs->hooks;


    pHooks->setupArg0 = pHooks->setupArg0;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);


    Task_Params_init(&taskParams);
    taskParams.stackSize = DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_TASK_STACK_SIZE;
    taskParams.priority = DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_TASK_PRIORITY + 1;
    taskParams.arg0 = arg0;
    taskParams.arg1 = arg1;
    taskHandle = Task_create((Task_FuncPtr)vDUTUartTestDevice_taskFxn, &taskParams, &eb);
    if (taskHandle == NULL) {
//        System_printf("Failed to create vForteManagerDeviceRX_task Task\n");
//        System_flush();
    }
}



void vDUTUartTestDevice_periodicEventHook(UArg arg0, UArg arg1)
{
//    DeviceList_Handler devHandle;
//    devHandle = (DeviceList_Handler)arg1;
    ALTO_Frame_t tFrameTx, tFrameRx;
    ALTO_Frame_t *ptFrameRx = &tFrameRx;


    int8_t i8bcc;
    uint8_t multinetAddress = 1;
    uint32_t ui32SerialPort;


    IF_Handle ifHandle;
    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
    char pRxMsgBuff70[70], pTxMsgBuff70[70];

    ASSERT( (IF_SERIAL_0 <= (uint32_t)arg0) && ((uint32_t)arg0 < IF_COUNT));
    ASSERT(arg1 != NULL);

    if ( !((IF_SERIAL_0 <= (int32_t)arg0) && ((uint32_t)arg0 < IF_COUNT)) ) {
        return;
    }

//    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
//    ifIndex = pArgs->arg1;
    ui32SerialPort = (uint32_t)arg0;


    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.functionID = ALTO_Function_DeviceDetailedStatus;
    tFrameTx.ui8MultinetAddress = multinetAddress;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.length = 0x00;


    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(pTxMsgBuff70, &tFrameTx);
    /*
     * Initialize interface
     */
    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
    ifHandle = hIF_open(ui32SerialPort, &params);

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 64;
    ifTransaction.readBuf = pRxMsgBuff70;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = pTxMsgBuff70;
    ifTransaction.writeCount = 70;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    transferOk = bIF_transfer(ifHandle, &ifTransaction);
    if (transferOk) {
        xALTOFrame_convert_ASCII_to_binary(pRxMsgBuff70, ptFrameRx);
        i8bcc = cALTOFrame_BCC_check(ptFrameRx);
        if (!i8bcc) {
            if (ptFrameRx->classID == ALTO_Class_Diagnostic) {
                switch(ptFrameRx->operationID) {
                case ALTO_Operation_Response:
                    break;
                default:
                    g_TFUartTestErrorCounter[ui32SerialPort]++;
                    break;
                }
            }

        }
    }else {
        g_TFUartTestErrorCounter[ui32SerialPort]++;
    }
}



void vDUTUartTestDevice_appMsgEventHook(UArg arg0, UArg arg1)
{
    DeviceList_Handler devHandle;
    Queue_Handle msgQHandle;
    device_msg_t *pMsg;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
//        return;
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



Void vDUTUartTestDevice_taskFxn(UArg arg0, UArg arg1)
{
//    GenericDevice_hookParams *pHooks;
//    DeviceList_Handler devHandle;
//    uint32_t myDeviceID;
    uint32_t ifIndex;
    uint32_t ui32retValue;

    IF_Handle ifHandle;
    IF_Params params;
//    IF_Transaction ifTransaction;
//    bool transferOk;
    char rxbuff[70], txbuff[70];
    ALTO_Frame_t tFrameTx, tFrameRx;
    ALTO_Frame_t *ptFrameRx = &tFrameRx;
    int8_t i8bcc;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }
//    devHandle = (DeviceList_Handler)arg1;
//    myDeviceID = devHandle->deviceID;

    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
    ifIndex = pArgs->arg1; //IF_SERIAL_4
//    pHooks = (GenericDevice_hookParams *)pArgs->hooks;

    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.functionID = ALTO_Function_DeviceDetailedStatus;
    tFrameTx.ui8MultinetAddress = 1;
    tFrameTx.operationID = ALTO_Operation_Response;
    tFrameTx.length = 0x00;


    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

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
//    memset(&ifTransaction, 0, sizeof(IF_Transaction));
//    ifTransaction.readCount = sizeof(rxbuff);
//    ifTransaction.readBuf = rxbuff;
//    ifTransaction.readTimeout = BIOS_WAIT_FOREVER;
//    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_NET;
//    ifTransaction.writeBuf = NULL;
//    ifTransaction.writeCount = 0;
//    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
//    ifTransaction.transferType = IF_TRANSFER_TYPE_ALTO_FORTE_MANAGER; //IF_TRANSFER_TYPE_NONE;

    while(1) {
//        ifTransaction.readCount = IF_ALTO_SERIAL_FRAME_SIZE;
//        ifTransaction.writeCount = 0;
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
            xALTOFrame_convert_ASCII_to_binary(rxbuff, ptFrameRx);
            i8bcc = cALTOFrame_BCC_check(ptFrameRx);
            if (!i8bcc) {
                if (ptFrameRx->classID == ALTO_Class_Diagnostic) {
                    switch(ptFrameRx->operationID) {
                    case ALTO_Operation_Get:
                        ui32retValue = xIFUART_sendData(ifHandle,
                                                        (const char *)txbuff,
                                                        70,
                                                        30);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}



