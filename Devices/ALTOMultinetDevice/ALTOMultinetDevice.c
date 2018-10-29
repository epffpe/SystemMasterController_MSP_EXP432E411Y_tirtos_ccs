/*
 * ALTOMultinetDevice.c
 *
 *  Created on: Mar 30, 2018
 *      Author: epenate
 */

#define __DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_GLOBAL
#include "includes.h"



typedef struct {
    Queue_Handle hControllersListQueue;
    uint32_t ui32InterfaceIndex;
}ALTOMultinetDeviceDiscovery_Args;



void vALTOMultinetDevice_close(DeviceList_Handler handle);
DeviceList_Handler hALTOMultinetDevice_open(DeviceList_Handler handle, void *params);
static void vALTOMultinet_processApplicationMessage(device_msg_t *pMsg, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff);


static ALTOMultinetDeviceControllerWorkerList_t *ptALTOMultinet_findControllerWorkerInList(Queue_Handle hControllersListQueue,
                                                                                           uint8_t ui8MultinetAddress);

static ALTOMultinetDeviceControllerWorkerList_t *ptALTOMultinet_addControllerWorkerToList(Queue_Handle hControllersListQueue,
                                                                                          uint32_t fatherDeviceID,
                                                                                          ALTOMultinetDeviceControllerWorker_Params *pWorkerParams);

static void vALTOMultinet_getDetailedStatusService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID);


static void vALTOMultinet_LocalRemoteService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID);


