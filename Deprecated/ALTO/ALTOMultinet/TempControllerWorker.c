/*
 * TempControllerWorker.c
 *
 *  Created on: Jan 29, 2018
 *      Author: epenate
 */


#define __TEMPCONTROLLERWORKER_GLOBAL
#include "includes.h"

extern void vRelayControllerTransferRemote(unsigned int ui32SerialPort,
                                                                char *pTxMsgBuff70,
                                                                char *pRxMsgBuff70,
                                                                ALTO_Frame_t *ptFrameRx,
                                                                ALTOMultinetControllerWorkerQ_Params *ptControllerWorkerQParams);


Void vTempControllerWorkerFxnV00(UArg arg0, UArg arg1)
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
        isSent = true;
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
//            tFrameTx.operationID = ALTO_Operation_Get;
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



void vTempControllerWorker_Params_init(RelayControllerWorker_Params *params) {
    vRelayControllerWorker_Params_init(params);
}

Task_Handle xTempControllerWorker_init(RelayControllerWorker_Params *params, void *ptControllerWorkerQParams)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.instance->name = "RelayControllerWorker" "1";
    taskParams.stackSize = TEMPCONTROLLERWORKER_TASK_STACK_SIZE;
    taskParams.priority = TEMPCONTROLLERWORKER_TASK_PRIORITY;
    taskParams.arg0 = (UArg)params;
    taskParams.arg1 = (UArg)ptControllerWorkerQParams;
    taskHandle = Task_create((Task_FuncPtr)vTempControllerWorkerFxnV00, &taskParams, &eb);

    return taskHandle;
}

