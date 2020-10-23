/*
 * ALTOMultinet.c
 *
 *  Created on: Jan 12, 2018
 *      Author: epenate
 */


#define __ALTOMULTINET_GLOBAL
#include "includes.h"




Void vALTOMultinetBusDiscoverFxn(UArg arg0, UArg arg1)
{
    char buff[128];
    char txbuff[70];
    ALTOMultinetRelays_Table *atALTOMultinetRelaysTable;
    ALTOMultinetRelays_Table *atALTOMultinetTempTable;
    volatile int n;
    char i8address;
    ALTO_Frame_t tFrameTx, tFrameRx;
//    tALTOManufacturingInfo *ptManfInfo;
    IArg key;
    int i32Index;
    volatile int8_t i8bcc;
    ALTOMultinet_Params *ptAltoMultinetParams;
    RelayControllerWorker_Params tRelayControllerWorkerParams;
    ALTOMultinetControllerWorkerQ_Params tControllerWorkerQParams;
    uint32_t ui32SerialPort;
    Task_Handle taskHandle;
    Error_Block eb;
    Queue_Handle hFreeQueue;
    ALTOMultinetRemoteRoutingMsgObj *msg;

//    HeapBuf_Params heapParams;
//    char *buf[ALTOMULTINET_NUM_ORT_MSGS * sizeof(ALTOMultinetRemoteRoutingMsgObj)];
//    HeapBuf_Handle heap;

    ASSERT (arg0 != NULL);

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    ptAltoMultinetParams = (ALTOMultinet_Params *)arg0;
    ui32SerialPort = ptAltoMultinetParams->ui32SerialPortIndex;


//    HeapBuf_Params_init(&heapParams);
//    heapParams.blockSize = 128;
//    heapParams.numBlocks = 10;
//    heapParams.buf = buf;
//    heapParams.bufSize = ALTOMULTINET_NUM_ORT_MSGS * sizeof(ALTOMultinetRemoteRoutingMsgObj);
//    heap = HeapBuf_create(&heapParams, &eb);
//    if (heap == NULL) {
//        System_abort("HeapBuf create failed");
//    }

    // No parameters or Error block are needed to create a Queue.
    atALTOMultinetRelaysTable = (ALTOMultinetRelays_Table *) Memory_alloc(NULL, ALTOMULTINET_NUM_RELAYS_DEVICES * sizeof(ALTOMultinetRelays_Table), 0, &eb);
    if (atALTOMultinetRelaysTable == NULL) {
        System_abort("Memory allocation failed");
    }
    atALTOMultinetTempTable = (ALTOMultinetRelays_Table *) Memory_alloc(NULL, ALTOMULTINET_NUM_TEMP_DEVICES * sizeof(ALTOMultinetRelays_Table), 0, &eb);
    if (atALTOMultinetTempTable == NULL) {
        System_abort("Memory allocation failed");
    }

    // No parameters or Error block are needed to create a Queue.
    hFreeQueue = Queue_create(NULL, NULL);
    msg = (ALTOMultinetRemoteRoutingMsgObj *) Memory_alloc(NULL, ALTOMULTINET_NUM_ORT_MSGS * sizeof(ALTOMultinetRemoteRoutingMsgObj), 0, &eb);
    if (msg == NULL) {
        System_abort("Memory allocation failed");
    }
    /* Put all messages on freeQueue */
    for (i32Index = 0; i32Index < ALTOMULTINET_NUM_ORT_MSGS; msg++, i32Index++) {
        Queue_put(hFreeQueue, (Queue_Elem *) msg);
    }


    /*
     * Initialize RelayController Parameters
     */
    vRelayControllerWorker_Params_init(&tRelayControllerWorkerParams);
    tRelayControllerWorkerParams.ui32SerialPortIndex = ui32SerialPort;
    tRelayControllerWorkerParams.atALTOMultinetRelaysTable = atALTOMultinetRelaysTable;

    for (i32Index = 0; i32Index < ALTOMULTINET_NUM_RELAYS_DEVICES; i32Index++) {
        atALTOMultinetRelaysTable[i32Index].bIsEnabled = false;
        atALTOMultinetRelaysTable[i32Index].hMsgQ = Queue_create(NULL, &eb);
    }


    for (i32Index = 0; i32Index < ALTOMULTINET_NUM_RELAYS_DEVICES; i32Index++) {
        atALTOMultinetTempTable[i32Index].bIsEnabled = false;
        atALTOMultinetTempTable[i32Index].hMsgQ = Queue_create(NULL, &eb);
    }

    tControllerWorkerQParams.hFreeQueue = hFreeQueue;
    tControllerWorkerQParams.atALTOMultinetRelayTable = atALTOMultinetRelaysTable;
    tControllerWorkerQParams.atALTOMultinetTempTable = atALTOMultinetTempTable;
    tControllerWorkerQParams.gateMutex = GateMutexPri_create(NULL, &eb);

//    /* get message */
//    msg = Queue_get(hFreeQueue);
//    /* free msg */
//    Queue_put(atALTOMultinetRelaysTable[1].hMsgQ, (Queue_Elem *) msg);


    /*
     * Initialize ALTO frame parameters
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.functionID = ALTO_Function_ManufacturingInformation;
    tFrameTx.length = 0x00;


    while (1) {
        /*
         * Scan for Relay Boxes
         */
        tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
        tRelayControllerWorkerParams.ui4unitType = ALTO_Multinet_RelayBox;
        tRelayControllerWorkerParams.atALTOMultinetRelaysTable = atALTOMultinetRelaysTable;
        for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
            tFrameTx.ui4address = i8address & 0x1F;
            vALTOFrame_BCC_fill(&tFrameTx);

            vALTOFrame_create_ASCII(txbuff, &tFrameTx);
            key = xSerialTransferMutex_enter(ui32SerialPort);
            {
                xBSPSerial_sendData(ui32SerialPort, txbuff, 70, 10);
                n = xBSPSerial_receiveALTOFrame(ui32SerialPort, buff, 20);
            }
            vSerialTransferMutex_leave(ui32SerialPort, key);
            if (n) {
                xALTOFrame_convert_ASCII_to_binary(buff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
//                    ptManfInfo = (tALTOManufacturingInfo *)tFrameRx.data;
                    tRelayControllerWorkerParams.ui8MultinetAddress = tFrameTx.ui8MultinetAddress;
                    if (!atALTOMultinetRelaysTable[i8address].bIsEnabled) {
                        atALTOMultinetRelaysTable[i8address].bIsEnabled = true;
                        atALTOMultinetRelaysTable[i8address].tRelayControllerWorkerParams = tRelayControllerWorkerParams;
                        taskHandle = xRelayControllerWorker_init(&atALTOMultinetRelaysTable[i8address].tRelayControllerWorkerParams,
                                                                 (void *)&tControllerWorkerQParams);
                        if (taskHandle == NULL) {
                            System_printf("Failed to initialize the RelayController Worker\n");
                            System_flush();
                        }
                    }
                }

            }

        }
        /*
         * Scan for Relay Boxes
         */
        tFrameTx.uin4unitType = ALTO_Multinet_TemperatureBox;
        tRelayControllerWorkerParams.ui4unitType = ALTO_Multinet_TemperatureBox;
        tRelayControllerWorkerParams.atALTOMultinetRelaysTable = atALTOMultinetTempTable;
        for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
            tFrameTx.ui4address = i8address & 0x1F;
            vALTOFrame_BCC_fill(&tFrameTx);

            key = xSerialTransferMutex_enter(ui32SerialPort);
            {
                vALTOFrame_create_ASCII(txbuff, &tFrameTx);
                xBSPSerial_sendData(ui32SerialPort, txbuff, 70, 10);
                n = xBSPSerial_receiveALTOFrame(ui32SerialPort, buff, 20);
            }
            vSerialTransferMutex_leave(ui32SerialPort, key);
            if (n) {
                xALTOFrame_convert_ASCII_to_binary(buff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
//                    ptManfInfo = (tALTOManufacturingInfo *)tFrameRx.data;
                    tRelayControllerWorkerParams.ui8MultinetAddress = tFrameTx.ui8MultinetAddress;
                    if (!atALTOMultinetTempTable[i8address].bIsEnabled) {
                        atALTOMultinetTempTable[i8address].bIsEnabled = true;
                        atALTOMultinetTempTable[i8address].tRelayControllerWorkerParams = tRelayControllerWorkerParams;
                        taskHandle = xTempControllerWorker_init(&atALTOMultinetTempTable[i8address].tRelayControllerWorkerParams,
                                                                 (void *)&tControllerWorkerQParams);
                        if (taskHandle == NULL) {
                            System_printf("Failed to initialize the RelayController Worker\n");
                            System_flush();
                        }
                    }
                }

            }

        }

        Task_sleep(10000);
    }

}