static int xALTOMultinet_readGateFrom(uint8_t multinetAddress, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static int xALTOMultinet_setGateAt(uint8_t multinetAddress, uint8_t inputAddress, uint8_t value, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static int xALTOMultinet_setVirtualRemoteAt(uint8_t multinetAddress, uint8_t inputAddress, uint8_t value, IF_Handle ifHandle, char *txbuff, char *rxbuff);


Void vALTOMultinetDeviceDiscovery(UArg arg0, UArg arg1);

const Device_FxnTable g_ALTOMultinetDevice_fxnTable =
{
 .closeFxn = vALTOMultinetDevice_close,
 .openFxn = hALTOMultinetDevice_open,
 .sendMsgFxn = NULL,
};





Void vALTOMultinetDeviceFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
    Event_Handle eventHandle;
    Queue_Handle msgQHandle;
    Clock_Handle clockHandle;
    device_msg_t *pMsg;

    IF_Handle ifHandle;
    IF_Params params;
    //    IF_Transaction ifTransaction;
    //    bool transferOk;
    //    ALTO_Frame_t tFrameTx;
    //    ALTO_Frame_t tFrameRx;
    char rxbuff[70];
    char txbuff[70];

    //    int i32Index;

    //    int8_t i8bcc;
    //    ALTOMultinetDeviceControllerWorkerList_t *pWorker;
    //    ALTOMultinetDeviceControllerWorker_Params workerParams;
    ALTOMultinetDeviceDiscovery_Args discoveryArgs;


    Error_Block eb;

    Queue_Handle hControllersListQueue;

    Task_Handle tasMultinetDiscoverykHandle;
    Task_Params taskParams;

    ASSERT(arg1 != NULL);

    if (arg1 == NULL) {
        return;
    }

    devHandle = (DeviceList_Handler)arg1;
    eventHandle = devHandle->eventHandle;
    msgQHandle = devHandle->msgQHandle;
    clockHandle = devHandle->clockHandle;
    myDeviceID = devHandle->deviceID;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);


    hControllersListQueue = Queue_create(NULL, NULL);



    /*
     * Initialize interface
     */
    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
    ifHandle = hIF_open(IF_SERIAL_6, &params);

    //    vALTOMultinet_relayControllerWorker_Params_init(&workerParams);
    //    workerParams.hControllersListQueue = hControllersListQueue;
    //    workerParams.ui32InterfaceIndex = IF_SERIAL_6;

    discoveryArgs.hControllersListQueue = hControllersListQueue;
    discoveryArgs.ui32InterfaceIndex = IF_SERIAL_6;


    Task_Params_init(&taskParams);
    taskParams.stackSize = ALTOMULTINETDEVICE_TASK_STACK_SIZE;
    taskParams.priority = ALTOMULTINETDEVICE_DISCOVERY_TASK_PRIORITY;
    taskParams.arg0 = (UArg)&discoveryArgs;
    tasMultinetDiscoverykHandle = Task_create((Task_FuncPtr)vALTOMultinetDeviceDiscovery, &taskParams, &eb);

    /*
     * Fill the tx frame
     */
    //    vALTOFrame_Params_init(&tFrameTx);
    //    tFrameTx.classID = ALTO_Class_Diagnostic;
    //    tFrameTx.operationID = ALTO_Operation_Get;
    //    tFrameTx.functionID = ALTO_Function_ManufacturingInformation;
    //    tFrameTx.length = 0x00;
    //    tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
    //    tFrameTx.ui4address = 1 & 0x1F;
    //    vALTOFrame_BCC_fill(&tFrameTx);
    //    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    /*
     * Prepare the transfer
     */
    //    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    //    ifTransaction.readCount = sizeof(rxbuff);
    //    ifTransaction.readBuf = rxbuff;
    //    ifTransaction.readTimeout = 30;
    //    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    //    ifTransaction.writeBuf = txbuff;
    //    ifTransaction.writeCount = sizeof(txbuff);
    //    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    //    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;

            //            char i8address;
            //
            //            tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
            //            for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
            //                tFrameTx.ui4address = i8address & 0x1F;
            //                vALTOFrame_BCC_fill(&tFrameTx);
            //                vALTOFrame_create_ASCII(txbuff, &tFrameTx);
            //
            //                ifTransaction.readCount = 64;
            //                ifTransaction.writeCount = sizeof(txbuff);
            //                transferOk = bIF_transfer(ifHandle, &ifTransaction);
            //                if (transferOk) {
            //                    xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            //                    i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            //                    if (!i8bcc) {
            //                        i8bcc = sizeof(ALTOMultinetDeviceRelay_detailedStatus);
            //                        ALTOMultinetDeviceManufacturingInformation *pManufacturingInformation = (ALTOMultinetDeviceManufacturingInformation *)tFrameRx.data;
            //                        workerParams.ui8VersionMajor = pManufacturingInformation->SwVersionMajor;
            //                        workerParams.ui8VersionMinor = pManufacturingInformation->SwVersionMinor;
            //                        workerParams.ui8MultinetAddress = tFrameRx.ui8MultinetAddress;
            //
            //                        if (!ptALTOMultinet_findControllerWorkerInList(hControllersListQueue, workerParams.ui8MultinetAddress)) {
            //                            pWorker = ptALTOMultinet_addControllerWorkerToList(hControllersListQueue,
            //                                                                               myDeviceID,
            //                                                                               &workerParams);
            //                        }
            //                    }
            //                }
            //
            //            }
            //
            //
            //            tFrameTx.uin4unitType = ALTO_Multinet_TemperatureBox;
            //            for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
            //                tFrameTx.ui4address = i8address & 0x1F;
            //                vALTOFrame_BCC_fill(&tFrameTx);
            //                vALTOFrame_create_ASCII(txbuff, &tFrameTx);
            //
            //                ifTransaction.readCount = 64;
            //                ifTransaction.writeCount = sizeof(txbuff);
            //                transferOk = bIF_transfer(ifHandle, &ifTransaction);
            //                if (transferOk) {
            //                    xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            //                    i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            //                    if (!i8bcc) {
            //                        ALTOMultinetDeviceManufacturingInformation *pManufacturingInformation = (ALTOMultinetDeviceManufacturingInformation *)tFrameRx.data;
            //                        workerParams.ui8VersionMajor = pManufacturingInformation->SwVersionMajor;
            //                        workerParams.ui8VersionMinor = pManufacturingInformation->SwVersionMinor;
            //                        workerParams.ui8MultinetAddress = tFrameRx.ui8MultinetAddress;
            //
            //                        if (!ptALTOMultinet_findControllerWorkerInList(hControllersListQueue, workerParams.ui8MultinetAddress)) {
            ////                            pWorker = ptALTOMultinet_addControllerWorkerToList(hControllersListQueue,
            ////                                                                               myDeviceID,
            ////                                                                               &workerParams);
            //                        }
            //                    }
            //                }
            //
            //            }
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

            //TODO: Free all the dinamic memory allocated by the workers
            Task_delete(&tasMultinetDiscoverykHandle);
            Event_delete(&eventHandle);
            Task_exit();
        }

        // Process messages sent from another task or another context.
        while (!Queue_empty(msgQHandle))
        {
            pMsg = Queue_dequeue(msgQHandle);
            //            UInt prevPrio = Task_setPri(Task_self(), ALTOMULTINETDEVICE_TASK_PRIORITY + 2);
            // Process application-layer message probably sent from ourselves.
            vALTOMultinet_processApplicationMessage(pMsg, myDeviceID, ifHandle, txbuff, rxbuff);
            //            Task_setPri(Task_self(), prevPrio);
            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }
}



