/*
 * ALTOMultinetDeviceRelayControllerWorker.c
 *
 *  Created on: May 21, 2018
 *      Author: epenate
 */

#define __RELAYCONTROLLERDEVICEWORKER_GLOBAL
#include "includes.h"


void vALTOMultinet_relayControllerTransferRemote(unsigned int ui32SerialPort,
                                                                char *pTxMsgBuff70,
                                                                char *pRxMsgBuff70,
                                                                ALTO_Frame_t *ptFrameRx,
                                                                Queue_Handle hControllersListQueue);

/*
 * Relay controller Worker task
 *
 *
 * arg0: ALTOMultinetDeviceControllerWorker_Params *params
 * arg1: ALTOMultinetDeviceControllerWorkerList_t *pWorker
 */
Void vALTOMultinet_relayControllerWorkerFxnV00(UArg arg0, UArg arg1)
{
    char rxbuff[70];
    char txbuff[70];
    int i32Index;
    ALTO_Frame_t tFrameTx, tFrameRx;
    tALTOORTInput tORTInput;
    tALTOORTInput *ptORTInput;
    int8_t i8bcc;
    ALTOMultinetDeviceControllerWorker_Params *ptRelayControllerWorkerParams;
//    ALTOMultinetControllerWorkerQ_Params *ptControllerWorkerQParams;
    Queue_Handle hControllersListQueue;
    Queue_Handle hMsgQ;
    ALTOMultinetRemoteRoutingMsg msg;
    uint32_t ui32SerialPort;
    Bool isSent;
//    GateMutexPri_Handle gateMutex;

    ASSERT (arg0 != NULL);
    ASSERT (arg1 != NULL);


    i8bcc = i8bcc;


    /*
     * Initialize ALTO Frame parameters
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Clu;
    tFrameTx.functionID = ALTO_Function_CluLocalRemoteVin;

    /*
     * Get Relay Controller Worker Parameters
     */

    ALTOMultinetDeviceControllerWorkerList_t *workerList = (ALTOMultinetDeviceControllerWorkerList_t *)arg1;

    ptRelayControllerWorkerParams = &workerList->worker.controllerWorkerParams;
    ui32SerialPort = ptRelayControllerWorkerParams->ui32InterfaceIndex;
    hControllersListQueue = ptRelayControllerWorkerParams->hControllersListQueue;

    hMsgQ = workerList->worker.hMsgQ;

//    ptControllerWorkerQParams = (ALTOMultinetControllerWorkerQ_Params *)arg1;
//    hFreeQueue = ptControllerWorkerQParams->hFreeQueue;
//    gateMutex = ptControllerWorkerQParams->gateMutex;


    while(1) {
        tFrameTx.ui8MultinetAddress = ptRelayControllerWorkerParams->ui8MultinetAddress;
        tFrameTx.operationID = ALTO_Operation_Get;
        tFrameTx.length = 0x00;
        vALTOFrame_BCC_fill(&tFrameTx);
        vALTOFrame_create_ASCII(txbuff, &tFrameTx);

        i32Index = 0;
        isSent = false;
        while (!Queue_empty(hMsgQ)) {
            // Implicit cast from (Queue_Elem *) to (ALTOMultinetRemoteRoutingMsg *)
//            msg = Queue_dequeue(hMsgQ);
            msg = Queue_get(hMsgQ);
            tORTInput = msg->tORTInput;

            // Free memory
//            Queue_put(hFreeQueue, (Queue_Elem *) msg);
            Memory_free(NULL, msg, sizeof(ALTOMultinetRemoteRoutingMsgObj));

            ptORTInput = &tORTInput;
            tFrameTx.data[i32Index] = *(uint8_t *)ptORTInput;

            if (++i32Index >= ALTO_FRAME_DATA_PAYLOAD_SIZE) {
                tFrameTx.operationID = ALTO_Operation_Set;
                tFrameTx.length = i32Index;
                i32Index = 0;
                isSent = true;
                vALTOFrame_BCC_fill(&tFrameTx);
                vALTOFrame_create_ASCII(txbuff, &tFrameTx);

                vALTOMultinet_relayControllerTransferRemote(ui32SerialPort,
                                                            txbuff,
                                                            rxbuff,
                                                            &tFrameRx,
                                                            hControllersListQueue);
            }
        }

        if (i32Index) {
            tFrameTx.operationID = ALTO_Operation_Set;
            isSent = false;
        }else {
            tFrameTx.operationID = ALTO_Operation_Get;
        }

        if (!isSent) {
            tFrameTx.length = i32Index;

            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);


            vALTOMultinet_relayControllerTransferRemote(ui32SerialPort,
                                                        txbuff,
                                                        rxbuff,
                                                        &tFrameRx,
                                                        hControllersListQueue);
        }
        Task_sleep(100);
    }

}



