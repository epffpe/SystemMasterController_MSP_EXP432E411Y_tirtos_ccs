/*
 * AVDSRS485Device.c
 *
 *  Created on: Apr 22, 2020
 *      Author: epffpe
 */




#define __DEVICES_AVDS_AVDSRS485DEVICE_GLOBAL
#include "includes.h"


static void vAVDS485Device_InitializeCommunication( IF_Handle ifHandle);
static void vAVDS485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
//static void vAVDS485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static void vAVDS485Device_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);

//uint32_t xAVDS485Device_createMsgFrame(AVDS485Device_directCommandData_t *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address);
//uint32_t xAVDS485Device_SingleBlueRayDVDCreateMsgFrame(AVDS485Device_directCommandData_t *ptsCmdBuf, uint32_t command, uint32_t address);


void vAVDS485Device_close(DeviceList_Handler handle);
DeviceList_Handler hAVDS485Device_open(DeviceList_Handler handle, void *params);


int xAVDS485Device_createMsgFrameWithCRC(char *pCmdBuffer, char *pCmdData, uint32_t packetLength);
int16_t xAVDS485Device_CRC_calculateFull(const void *pSource, size_t sourceBytes, uint16_t *pui16Result);
void vAVDS485Device_InitWrapper(AVDS485Device_Command_Wrapper *pWrapper, uint16_t length);
void vAVDS485Device_cmdFrameInitialezeComunications(AVDS485Device_Command_Initialize_Communication *pCmd);
int xAVDS485Device_cmdFrameSetChannel(AVDS485Device_Command_set_channel *pCmd,
                                      uint16_t outputChannel,
                                      uint16_t audioInputChannel,
                                      uint16_t videoInputChannel);
int xAVDS485Device_cmdFrameGetChannel(AVDS485Device_Command_get_channel *pCmd,
                                      uint16_t outputChannel);
int xAVDS485Device_cmdFrameSetControlProperty(AVDS485Device_Command_setControlProperty *pCmd,
                                              uint16_t outputChannel,
                                              uint8_t property,
                                              uint32_t value);
int xAVDS485Device_cmdFrameGetControlProperty(AVDS485Device_Command_getControlProperty *pCmd,
                                              uint16_t outputChannel,
                                              uint8_t property);
int xAVDS485Device_cmdFrameVolumeSet(AVDS485Device_Command_setControlProperty *pCmd,
                                     uint16_t outputChannel,
                                     uint32_t value);
int xAVDS485Device_cmdFrameMuteSet(AVDS485Device_Command_setControlProperty *pCmd,
                                     uint16_t outputChannel,
                                     uint32_t value);
int xAVDS485Device_cmdFrameBassSet(AVDS485Device_Command_setControlProperty *pCmd,
                                     uint16_t outputChannel,
                                     uint32_t value);
int xAVDS485Device_cmdFrameVolumeGet(AVDS485Device_Command_getControlProperty *pCmd,
                                     uint16_t outputChannel);
int xAVDS485Device_cmdFrameMuteGet(AVDS485Device_Command_getControlProperty *pCmd,
                                     uint16_t outputChannel);
int xAVDS485Device_cmdFrameBassGet(AVDS485Device_Command_getControlProperty *pCmd,
                                     uint16_t outputChannel);

const Device_FxnTable g_AVDS485Device_fxnTable =
{
 .closeFxn = vAVDS485Device_close,
 .openFxn = hAVDS485Device_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};