/*
 * @brief   Handle application messages
 *
 *          These are messages not from the BLE stack, but from the
 *          application itself.
 *
 *          For example, in a Software Interrupt (Swi) it is not possible to
 *          call any BLE APIs, so instead the Swi function must send a message
 *          to the application Task for processing in Task context.
 *
 * @param   pMsg  Pointer to the message of type app_msg_t.
 *
 * @return  None.
 */
static void vALTOMultinet_processApplicationMessage(device_msg_t *pMsg, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    char_data_t *pCharData = (char_data_t *)pMsg->pdu;

    switch (pMsg->type)
    {
    case APP_MSG_SERVICE_WRITE: /* Message about received value write */
        /* Call different handler per service */
        switch(pCharData->svcUUID) {
        case SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID:
            vALTOMultinet_getDetailedStatusService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_DeviceDetailedStatus);
            break;
        case SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID:
            vALTOMultinet_LocalRemoteService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_CluLocalRemoteVin);
            break;
        default:
            break;
        }

        break;
        default:
            break;

    }
}


static void vALTOMultinet_getDetailedStatusService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID)
{
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;
    int8_t i8bcc;

    /*
     * Fill the tx frame
     */

    uint8_t *pui8MultinetAddress = (uint8_t *)pCharData->data;
    ALTOMultinetDevice_multinetAddress *pMultinetAddress = (ALTOMultinetDevice_multinetAddress *)pCharData->data;

    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.functionID = functionID;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.length = 0x00;
    tFrameTx.ui8MultinetAddress = *pui8MultinetAddress;
    memset(tFrameTx.data, 0, sizeof(tFrameTx.data));
    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 70;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = 70;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;

    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.length = 0x00;
    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    ifTransaction.readCount = 64;
    ifTransaction.writeCount = 70;
    transferOk = bIF_transfer(ifHandle, &ifTransaction);


    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_MULTINET_DETAILED_STATUS_GET_ID:
//        tFrameTx.operationID = ALTO_Operation_Get;
//        tFrameTx.length = 0x00;
//
//        vALTOFrame_BCC_fill(&tFrameTx);
//        vALTOFrame_create_ASCII(txbuff, &tFrameTx);
//
//        ifTransaction.readCount = 64;
//        ifTransaction.writeCount = 70;
//        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                if (tFrameRx.operationID == ALTO_Operation_Response) {
                    ALTOMultinetDeviceRelay_detailedStatus *pDetailedStatus = (ALTOMultinetDeviceRelay_detailedStatus *)tFrameRx.data;
                    vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                             APP_MSG_SERVICE_WRITE,
                                             pCharData->connHandle,
                                             pCharData->retSvcUUID, pCharData->retParamID,
                                             myDeviceID,
                                             SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID, CHARACTERISTIC_ALTO_MULTINET_DETAILED_STATUS_GET_ID,
                                             (uint8_t *)pDetailedStatus, sizeof(ALTOMultinetDeviceRelay_detailedStatus) );
                }else{
                    // NoACK

                }
            }
        }
        break;

    case CHARACTERISTIC_ALTO_MULTINET_STATUS_RELAY_GET_ID:
//        tFrameTx.operationID = ALTO_Operation_Get;
//        tFrameTx.length = 0x00;
//        vALTOFrame_BCC_fill(&tFrameTx);
//        vALTOFrame_create_ASCII(txbuff, &tFrameTx);
//
//        ifTransaction.readCount = 64;
//        ifTransaction.writeCount = 70;
//        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                if (tFrameRx.operationID == ALTO_Operation_Response) {
                    ALTOMultinetDeviceRelay_getStatusRelayResponse relay;
                    ALTOMultinetDeviceRelay_getStatusRelay *pStatusRelay = (ALTOMultinetDeviceRelay_getStatusRelay *)pCharData->data;
                    relay.sender = *pStatusRelay;
                    if (pMultinetAddress->ui4unitType == ALTO_Multinet_RelayBox) {
//                        ALTOMultinetDeviceRelay_getStatusRelay *pStatusRelay = (ALTOMultinetDeviceRelay_getStatusRelay *)pCharData->data;
                        ALTOMultinetDeviceRelay_detailedStatus *pDetailedStatus = (ALTOMultinetDeviceRelay_detailedStatus *)tFrameRx.data;

//                        pDetailedStatus->v28v5 = htons(pDetailedStatus->v28v5);
//                        pDetailedStatus->v5 = htons(pDetailedStatus->v5);
//                        pDetailedStatus->v8 = htons(pDetailedStatus->v8);
//                        pDetailedStatus->v28[0] = htons(pDetailedStatus->v28[0]);
//                        pDetailedStatus->v28[1] = htons(pDetailedStatus->v28[1]);
//                        pDetailedStatus->v28[2] = htons(pDetailedStatus->v28[2]);


//                        ALTOMultinetDeviceRelay_getStatusRelayResponse relay;
                        //                    relay.ui8MultinetAddress = tFrameRx.ui8MultinetAddress;
                        //                    relay.relayBitPosition = pStatusRelay->relayBitPosition;
//                        relay.sender = *pStatusRelay;
                        if (pStatusRelay->relayBitPosition < 8) {
                            relay.relayState = (pDetailedStatus->relay & (1 << pStatusRelay->relayBitPosition)) ? 1:0;
                        }else{
                            relay.relayState = (pDetailedStatus->indicator & (1 << (pStatusRelay->relayBitPosition - 8))) ? 1:0;
                        }
//                        vDevice_sendCharDataMsg (pCharData->retDeviceID,
//                                                 APP_MSG_SERVICE_WRITE,
//                                                 pCharData->connHandle,
//                                                 pCharData->retSvcUUID, pCharData->retParamID,
//                                                 myDeviceID,
//                                                 SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID, CHARACTERISTIC_ALTO_MULTINET_STATUS_RELAY_GET_ID,
//                                                 (uint8_t *)&relay, sizeof(ALTOMultinetDeviceRelay_getStatusRelayResponse) );
                    }else if (pMultinetAddress->ui4unitType == ALTO_Multinet_TemperatureBox) {
                        ALTOMultinetDeviceTemp_detailedStatus *pDetailedStatus = (ALTOMultinetDeviceTemp_detailedStatus *)tFrameRx.data;
                        uint32_t ui32Input = (uint32_t)pDetailedStatus->indicator | (((uint32_t)pDetailedStatus->indicator16) << 24);
                        if (pStatusRelay->relayBitPosition < 8) {
                            relay.relayState = (pDetailedStatus->relay & (1 << pStatusRelay->relayBitPosition)) ? 1:0;
                        }else{
                            relay.relayState = (ui32Input & (1 << (pStatusRelay->relayBitPosition - 8))) ? 1:0;
                        }

                    }
                    vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                             APP_MSG_SERVICE_WRITE,
                                             pCharData->connHandle,
                                             pCharData->retSvcUUID, pCharData->retParamID,
                                             myDeviceID,
                                             SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID, CHARACTERISTIC_ALTO_MULTINET_STATUS_RELAY_GET_ID,
                                             (uint8_t *)&relay, sizeof(ALTOMultinetDeviceRelay_getStatusRelayResponse) );
                }else{
                    // NoACK

                }
            }
        }
        break;
    case CHARACTERISTIC_ALTO_MULTINET_DETAILED_POT_GET_ID:
        if (transferOk) {
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                if (tFrameRx.operationID == ALTO_Operation_Response) {
                    ALTOMultinetDeviceRelay_getDetailedStatusPotResponse potResponse;
                    potResponse.sender = *pMultinetAddress;
                    if (pMultinetAddress->ui4unitType == ALTO_Multinet_RelayBox) {

                    }else if (pMultinetAddress->ui4unitType == ALTO_Multinet_TemperatureBox) {
                        ALTOMultinetDeviceTemp_detailedStatus *pDetailedStatus = (ALTOMultinetDeviceTemp_detailedStatus *)tFrameRx.data;
                        potResponse.pot1Step = pDetailedStatus->pot1Step;
                        potResponse.pot2Step = pDetailedStatus->pot2Step;
                        potResponse.dac1Step = pDetailedStatus->dac1Step;
                        potResponse.dac2Step = pDetailedStatus->dac2Step;
                        potResponse.dac3Step = pDetailedStatus->dac3Step;
                        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                                 APP_MSG_SERVICE_WRITE,
                                                 pCharData->connHandle,
                                                 pCharData->retSvcUUID, pCharData->retParamID,
                                                 myDeviceID,
                                                 SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID, CHARACTERISTIC_ALTO_MULTINET_DETAILED_POT_GET_ID,
                                                 (uint8_t *)&potResponse, sizeof(ALTOMultinetDeviceRelay_getDetailedStatusPotResponse) );
                    }
                }
            }else{
                // NoACK

            }
        }
        break;
    default:
        break;
    }
}