Void vTestFxn(UArg arg0, UArg arg1)
{
    char buff[128];
    char txbuff[70];
    ALTOMultinetRelays_Table atALTOMultinetRelaysTable[32];
    volatile int n;
    char i8address;
    ALTO_Frame_t tFrameTx, tFrameRx;
    tALTOManufacturingInfo *ptManfInfo;
    IArg key;
    int i32Index;
    volatile int8_t i8bcc;
    ALTOMultinet_Params *ptAltoMultinetParams;
    RelayControllerWorker_Params tRelayControllerWorkerParams;
    uint32_t ui32SerialPort;
    Task_Handle taskHandle;

    ASSERT (arg0 != NULL);

    ptAltoMultinetParams = (ALTOMultinet_Params *)arg0;
    ui32SerialPort = ptAltoMultinetParams->ui32SerialPortIndex;

    i8bcc = i8bcc;
    ptManfInfo = ptManfInfo;

    /*
     * Initialize ALTO frame parameters
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.functionID = ALTO_Function_ManufacturingInformation;
    tFrameTx.length = 0x00;

    /*
     * Initialize RelayController Parameters
     */
    vRelayControllerWorker_Params_init(&tRelayControllerWorkerParams);
    tRelayControllerWorkerParams.ui32SerialPortIndex = ui32SerialPort;
    for (i32Index = 0; i32Index < 32; i32Index++) {
        atALTOMultinetRelaysTable[i32Index].bIsEnabled = false;
    }
    while (1) {
        /*
         * Scan for Relay Boxes
         */
        tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
        tRelayControllerWorkerParams.ui4unitType = ALTO_Multinet_RelayBox;
        for (i8address = 0; i8address < 32; i8address++) {
            tFrameTx.ui4address = i8address & 0x1F;
            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            key = xSerialTransferMutex_enter(ui32SerialPort);
            {
                xBSPSerial_sendData(ui32SerialPort, txbuff, 70, 10);
                n = xBSPSerial_receiveALTOFrame(ui32SerialPort, buff, 20);
            }
            vSerialTransferMutex_leave(ui32SerialPort, key);
            if (n) {
                xALTOFrame_convert_ASCII_to_binary(buff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    ptManfInfo = (tALTOManufacturingInfo *)tFrameRx.data;
                    tRelayControllerWorkerParams.ui8MultinetAddress = tFrameTx.ui8MultinetAddress;
                    if (!atALTOMultinetRelaysTable[i8address].bIsEnabled) {
                        atALTOMultinetRelaysTable[i8address].bIsEnabled = true;
                        atALTOMultinetRelaysTable[i8address].tRelayControllerWorkerParams = tRelayControllerWorkerParams;
                        taskHandle = xRelayControllerWorker_init(&atALTOMultinetRelaysTable[i8address].tRelayControllerWorkerParams, 0);
                        if (taskHandle == NULL) {
                            System_printf("Failed to initialize the RelayController Worker\n");
                            System_flush();
                        }
                    }
                }

            }

        }

        Task_sleep(10000);
    }

}




void vALTOMultinet_Params_init(ALTOMultinet_Params *params) {
    params->ui32SerialPortIndex = Board_SerialMultinet;
    params->ui32SerialDEPin = SMC_SERIAL0_DE;
    params->ui32SerialREPin = SMC_SERIAL0_RE;
}

Task_Handle xALTOMultinet_init(ALTOMultinet_Params *params)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.stackSize = ALTOMULTINET_TASK_STACK_SIZE;
    taskParams.priority = ALTOMULTINET_TASK_PRIORITY;
    taskParams.arg0 = (UArg)params;
    taskHandle = Task_create((Task_FuncPtr)vALTOMultinetBusDiscoverFxn, &taskParams, &eb);

    return taskHandle;
}