Void vAVDS485Device_taskFxn(UArg arg0, UArg arg1)
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

    //    uint32_t ui32SerialIndex;
    //    char rxbuff[70];
    //    char txbuff[70];
    volatile tEEPROM_Data *psEEPROMData;

    //    Task_Handle taskTCPServerHandle;
    //    Task_Params taskParams;
    Error_Block eb;


    ASSERT(arg1 != NULL);


    if (arg1 == NULL) {
        return;
    }
    devHandle = (DeviceList_Handler)arg1;
    eventHandle = devHandle->eventHandle;
    msgQHandle = devHandle->msgQHandle;
    clockHandle = devHandle->clockHandle;
    myDeviceID = devHandle->deviceID;

    psEEPROMData = INFO_get();

    Display_printf(g_SMCDisplay, 0, 0, "AVDS DeviceId (%d) started\n", myDeviceID);

    if ((uint32_t)arg0 == IF_SERIAL_5) {
        DOSet(DIO_UART_DEBUG, 0);
        DOSet(DIO_SERIAL5_EN_, 1);
    }

    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (uint32_t)arg0 < 8 ? psEEPROMData->serialBaudRate[(uint32_t)arg0] : DEFAULT_EEPROM_SERIAL_BAUDRATE;
    //    ifHandle = hIF_open(IF_SERIAL_4, &params);
    ifHandle = hIF_open((uint32_t)arg0, &params);


    //    /*
    //     * Prepare the transfer
    //     */
    //    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    //    ifTransaction.readCount = sizeof(rxbuff);
    //    ifTransaction.readBuf = rxbuff;
    //    ifTransaction.readTimeout = 30;
    //    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_NONE;
    ////    ifTransaction.writeBuf = txbuff;
    //    ifTransaction.writeBuf = txbuff;
    //    ifTransaction.writeCount = sizeof(txbuff);
    //    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    //    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    vAVDS485Device_InitializeCommunication(ifHandle);

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;

            // set channel
            char tempBuff[sizeof(char_data_t) + sizeof(AVDS485Device_serviceSteveCommand_charSetChannel_data)];
            char_data_t *pCharData = (char_data_t *)tempBuff;
            pCharData->paramID = CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_SET_ID;
            AVDS485Device_serviceSteveCommand_charSetChannel_data *pData = (AVDS485Device_serviceSteveCommand_charSetChannel_data *)pCharData->data;
            pData->outputChannel = 1;
            pData->audioInputChannel = 2;
            pData->videoInputChannel = 3;


            vAVDS485Device_SteveCommandsService_ValueChangeHandler(pCharData, arg0, arg1, ifHandle, NULL, NULL);
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

        // TODO: Implement messages
        // Process messages sent from another task or another context.
        while (!Queue_empty(msgQHandle))
        {
            pMsg = Queue_dequeue(msgQHandle);

            //            vAVDS485Device_processApplicationMessage(pMsg, arg0, arg1, ifHandle, txbuff, rxbuff);
            vAVDS485Device_processApplicationMessage(pMsg, arg0, arg1, ifHandle, NULL, NULL);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }
}



void vAVDS485Device_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vAVDS485Device_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_AVDS485;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_AVDS485Device_fxnTable;
}


void vAVDS485Device_close(DeviceList_Handler handle)
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

DeviceList_Handler hAVDS485Device_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_AVDS;


    //    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


    Clock_Params_init(&paramsUnion.clockParams);
    paramsUnion.clockParams.period = DEVICES_AVDS_AVDSRS485DEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vAVDS485Device_clockHandler, DEVICES_AVDS_AVDSRS485DEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = DEVICES_AVDS_AVDSRS485DEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = DEVICES_AVDS_AVDSRS485DEVICE_TASK_PRIORITY;
    //    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg0 = (UArg)deviceParams->arg0;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vAVDS485Device_taskFxn, &paramsUnion.taskParams, &eb);

    return handle;
}

/*
 * This should be the first command written to the AVDS after a reset or offline event.
 * The AVDS will not respond to any command until it has received the Initialize Communication command.
 * This guarantees that the control system will detect any offline event that occurs.
 */


