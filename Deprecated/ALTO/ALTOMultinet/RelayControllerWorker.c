/*
 * RelayControllerWorker.c
 *
 *  Created on: Jan 12, 2018
 *      Author: epenate
 */


#define __RELAYCONTROLLERWORKER_GLOBAL
#include "includes.h"


void vRelayControllerTransferRemote(unsigned int ui32SerialPort,
                                                                char *pTxMsgBuff70,
                                                                char *pRxMsgBuff70,
                                                                ALTO_Frame_t *ptFrameRx,
                                                                ALTOMultinetControllerWorkerQ_Params *ptControllerWorkerQParams);


Void vRelayControllerWorkerFxnV00(UArg arg0, UArg arg1)
{
    char rxbuff[70];
    char txbuff[70];
    int i32Index;
    ALTO_Frame_t tFrameTx, tFrameRx;
    tALTOORTInput tORTInput;
    tALTOORTInput *ptORTInput;
    int8_t i8bcc;
    RelayControllerWorker_Params *ptRelayControllerWorkerParams;
    ALTOMultinetControllerWorkerQ_Params *ptControllerWorkerQParams;
    Queue_Handle hFreeQueue;
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
    ptRelayControllerWorkerParams = (RelayControllerWorker_Params *)arg0;
    ui32SerialPort = ptRelayControllerWorkerParams->ui32SerialPortIndex;

    hMsgQ = ((ALTOMultinetRelays_Table *)arg0)->hMsgQ;

    ptControllerWorkerQParams = (ALTOMultinetControllerWorkerQ_Params *)arg1;
    hFreeQueue = ptControllerWorkerQParams->hFreeQueue;
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
            Queue_put(hFreeQueue, (Queue_Elem *) msg);

            ptORTInput = &tORTInput;
            tFrameTx.data[i32Index] = *(uint8_t *)ptORTInput;

            if (++i32Index >= ALTO_FRAME_DATA_PAYLOAD_SIZE) {
                tFrameTx.operationID = ALTO_Operation_Set;
                tFrameTx.length = i32Index;
                i32Index = 0;
                isSent = true;
                vALTOFrame_BCC_fill(&tFrameTx);
                vALTOFrame_create_ASCII(txbuff, &tFrameTx);

                vRelayControllerTransferRemote(ui32SerialPort,
                                               txbuff,
                                               rxbuff,
                                               &tFrameRx,
                                               ptControllerWorkerQParams);
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


            vRelayControllerTransferRemote(ui32SerialPort,
                                           txbuff,
                                           rxbuff,
                                           &tFrameRx,
                                           ptControllerWorkerQParams);
        }
        Task_sleep(100);
    }

}



void vRelayControllerTransferRemote(unsigned int ui32SerialPort,
                                    char *pTxMsgBuff70,
                                    char *pRxMsgBuff70,
                                    ALTO_Frame_t *ptFrameRx,
                                    ALTOMultinetControllerWorkerQ_Params *ptControllerWorkerQParams)
{
    int n, i32Index;
    IArg key;
    tALTOORTRouting *ptORT;
    tALTOORTInput tORTInput;
    int8_t i8bcc;
    Queue_Handle hFreeQueue;
    Queue_Handle hDestMsgQ;
    ALTOMultinetRemoteRoutingMsg msg;
    ALTOMultinetRelays_Table *atALTOMultinetRelayTable;
    ALTOMultinetRelays_Table *atALTOMultinetTempTable;
//    GateMutexPri_Handle gateMutex;

    hFreeQueue = ptControllerWorkerQParams->hFreeQueue;
    atALTOMultinetRelayTable = ptControllerWorkerQParams->atALTOMultinetRelayTable;
    atALTOMultinetTempTable = ptControllerWorkerQParams->atALTOMultinetTempTable;
//    gateMutex = ptControllerWorkerQParams->gateMutex;
    key = xSerialTransferMutex_enter(ui32SerialPort);
    {
        xBSPSerial_sendData(ui32SerialPort, pTxMsgBuff70, 70, 10);
        n = xBSPSerial_receiveALTOFrame(ui32SerialPort, pRxMsgBuff70, 20);
    }
    vSerialTransferMutex_leave(ui32SerialPort, key);
    if (n) {
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
                            /*
                             * If there is a task that can consume the message then put the message in the list queue
                             */
                            if (atALTOMultinetRelayTable[(uint8_t)(ptORT->destinationAddress) & 0x1F].bIsEnabled) {
                                hDestMsgQ = atALTOMultinetRelayTable[(uint8_t)(ptORT->destinationAddress) & 0x1F].hMsgQ;
                                if (!Queue_empty(hFreeQueue)) {
                                    msg = Queue_get(hFreeQueue);
                                    msg->tORTInput = tORTInput;
                                    Queue_put(hDestMsgQ, (Queue_Elem *) msg);
                                }
                            }
                            break;
                        case ALTO_Multinet_TemperatureBox:
                            /*
                             * If there is a task that can consume the message then put the message in the list queue
                             */
                            if (atALTOMultinetTempTable[(uint8_t)(ptORT->destinationAddress) & 0x1F].bIsEnabled) {
                                hDestMsgQ = atALTOMultinetTempTable[(uint8_t)(ptORT->destinationAddress) & 0x1F].hMsgQ;
                                if (!Queue_empty(hFreeQueue)) {
                                    msg = Queue_get(hFreeQueue);
                                    msg->tORTInput = tORTInput;
                                    Queue_put(hDestMsgQ, (Queue_Elem *) msg);
                                }
                            }
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





void vRelayControllerWorker_Params_init(RelayControllerWorker_Params *params) {
    params->ui32SerialPortIndex = Board_SerialMultinet;
    params->ui8VersionMajor = 0;
    params->ui8VersionMinor = 0;
    params->ui8MultinetAddress = 0;
    params->atALTOMultinetRelaysTable = 0;
}

Task_Handle xRelayControllerWorker_init(RelayControllerWorker_Params *params, void *ptControllerWorkerQParams)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.instance->name = "RelayControllerWorker" "1";
    taskParams.stackSize = RELAYCONTROLLERWORKER_TASK_STACK_SIZE;
    taskParams.priority = RELAYCONTROLLERWORKER_TASK_PRIORITY;
    taskParams.arg0 = (UArg)params;
    taskParams.arg1 = (UArg)ptControllerWorkerQParams;
    taskHandle = Task_create((Task_FuncPtr)vRelayControllerWorkerFxnV00, &taskParams, &eb);

    return taskHandle;
}



