/*
 * TFUARTTestDevice.c
 *
 *  Created on: Aug 19, 2019
 *      Author: epenate
 */




#define __DEVICES_TFTEST_TFUARTTESTDEVICE_GLOBAL
#include "includes.h"


void vTFUartTestDevice_periodicEventHook(UArg arg0, UArg arg1);
void vTFUartTestDevice_appMsgEventHook(UArg arg0, UArg arg1);
void vTFUartTestDevice_setupHook(UArg arg0, UArg arg1);



void vTFUartTestDevice_Params_init(Device_Params *params, uint32_t address, uint32_t ifIndex)
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

//        vGeneticDevice_registerHookFunctionForSetUp(pHooks, vTFUartTestDevice_setupHook, arg0);

        /*
         * Needs to change vGenericTemplateDevice_periodicEventHook
         */
        arg0 = (UArg)ifIndex;
        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_PERIODIC_EVT, vTFUartTestDevice_periodicEventHook, arg0);
        /*
         * Needs to change vGenericTemplateDevice_appMsgEventHook
         */

        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_APP_MSG_EVT, vTFUartTestDevice_appMsgEventHook, arg0);

        vGeneticDevice_Params_init(params, address, pHooks);
        params->arg1 = (void *)ifIndex;
        params->deviceType = DEVICE_TYPE_TF_UART_TEST;
        params->period = DEVICES_TFTEST_TFUARTTESTDEVICE_CLOCK_PERIOD;
        params->timeout = DEVICES_TFTEST_TFUARTTESTDEVICE_CLOCK_TIMEOUT;
        params->priority = DEVICES_TFTEST_TFUARTTESTDEVICE_TASK_PRIORITY;
        params->stackSize = DEVICES_TFTEST_TFUARTTESTDEVICE_TASK_STACK_SIZE;
    }
    (void)ui32EventId;
}


void vTFUartTestDevice_setupHook(UArg arg0, UArg arg1)
{
    GenericDevice_hookParams *pHooks;
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

    /*
     * arg0 is a pointer to GenericDevice_hookParams struct so it can be used to share data with the other hooks functions
     */
    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
    pHooks = (GenericDevice_hookParams *)pArgs->hooks;


    pHooks->setupArg0 = pHooks->setupArg0;
}

void vTFUartTestDevice_periodicEventHook(UArg arg0, UArg arg1)
{
//    DeviceList_Handler devHandle;
//    devHandle = (DeviceList_Handler)arg1;
    ALTO_Frame_t tFrameTx, tFrameRx;
    ALTO_Frame_t *ptFrameRx = &tFrameRx;


//    int i32Index;
//    tALTOORTRouting *ptORT;
//    tALTOORTInput tORTInput;
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
    ifTransaction.readTimeout = 70;
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
//                    vALTOFrame_ORTConvertDataFromBigEndianToLittleEndian(ptFrameRx->data, ptFrameRx->length);
//                    ptORT = (tALTOORTRouting *)ptFrameRx->data;
//                    for (i32Index = 0; i32Index < ptFrameRx->length / 2; i32Index++) {
//                        tORTInput.value = ptORT->inputState;
//                        tORTInput.inputAddress = ptORT->destinationVirtualInput;
//                        tORTInput.reserved = 0;
//
//                        switch(ptORT->destinationType) {
//                        case ALTO_Multinet_RelayBox:
//                        case ALTO_Multinet_TemperatureBox:
////                            multinetAddress = ((ptORT->destinationType & 0x07) << 5) | (ptORT->destinationAddress & 0x1F);
////                            ALTOMultinetRemoteRoutingMsgObj *pMsgObj = ptALTOMultinet_SendMsgToControllerWorker(hControllersListQueue,
////                                                                                                                multinetAddress,
////                                                                                                                &tORTInput);
////                            if (pMsgObj == NULL) {};
//
//                            break;
//                        default:
//                            break;
//                        }
//                        ptORT++;
//                    }
                    break;
//                case ALTO_Operation_ACKNAK:
//
//                    if (ptFrameRx->length) {
//                        switch(ptFrameRx->data[0]) {
//                        case ALTO_AckNak_GoodStatus:
//                            break;
//                        case ALTO_AckNak_Busy_WaitingToRespond:
//                            break;
//                        case ALTO_AckNak_DiagSessionNotActive:
//                            break;
//                        case ALTO_AckNak_HardwareFault:
//                            break;
//                        case ALTO_AckNak_InvalidCRC:
//                            break;
//                        case ALTO_AckNak_FunctionNotExecuted:
//                            break;
//                        case ALTO_AckNak_InvalidArgument:
//                            break;
//                        case ALTO_AckNak_InvalidBcc:
//                            break;
//                        default:
//                            break;
//                        }
//                    }
//                    break;
                default:
                    g_TFUartTestErrorCounter[ui32SerialPort]++;
//                    DOSet(DIO_LED_D20, DO_ON);
                    break;
                }
            }

        }
    }else {
        g_TFUartTestErrorCounter[ui32SerialPort]++;
//        DOSet(DIO_LED_D20, DO_ON);
    }
}



void vTFUartTestDevice_appMsgEventHook(UArg arg0, UArg arg1)
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