static void vAVDS485Device_InitializeCommunication( IF_Handle ifHandle)
{
    IF_Transaction ifTransaction;
    bool transferOk;

    AVDS485Device_Command_Initialize_Communication cmdTx;
    AVDS485Device_Command_Initialize_Communication_Response cmdRx;

    ASSERT(ifHandle != NULL);

    if(ifHandle == NULL) {
        return;
    }


    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = sizeof(AVDS485Device_Command_Initialize_Communication_Response);
    ifTransaction.readBuf = &cmdRx;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_AVDS485;
    ifTransaction.writeBuf = &cmdTx;
    ifTransaction.writeCount = sizeof(AVDS485Device_Command_Initialize_Communication);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    vAVDS485Device_cmdFrameInitialezeComunications(&cmdTx);


    transferOk = bIF_transfer(ifHandle, &ifTransaction);
    if (transferOk) {
        //        uint16_t *pui16PacketLength;
        CRC_Handle handle;
        CRC_Params params;
        int_fast16_t status;
        uint32_t result;


        /* Set data processing options, including endianness control */
        CRC_Params_init(&params);
        params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
        params.dataSize = CRC_DATA_SIZE_8BIT;
        params.seed = 0xFFFF;
        params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
        //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

        /* Open the driver using the settings above */
        handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
        if (handle == NULL)
        {
            /* If the handle is already open, execution will stop here */
            return;
        }

        result = 0;
        //        /* Calculate the CRC of all 32 bytes in the source array */
        //        status = CRC_calculateFull(handle, &cmdRx.command, 5, &result);
        //        if (status != CRC_STATUS_SUCCESS)
        //        {
        //            CRC_close(handle);
        //            /* If the CRC engine is busy or if an error occurs execution will stop here */
        //            return;
        //        }

        status = CRC_addData(handle, &cmdRx.command, 1);
        //        if (status != CRC_STATUS_SUCCESS)
        //        {
        //            /* If the CRC engine is busy or if an error occurs execution will stop here */
        //            while(1);
        //        }

        //        cmdRx.result = ntohl(cmdRx.result);
        status = CRC_addData(handle, &cmdRx.result, 4);

        /* Extract the result from the internal state */
        CRC_finalize(handle, &result);

        /* Close the driver to allow other users to access this driver instance */
        CRC_close(handle);

        uint16_t crc;
        crc = ntohs(cmdRx.crc);
        if (crc == (result & 0x0000FFFF)) {
            status = status;
        }

    }

}




static void vAVDS485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    char_data_t *pCharData = (char_data_t *)pMsg->pdu;

    switch (pMsg->type)
    {
    case APP_MSG_SERVICE_WRITE: /* Message about received value write */
        /* Call different handler per service */
        switch(pCharData->svcUUID) {
        case SERVICE_AVDSRS485DEVICE_STEVE_COMMANDS_UUID:
            vAVDS485Device_SteveCommandsService_ValueChangeHandler(pCharData, arg0, arg1, ifHandle, txbuff, rxbuff);
            break;
        default:
            break;
        }

        break;
        case APP_MSG_SERVICE_CFG: /* Message about received CCCD write */
            /* Call different handler per service */
            //      switch(pCharData->svcUUID) {
            //        case BUTTON_SERVICE_SERV_UUID:
            //          user_ButtonService_CfgChangeHandler(pCharData);
            //          break;
            //        case DATA_SERVICE_SERV_UUID:
            //          user_DataService_CfgChangeHandler(pCharData);
            //          break;
            //      }
            break;

    }
}
//
//static void vAVDS485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
//{
//    AVDS485Device_directCommandData_t cmd;
//
////    IF_Params params;
//    IF_Transaction ifTransaction;
//    bool transferOk;
//    AVDS485Device_directCommandData_t *pRxData = (AVDS485Device_directCommandData_t *)pCharData->data;
//
//    /*
//     * Prepare the transfer
//     */
//    memset(&ifTransaction, 0, sizeof(IF_Transaction));
//    ifTransaction.readCount = 0;
//    ifTransaction.readBuf = rxbuff;
//    ifTransaction.readTimeout = 30;
//    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_NONE;
////    ifTransaction.writeBuf = txbuff;
//    ifTransaction.writeBuf = &cmd;
//    ifTransaction.writeCount = sizeof(AVDS485Device_directCommandData_t);
//    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
//    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;
//
//
//    xAVDS485Device_createMsgFrame(&cmd,
//                                   pRxData->header,
//                                   pRxData->command,
//                                   pRxData->address);
//    switch (pCharData->paramID) {
//    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_ALTO_EMULATOR_GET_ID:
//        break;
//    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_ALTO_EMULATOR_SET_ID:
//        transferOk = bIF_transfer(ifHandle, &ifTransaction);
//        if (transferOk) {
//
//        }
//        break;
//    default:
//        break;
//    }
//}