static void vALTOMultinet_LocalRemoteService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID)
{
    ALTOMultinetDeviceRelay_setSwitchOnOff *pPayload;
    ALTOMultinetDeviceRelay_setSwitchOnOffResponse response;

    pPayload = (ALTOMultinetDeviceRelay_setSwitchOnOff *)pCharData->data;
    response.sender = *pPayload;
    response.acknak = 0;


    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_ID:
    {
        int gate = xALTOMultinet_readGateFrom(pPayload->ui8MultinetAddress, ifHandle, txbuff, rxbuff);
        int ack = xALTOMultinet_setVirtualRemoteAt(pPayload->ui8MultinetAddress,
                                                   pPayload->tORTInput.inputAddress,
                                                   pPayload->tORTInput.value,
                                                   ifHandle, txbuff, rxbuff);
        response.acknak = ack;
        if (!(gate & (1 << pPayload->tORTInput.inputAddress))) {
            xALTOMultinet_setGateAt(pPayload->ui8MultinetAddress,
                                    pPayload->tORTInput.inputAddress,
                                    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_VIRTUAL,
                                    ifHandle, txbuff, rxbuff);
            xALTOMultinet_setGateAt(pPayload->ui8MultinetAddress,
                                    pPayload->tORTInput.inputAddress,
                                    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_PHYSICAL,
                                    ifHandle, txbuff, rxbuff);
        }
        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 pCharData->connHandle,
                                 pCharData->retSvcUUID, pCharData->retParamID,
                                 myDeviceID,
                                 SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_ID,
                                 (uint8_t *)&response, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOffResponse) );
    }
    break;
    case CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_TRIGGER_ID:
    {
        int gate = xALTOMultinet_readGateFrom(pPayload->ui8MultinetAddress, ifHandle, txbuff, rxbuff);
        int ack = xALTOMultinet_setVirtualRemoteAt(pPayload->ui8MultinetAddress,
                                                   pPayload->tORTInput.inputAddress,
                                                   1,
                                                   ifHandle, txbuff, rxbuff);
        response.acknak = ack;
        if (!(gate & (1 << pPayload->tORTInput.inputAddress))) {
            xALTOMultinet_setGateAt(pPayload->ui8MultinetAddress,
                                    pPayload->tORTInput.inputAddress,
                                    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_VIRTUAL,
                                    ifHandle, txbuff, rxbuff);
        }
        ack = xALTOMultinet_setVirtualRemoteAt(pPayload->ui8MultinetAddress,
                                               pPayload->tORTInput.inputAddress,
                                               0,
                                               ifHandle, txbuff, rxbuff);
        if (!(gate & (1 << pPayload->tORTInput.inputAddress))) {

            xALTOMultinet_setGateAt(pPayload->ui8MultinetAddress,
                                    pPayload->tORTInput.inputAddress,
                                    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_PHYSICAL,
                                    ifHandle, txbuff, rxbuff);
        }
        response.acknak = ack;
        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 pCharData->connHandle,
                                 pCharData->retSvcUUID, pCharData->retParamID,
                                 myDeviceID,
                                 SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_TRIGGER_ID,
                                 (uint8_t *)&response, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOffResponse)  );
    }
    break;
    case CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_GET_GATE_ID:
    {
        int gate = xALTOMultinet_readGateFrom(pPayload->ui8MultinetAddress, ifHandle, txbuff, rxbuff);
        response.acknak = (gate & (1 << pPayload->tORTInput.inputAddress)) ? 1:0;
        //        uint8_t ui8gate = (gate & (1 << pPayload->tORTInput.inputAddress)) ? 1:0;
        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 pCharData->connHandle,
                                 pCharData->retSvcUUID, pCharData->retParamID,
                                 myDeviceID,
                                 SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_GET_GATE_ID,
                                 (uint8_t *)&response, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOffResponse) );
    }
    break;
    case CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_GATE_ID:
    {

        int ack = xALTOMultinet_setGateAt(pPayload->ui8MultinetAddress,
                                          pPayload->tORTInput.inputAddress,
                                          pPayload->tORTInput.value,
                                          ifHandle, txbuff, rxbuff);
        response.acknak = ack;
        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 pCharData->connHandle,
                                 pCharData->retSvcUUID, pCharData->retParamID,
                                 myDeviceID,
                                 SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_GATE_ID,
                                 (uint8_t *)&response, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOffResponse)  );
    }
    break;
    default:
        break;
    }// end switch(pCharData->paramID)

}