void vALTOMultinet_relayControllerTransferRemote(unsigned int ui32SerialPort,
                                                 char *pTxMsgBuff70,
                                                 char *pRxMsgBuff70,
                                                 ALTO_Frame_t *ptFrameRx,
                                                 Queue_Handle hControllersListQueue)
{
    int i32Index;
    tALTOORTRouting *ptORT;
    tALTOORTInput tORTInput;
    int8_t i8bcc;
    uint8_t multinetAddress;


    IF_Handle ifHandle;
    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;


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
            if (ptFrameRx->classID == ALTO_Class_Clu) {
                switch(ptFrameRx->operationID) {
                case ALTO_Operation_Response:
                    vALTOFrame_ORTConvertDataFromBigEndianToLittleEndian(ptFrameRx->data, ptFrameRx->length);
                    ptORT = (tALTOORTRouting *)ptFrameRx->data;
                    for (i32Index = 0; i32Index < ptFrameRx->length / 2; i32Index++) {
                        tORTInput.value = ptORT->inputState;
                        tORTInput.inputAddress = ptORT->destinationVirtualInput;
                        tORTInput.reserved = 0;

                        switch(ptORT->destinationType) {
                        case ALTO_Multinet_RelayBox:
                        case ALTO_Multinet_TemperatureBox:
                            multinetAddress = ((ptORT->destinationType & 0x07) << 5) | (ptORT->destinationAddress & 0x1F);
                            ALTOMultinetRemoteRoutingMsgObj *pMsgObj = ptALTOMultinet_SendMsgToControllerWorker(hControllersListQueue,
                                                                                                                multinetAddress,
                                                                                                                &tORTInput);
                            if (pMsgObj == NULL) {};

                            break;
                        default:
                            break;
                        }
                        ptORT++;
                    }
                    break;
                case ALTO_Operation_ACKNAK:
                    if (ptFrameRx->length) {
                        switch(ptFrameRx->data[0]) {
                        case ALTO_AckNak_GoodStatus:
                            break;
                        case ALTO_AckNak_Busy_WaitingToRespond:
                            break;
                        case ALTO_AckNak_DiagSessionNotActive:
                            break;
                        case ALTO_AckNak_HardwareFault:
                            break;
                        case ALTO_AckNak_InvalidCRC:
                            break;
                        case ALTO_AckNak_FunctionNotExecuted:
                            break;
                        case ALTO_AckNak_InvalidArgument:
                            break;
                        case ALTO_AckNak_InvalidBcc:
                            break;
                        default:
                            break;
                        }
                    }
                    break;
                default:
                    break;
                }
            }

        }
    }
}






void vALTOMultinet_relayControllerWorker_Params_init(ALTOMultinetDeviceControllerWorker_Params *params)
{
    params->ui32InterfaceIndex = IF_SERIAL_6;
    params->ui8VersionMajor = 0;
    params->ui8VersionMinor = 0;
    params->ui8MultinetAddress = 0;
    params->hControllersListQueue = NULL;
}

Task_Handle xALTOMultinet_relayControllerWorker_init(void *arg0, void *ptControllerWorkerQParams)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.instance->name = "RelayControllerWorker" "1";
    taskParams.stackSize = RELAYCONTROLLERDEVICEWORKER_TASK_STACK_SIZE;
    taskParams.priority = RELAYCONTROLLERDEVICEWORKER_TASK_PRIORITY;
    taskParams.arg0 = (UArg)arg0;
    taskParams.arg1 = (UArg)ptControllerWorkerQParams;
    ALTOMultinetDeviceControllerWorker_Params *params = &(((ALTOMultinetDeviceControllerWorkerList_t *)ptControllerWorkerQParams)->worker.controllerWorkerParams);
    switch(params->ui8VersionMajor) {
    case 1:
        switch (params->ui8VersionMinor) {
        case 0:
            break;
        default:
            break;
        }
//        break;
    default:
        taskHandle = Task_create((Task_FuncPtr)vALTOMultinet_relayControllerWorkerFxnV00, &taskParams, &eb);
        break;
    }

    return taskHandle;
}