static void vAVDS485Device_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
    IF_Transaction ifTransaction;
    AVDS485Device_serviceSteveCommand_charSetChannel_data *pSetChannelData;
    bool transferOk;
    uint16_t ui16Result;

    union {
        AVDS485Device_Command_set_channel cmdSetCh;
        AVDS485Device_Command_get_channel cmdGetCh;
        AVDS485Device_Command_setControlProperty cmdSetCtlProperty;
        AVDS485Device_Command_getControlProperty cmdGetCtlProperty;
    } bufferTxUnion;

    union {
        AVDS485Device_Command_set_channel_Response cmdSetChResp;
        AVDS485Device_Command_get_channel_Response cmdGetChResp;
        AVDS485Device_Command_setControlProperty_Response cmdSetCtlPropertyResp;
        AVDS485Device_Command_getControlProperty_Response cmdGetCtlPropertyResp;
    } bufferRxUnion;


    if(arg1 == NULL) {
        return;
    }
    if(pCharData == NULL) {
        return;
    }

    if(ifHandle == NULL) {
        return;
    }

    devHandle = (DeviceList_Handler)arg1;
    myDeviceID = devHandle->deviceID;

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 0;
    ifTransaction.readBuf = &bufferRxUnion.cmdGetChResp;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_AVDS485;
    //    ifTransaction.writeBuf = txbuff;
    //    ifTransaction.writeBuf = &cmd;
    ifTransaction.writeCount = 0;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;



    switch (pCharData->paramID) {
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_SET_ID:
        pSetChannelData = (AVDS485Device_serviceSteveCommand_charSetChannel_data *)pCharData->data;
        xAVDS485Device_cmdFrameSetChannel(&bufferTxUnion.cmdSetCh,
                                          pSetChannelData->outputChannel,
                                          pSetChannelData->audioInputChannel,
                                          pSetChannelData->videoInputChannel);
        ifTransaction.writeBuf = &bufferTxUnion.cmdSetCh;
        ifTransaction.readBuf = &bufferRxUnion.cmdSetChResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_set_channel);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_set_channel_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            bufferRxUnion.cmdSetChResp.wrapper.packetLength = ntohs(bufferRxUnion.cmdSetChResp.wrapper.packetLength);
            bufferRxUnion.cmdSetChResp.crc = ntohs(bufferRxUnion.cmdSetChResp.crc);
            if (xAVDS485Device_CRC_calculateFull(&bufferRxUnion.cmdSetChResp.command,
                                                 bufferRxUnion.cmdSetChResp.wrapper.packetLength,
                                                 &ui16Result) == CRC_STATUS_SUCCESS)
            {
                if (ui16Result == bufferRxUnion.cmdSetChResp.crc)
                {
                    AVDS485Device_serviceSteveCommand_charSetChannelResp_data *pSetChannelResData = (AVDS485Device_serviceSteveCommand_charSetChannelResp_data *)&bufferRxUnion.cmdSetChResp.result;
//                    vDevice_sendCharDataMsg (pCharData->retDeviceID,
//                                             APP_MSG_SERVICE_WRITE,
//                                             pCharData->connHandle,
//                                             pCharData->retSvcUUID, pCharData->retParamID,
//                                             myDeviceID,
//                                             SERVICE_AVDSRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_SET_ID,
//                                             (uint8_t *)pSetChannelResData, sizeof(AVDS485Device_serviceSteveCommand_charSetChannelResp_data));
                }
            }
        }

        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_GET_ID:
        xAVDS485Device_cmdFrameGetChannel(&bufferTxUnion.cmdGetCh, 1);
        ifTransaction.writeBuf = &bufferTxUnion.cmdGetCh;
        ifTransaction.readBuf = &bufferRxUnion.cmdGetChResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_get_channel);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_get_channel_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_VOLUME_SET_ID:
        xAVDS485Device_cmdFrameVolumeSet(&bufferTxUnion.cmdSetCtlProperty, 1, 303);
        ifTransaction.writeBuf = &bufferTxUnion.cmdSetCtlProperty;
        ifTransaction.readBuf = &bufferRxUnion.cmdSetCtlPropertyResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_setControlProperty);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_setControlProperty_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_VOLUME_GET_ID:
        xAVDS485Device_cmdFrameVolumeGet(&bufferTxUnion.cmdGetCtlProperty, 1);
        ifTransaction.writeBuf = &bufferTxUnion.cmdGetCtlProperty;
        ifTransaction.readBuf = &bufferRxUnion.cmdGetCtlPropertyResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_getControlProperty);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_getControlProperty_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_MUTE_SET_ID:
        xAVDS485Device_cmdFrameMuteSet(&bufferTxUnion.cmdSetCtlProperty, 1, 1);
        ifTransaction.writeBuf = &bufferTxUnion.cmdSetCtlProperty;
        ifTransaction.readBuf = &bufferRxUnion.cmdSetCtlPropertyResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_setControlProperty);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_setControlProperty_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_MUTE_GET_ID:
        xAVDS485Device_cmdFrameMuteGet(&bufferTxUnion.cmdGetCtlProperty, 1);
        ifTransaction.writeBuf = &bufferTxUnion.cmdGetCtlProperty;
        ifTransaction.readBuf = &bufferRxUnion.cmdGetCtlPropertyResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_getControlProperty);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_getControlProperty_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_BASS_SET_ID:
        xAVDS485Device_cmdFrameBassSet(&bufferTxUnion.cmdSetCtlProperty, 1, 10);
        ifTransaction.writeBuf = &bufferTxUnion.cmdSetCtlProperty;
        ifTransaction.readBuf = &bufferRxUnion.cmdSetCtlPropertyResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_setControlProperty);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_setControlProperty_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_BASS_GET_ID:
        xAVDS485Device_cmdFrameBassGet(&bufferTxUnion.cmdGetCtlProperty, 1);
        ifTransaction.writeBuf = &bufferTxUnion.cmdGetCtlProperty;
        ifTransaction.readBuf = &bufferRxUnion.cmdGetCtlPropertyResp;
        ifTransaction.writeCount = sizeof(AVDS485Device_Command_getControlProperty);
        ifTransaction.readCount = sizeof(AVDS485Device_Command_getControlProperty_Response);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;

    default:
        break;
    }
}