static ALTOMultinetDeviceControllerWorkerList_t *ptALTOMultinet_findControllerWorkerInList(Queue_Handle hControllersListQueue,
                                                                                           uint8_t ui8MultinetAddress)
{
    Queue_Elem *elem;
    ALTOMultinetDeviceControllerWorkerList_t *pWorker, *pRetWorker = NULL;

    for (elem = Queue_head(hControllersListQueue); elem != (Queue_Elem *)hControllersListQueue; elem = Queue_next(elem)) {
        pWorker = (ALTOMultinetDeviceControllerWorkerList_t *)elem;
        if (pWorker->worker.controllerWorkerParams.ui8MultinetAddress == ui8MultinetAddress) {
            pRetWorker = pWorker;
        }
    }
    return pRetWorker;
}

static ALTOMultinetDeviceControllerWorkerList_t *ptALTOMultinet_addControllerWorkerToList(Queue_Handle hControllersListQueue,
                                                                                          uint32_t fatherDeviceID,
                                                                                          ALTOMultinetDeviceControllerWorker_Params *pWorkerParams)
{
    Error_Block eb;

    ASSERT(hControllersListQueue != NULL);
    ASSERT(pWorkerParams != NULL);

    if (hControllersListQueue == NULL) {
        return NULL;
    }
    if (pWorkerParams == NULL) {
        return NULL;
    }

    Error_init(&eb);

    ALTOMultinetDeviceControllerWorkerList_t *pWorker = Memory_alloc(NULL,
                                                                     sizeof(ALTOMultinetDeviceControllerWorkerList_t),
                                                                     0, &eb);

    if (pWorker != NULL)
    {
        pWorker->fatherDeviceID = fatherDeviceID;
        pWorker->worker.controllerWorkerParams = *pWorkerParams;
        pWorker->worker.hMsgQ = Queue_create(NULL, NULL);

        pWorker->taskHandle = xALTOMultinet_relayControllerWorker_init((void *)pWorkerParams->ui8MultinetAddress, (void *)pWorker);

        if (pWorker->taskHandle) {
            pWorker->worker.bIsEnabled = true;
            Queue_enqueue(hControllersListQueue, &pWorker->_elem);
        }else {
            Queue_delete(&pWorker->worker.hMsgQ);
            Memory_free(NULL, pWorker, sizeof(ALTOMultinetDeviceControllerWorkerList_t));
            pWorker = NULL;
        }
    }
    return pWorker;
}

ALTOMultinetRemoteRoutingMsgObj *ptALTOMultinet_SendMsgToControllerWorker(Queue_Handle hControllersListQueue,
                                                                          uint8_t ui8MultinetAddress,
                                                                          tALTOORTInput *pMsg)
{
    Error_Block eb;
    ALTOMultinetRemoteRoutingMsgObj *msg = NULL;

    Error_init(&eb);
    ALTOMultinetDeviceControllerWorkerList_t *pControllerWorker = ptALTOMultinet_findControllerWorkerInList(hControllersListQueue, ui8MultinetAddress);
    if (pControllerWorker) {
        if (pControllerWorker->worker.bIsEnabled) {
            msg = (ALTOMultinetRemoteRoutingMsgObj *)Memory_alloc(NULL,
                                                                  sizeof(ALTOMultinetRemoteRoutingMsgObj),
                                                                  0, &eb);
            if (msg != NULL) {
                msg->tORTInput = *pMsg;
                Queue_put(pControllerWorker->worker.hMsgQ , &msg->elem);
            }
        }
    }
    return msg;
}