/* ==================================================================================
 * ==================================================================================
 *
 */

int xAVDS485Device_createMsgFrameWithCRC(char *pCmdBuffer, char *pCmdData, uint32_t packetLength)
{
    uint16_t *pui16PacketLength;
    uint8_t *pui8Crc;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    pCmdBuffer[0] = 0xAA;
    pCmdBuffer[1] = 0x55;
    pui16PacketLength =  (uint16_t *)&pCmdBuffer[2];
    *pui16PacketLength++ = htons(packetLength);
    memcpy(pui16PacketLength, pCmdData, packetLength);

    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
    //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        return -1;
    }

    result = 0;
    /* Calculate the CRC of all 32 bytes in the source array */
    status = CRC_calculateFull(handle, pCmdData, packetLength, &result);
    if (status != CRC_STATUS_SUCCESS)
    {
        CRC_close(handle);
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        return -2;
    }
    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    pui8Crc = (uint8_t *)pui16PacketLength + packetLength;
    pui8Crc[1] = result & 0xFF;
    pui8Crc[0] = (result >> 8) & 0xFF;

    return (6+packetLength);
}

int16_t xAVDS485Device_CRC_calculateFull(const void *pSource, size_t sourceBytes, uint16_t *pui16Result)
{
    //    uint16_t *pui16PacketLength;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;


    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
    //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        return CRC_STATUS_RESOURCE_UNAVAILABLE;
    }

    result = 0;
    /* Calculate the CRC of all 32 bytes in the source array */
    status = CRC_calculateFull(handle, pSource, sourceBytes, &result);
    if (status != CRC_STATUS_SUCCESS)
    {
        CRC_close(handle);
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        return status;
    }
    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    *pui16Result = (uint16_t) (result & 0x0000FFFF);
    return status;
}


void vAVDS485Device_InitWrapper(AVDS485Device_Command_Wrapper *pWrapper, uint16_t length)
{
    pWrapper->preamble1 = 0xAA;
    pWrapper->preamble2 = 0x55;
    pWrapper->packetLength = htons(length);
}

void vAVDS485Device_cmdFrameInitialezeComunications(AVDS485Device_Command_Initialize_Communication *pCmd)
{
    uint16_t crc;
    vAVDS485Device_InitWrapper(&pCmd->wrapper, AVDS485DEVICE_COMMAND_INITIALIZE_COMMUNICATIONS_SIZE);
    pCmd->command = AVDS485DEVICE_COMMAND_INITIALIZE_COMMUNICATIONS;

    xAVDS485Device_CRC_calculateFull(&pCmd->command, AVDS485DEVICE_COMMAND_INITIALIZE_COMMUNICATIONS_SIZE, &crc);
    pCmd->crc = htons(crc);
}

int xAVDS485Device_cmdFrameSetChannel(AVDS485Device_Command_set_channel *pCmd,
                                      uint16_t outputChannel,
                                      uint16_t audioInputChannel,
                                      uint16_t videoInputChannel)
{
    uint16_t crc;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    vAVDS485Device_InitWrapper(&pCmd->wrapper, AVDS485DEVICE_COMMAND_SET_CHANNEL_SIZE);
    pCmd->command = AVDS485DEVICE_COMMAND_SET_CHANNEL;

    pCmd->outputChannel = htons(outputChannel);
    pCmd->audioInputChannel = htons(audioInputChannel);
    pCmd->videoInputChannel = htons(videoInputChannel);

    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
    //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        return CRC_STATUS_RESOURCE_UNAVAILABLE;
    }
    result = 0;

    status = CRC_calculateFull(handle, &pCmd->command, AVDS485DEVICE_COMMAND_SET_CHANNEL_SIZE, &result);

    //    result = 0;
    //    status = CRC_addData(handle, &pCmd->command, 1);
    //    status = CRC_addData(handle, &pCmd->outputChannel, 2);
    //    status = CRC_addData(handle, &pCmd->audioInputChannel, 2);
    //    status = CRC_addData(handle, &pCmd->videoInputChannel, 2);
    //
    //    /* Extract the result from the internal state */
    //    CRC_finalize(handle, &result);
    if (status){

    }


    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    crc = (result & 0x0000FFFF);
    pCmd->crc = htons(crc);

    return 1;
}

int xAVDS485Device_cmdFrameGetChannel(AVDS485Device_Command_get_channel *pCmd,
                                      uint16_t outputChannel)
{
    uint16_t crc;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    vAVDS485Device_InitWrapper(&pCmd->wrapper, AVDS485DEVICE_COMMAND_GET_CHANNEL_SIZE);
    pCmd->command = AVDS485DEVICE_COMMAND_GET_CHANNEL;

    pCmd->outputChannel = htons(outputChannel);

    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
    //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        return CRC_STATUS_RESOURCE_UNAVAILABLE;
    }
    result = 0;

    status = CRC_calculateFull(handle, &pCmd->command, AVDS485DEVICE_COMMAND_GET_CHANNEL_SIZE, &result);
    if (status){

    }

    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    crc = (result & 0x0000FFFF);
    pCmd->crc = htons(crc);

    return 1;
}