static int xALTOMultinet_readGateFrom(uint8_t multinetAddress, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;
    ALTOMultinetDeviceRelay_CluBitMap *pcluBitMap;
    int retVal = 0;
    int8_t i8bcc;

    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Clu;
    tFrameTx.functionID = ALTO_Function_CluBitMap;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.length = 0x00;
    tFrameTx.ui8MultinetAddress = multinetAddress;
    memset(tFrameTx.data, 0, sizeof(tFrameTx.data));
    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 70;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = 70;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    ifTransaction.readCount = 64;
    ifTransaction.writeCount = 70;
    transferOk = bIF_transfer(ifHandle, &ifTransaction);                                                            /* 1- Read Gate state */

    if (transferOk) {
        xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
        i8bcc = cALTOFrame_BCC_check(&tFrameRx);
        if (!i8bcc) {
            switch (tFrameRx.operationID) {
            case ALTO_Operation_ACKNAK:
                retVal = -1;
                break;
            case ALTO_Operation_Response:
                pcluBitMap = (ALTOMultinetDeviceRelay_CluBitMap *)tFrameRx.data;
//                retVal = htonl(pcluBitMap->gate);
                retVal = pcluBitMap->gate;
                break;
            default:
                break;
            }
        }
    }
    return retVal;
}


static int xALTOMultinet_setGateAt(uint8_t multinetAddress, uint8_t inputAddress, uint8_t value, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;
    int8_t *pAck;
    ALTOMultinetDeviceRelay_CluPinVinGate_inputSpecifier specifier;
    int retVal = -1;
    int8_t i8bcc;

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 70;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = 70;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Clu;
    tFrameTx.functionID = ALTO_Function_CluPinVinGate;
    tFrameTx.operationID = ALTO_Operation_Set;
    tFrameTx.ui8MultinetAddress = multinetAddress;
    memset(tFrameTx.data, 0, sizeof(tFrameTx.data));

    tFrameTx.length = 0x01;

    specifier.inputAddress = inputAddress;
    specifier.value = (value) ? SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_VIRTUAL : SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_PHYSICAL;
    specifier.singleWildcard = SERVICE_ALTO_MULTINET_CLUPINVINGATE_SINGLE_UNIT;
    tFrameTx.data[0] = *(uint8_t *)&specifier;

    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    ifTransaction.readCount = 64;
    ifTransaction.writeCount = 70;
    transferOk = bIF_transfer(ifHandle, &ifTransaction);                                        /* 2- Set Gate to virtual */
    if (transferOk) {
        xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
        i8bcc = cALTOFrame_BCC_check(&tFrameRx);
        if (!i8bcc) {
            if (tFrameRx.operationID == ALTO_Operation_ACKNAK) {
                pAck = (int8_t *)tFrameRx.data;
                if (*pAck == ALTO_AckNak_GoodStatus) {
                    retVal = *pAck;
                }
            }
        }
    }
    return retVal;
}


static int xALTOMultinet_setVirtualRemoteAt(uint8_t multinetAddress, uint8_t inputAddress, uint8_t value, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;
    tALTOORTInput tORTInput;
    int8_t *pAck;
    int retVal = -1;
    int8_t i8bcc;

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 70;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = 70;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Clu;
    tFrameTx.functionID = ALTO_Function_CluLocalRemoteVin;
    tFrameTx.operationID = ALTO_Operation_Set;
    tFrameTx.length = 0x01;
    tFrameTx.ui8MultinetAddress = multinetAddress;
    memset(tFrameTx.data, 0, sizeof(tFrameTx.data));

    tORTInput.inputAddress = inputAddress;
    tORTInput.value = value;
    tORTInput.reserved = 0;
    tFrameTx.data[0] = *(uint8_t *)&tORTInput;

    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    ifTransaction.readCount = 64;
    ifTransaction.writeCount = 70;
    transferOk = bIF_transfer(ifHandle, &ifTransaction);
    if (transferOk) {
        xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
        i8bcc = cALTOFrame_BCC_check(&tFrameRx);
        if (!i8bcc) {
            if (tFrameRx.operationID == ALTO_Operation_ACKNAK) {
                pAck = (int8_t *)tFrameRx.data;
                if (*pAck == ALTO_AckNak_GoodStatus) {
                    retVal = *pAck;
                }
            }else{
                // NoACK

            }
        }
    }

    return retVal;
}







/*
 * *********************************************************************************************************
 */



void vALTOMultinetDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}