int xAVDS485Device_cmdFrameSetControlProperty(AVDS485Device_Command_setControlProperty *pCmd,
                                              uint16_t outputChannel,
                                              uint8_t property,
                                              uint32_t value)
{
    uint16_t crc;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    vAVDS485Device_InitWrapper(&pCmd->wrapper, AVDS485DEVICE_COMMAND_SET_CONTROL_PROPERTY_SIZE);
    pCmd->command = AVDS485DEVICE_COMMAND_SET_CONTROL_PROPERTY;

    pCmd->outputChannel = htons(outputChannel);
    pCmd->property = htons(property);
    pCmd->value = htons(value);

    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
    //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        return CRC_STATUS_RESOURCE_UNAVAILABLE;
    }
    result = 0;

    status = CRC_calculateFull(handle, &pCmd->command, AVDS485DEVICE_COMMAND_SET_CONTROL_PROPERTY_SIZE, &result);
    if (status){

    }

    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    crc = (result & 0x0000FFFF);
    pCmd->crc = htons(crc);

    return 1;
}


int xAVDS485Device_cmdFrameGetControlProperty(AVDS485Device_Command_getControlProperty *pCmd,
                                              uint16_t outputChannel,
                                              uint8_t property)
{
    uint16_t crc;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    vAVDS485Device_InitWrapper(&pCmd->wrapper, AVDS485DEVICE_COMMAND_GET_CONTROL_PROPERTY_SIZE);
    pCmd->command = AVDS485DEVICE_COMMAND_GET_CONTROL_PROPERTY;

    pCmd->outputChannel = htons(outputChannel);
    pCmd->property = htons(property);

    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
    //    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        return CRC_STATUS_RESOURCE_UNAVAILABLE;
    }
    result = 0;

    status = CRC_calculateFull(handle, &pCmd->command, AVDS485DEVICE_COMMAND_GET_CONTROL_PROPERTY_SIZE, &result);
    if (status){

    }

    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    crc = (result & 0x0000FFFF);
    pCmd->crc = htons(crc);

    return 1;
}

int xAVDS485Device_cmdFrameVolumeSet(AVDS485Device_Command_setControlProperty *pCmd,
                                     uint16_t outputChannel,
                                     uint32_t value)
{
    return xAVDS485Device_cmdFrameSetControlProperty(pCmd,
                                                     outputChannel,
                                                     AVDS485DEVICE_PROPERTY_VOLUME,
                                                     value);
}


int xAVDS485Device_cmdFrameMuteSet(AVDS485Device_Command_setControlProperty *pCmd,
                                   uint16_t outputChannel,
                                   uint32_t value)
{
    return xAVDS485Device_cmdFrameSetControlProperty(pCmd,
                                                     outputChannel,
                                                     AVDS485DEVICE_PROPERTY_MUTE,
                                                     value);
}




int xAVDS485Device_cmdFrameBassSet(AVDS485Device_Command_setControlProperty *pCmd,
                                   uint16_t outputChannel,
                                   uint32_t value)
{
    return xAVDS485Device_cmdFrameSetControlProperty(pCmd,
                                                     outputChannel,
                                                     AVDS485DEVICE_PROPERTY_BASS,
                                                     value);
}


int xAVDS485Device_cmdFrameVolumeGet(AVDS485Device_Command_getControlProperty *pCmd,
                                     uint16_t outputChannel)
{
    return xAVDS485Device_cmdFrameGetControlProperty(pCmd,
                                                     outputChannel,
                                                     AVDS485DEVICE_PROPERTY_VOLUME);
}


int xAVDS485Device_cmdFrameMuteGet(AVDS485Device_Command_getControlProperty *pCmd,
                                   uint16_t outputChannel)
{
    return xAVDS485Device_cmdFrameGetControlProperty(pCmd,
                                                     outputChannel,
                                                     AVDS485DEVICE_PROPERTY_MUTE);
}


int xAVDS485Device_cmdFrameBassGet(AVDS485Device_Command_getControlProperty *pCmd,
                                   uint16_t outputChannel)
{
    return xAVDS485Device_cmdFrameGetControlProperty(pCmd,
                                                     outputChannel,
                                                     AVDS485DEVICE_PROPERTY_BASS);
}