Void vALTOMultinetDeviceDiscovery(UArg arg0, UArg arg1)
{
    char i8address;
    uint32_t myDeviceID;
//    DeviceList_Handler devHandle;

    IF_Handle ifHandle;
    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx;
    ALTO_Frame_t tFrameRx;
    char rxbuff[70];
    char txbuff[70];

    int8_t i8bcc;
    ALTOMultinetDeviceControllerWorkerList_t *pWorker;
    ALTOMultinetDeviceControllerWorker_Params workerParams;
    Queue_Handle hControllersListQueue;

    /*
     * Initialize interface
     */
    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
    ifHandle = hIF_open(IF_SERIAL_6, &params);

    ALTOMultinetDeviceDiscovery_Args *args = (ALTOMultinetDeviceDiscovery_Args *)arg0;

    vALTOMultinet_relayControllerWorker_Params_init(&workerParams);
    workerParams.hControllersListQueue = args->hControllersListQueue;
    workerParams.ui32InterfaceIndex = args->ui32InterfaceIndex;
    hControllersListQueue = args->hControllersListQueue;
    /*
     * Fill the tx frame
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.functionID = ALTO_Function_ManufacturingInformation;
    tFrameTx.length = 0x00;
    tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
    tFrameTx.ui4address = 1 & 0x1F;
    vALTOFrame_BCC_fill(&tFrameTx);
    vALTOFrame_create_ASCII(txbuff, &tFrameTx);

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = sizeof(rxbuff);
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET;
    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeCount = sizeof(txbuff);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;

    while(1) {
        tFrameTx.uin4unitType = ALTO_Multinet_RelayBox;
        for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
            tFrameTx.ui4address = i8address & 0x1F;
            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            ifTransaction.readCount = 64;
            ifTransaction.writeCount = sizeof(txbuff);
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {
                xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    i8bcc = sizeof(ALTOMultinetDeviceRelay_detailedStatus);
                    ALTOMultinetDeviceManufacturingInformation *pManufacturingInformation = (ALTOMultinetDeviceManufacturingInformation *)tFrameRx.data;
                    workerParams.ui8VersionMajor = pManufacturingInformation->SwVersionMajor;
                    workerParams.ui8VersionMinor = pManufacturingInformation->SwVersionMinor;
                    workerParams.ui8MultinetAddress = tFrameRx.ui8MultinetAddress;

                    if (!ptALTOMultinet_findControllerWorkerInList(hControllersListQueue, workerParams.ui8MultinetAddress)) {
                        pWorker = ptALTOMultinet_addControllerWorkerToList(hControllersListQueue,
                                                                           myDeviceID,
                                                                           &workerParams);
                        pWorker = pWorker;
                    }
                }
            }

        }


        tFrameTx.uin4unitType = ALTO_Multinet_TemperatureBox;
        for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
            tFrameTx.ui4address = i8address & 0x1F;
            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            ifTransaction.readCount = 64;
            ifTransaction.writeCount = sizeof(txbuff);
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {
                xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    ALTOMultinetDeviceManufacturingInformation *pManufacturingInformation = (ALTOMultinetDeviceManufacturingInformation *)tFrameRx.data;
                    workerParams.ui8VersionMajor = pManufacturingInformation->SwVersionMajor;
                    workerParams.ui8VersionMinor = pManufacturingInformation->SwVersionMinor;
                    workerParams.ui8MultinetAddress = tFrameRx.ui8MultinetAddress;

                    if (!ptALTOMultinet_findControllerWorkerInList(hControllersListQueue, workerParams.ui8MultinetAddress)) {
                        //                            pWorker = ptALTOMultinet_addControllerWorkerToList(hControllersListQueue,
                        //                                                                               myDeviceID,
                        //                                                                               &workerParams);
                    }
                }
            }

        }
        Task_sleep(ALTOMULTINETDEVICE_CLOCK_PERIOD);
    }
}



void vALTOMultinetDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_ALTO_MULTINET;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_ALTOMultinetDevice_fxnTable;
}


void vALTOMultinetDevice_close(DeviceList_Handler handle)
{
    unsigned int key;

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

DeviceList_Handler hALTOMultinetDevice_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_ALTO_MULTINET;


    //    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


    Clock_Params_init(&paramsUnion.clockParams);
    paramsUnion.clockParams.period = ALTOMULTINETDEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vALTOMultinetDevice_clockHandler, ALTOMULTINETDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = ALTOMULTINETDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = ALTOMULTINETDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vALTOMultinetDeviceFxn, &paramsUnion.taskParams, &eb);

    return handle;
}
