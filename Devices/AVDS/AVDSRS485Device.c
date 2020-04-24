/*
 * AVDSRS485Device.c
 *
 *  Created on: Apr 22, 2020
 *      Author: epffpe
 */




#define __DEVICES_AVDS_AVDSRS485DEVICE_GLOBAL
#include "includes.h"



static void vAVDS485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
//static void vAVDS485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static void vAVDS485Device_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);

//uint32_t xAVDS485Device_createMsgFrame(AVDS485Device_directCommandData_t *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address);
//uint32_t xAVDS485Device_SingleBlueRayDVDCreateMsgFrame(AVDS485Device_directCommandData_t *ptsCmdBuf, uint32_t command, uint32_t address);


void vAVDS485Device_close(DeviceList_Handler handle);
DeviceList_Handler hAVDS485Device_open(DeviceList_Handler handle, void *params);


int xAVDS485Device_createMsgFrame(char *pCmdBuffer, char *pCmdData, uint32_t packetLength);



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

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;
            char_data_t pCharData;
            pCharData.paramID = CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_SET_ID;

            vAVDS485Device_SteveCommandsService_ValueChangeHandler(&pCharData, arg0, arg1, ifHandle, NULL, NULL);
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

//    AVDS485Device_directCommandData_t cmd;
//    AVDS485Device_pingResponse pingResponse;
//    AVDS485Device_generalStatusResponse statusResponse;

//    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
//    AVDS485Device_SteveCommandData_t *pRxData = (AVDS485Device_SteveCommandData_t *)pCharData->data;

    char pCmdBuffer[16];
    char pRxBuffer[16];
    char cmdData;


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
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_AVDS485;
//    ifTransaction.writeBuf = txbuff;
//    ifTransaction.writeBuf = &cmd;
    ifTransaction.writeCount = 0;
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;



    switch (pCharData->paramID) {
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_SET_ID:

        cmdData = 0x22;
        ifTransaction.writeBuf = pCmdBuffer;
        ifTransaction.readBuf = pRxBuffer;
        ifTransaction.readCount = 8+5;
        int n = xAVDS485Device_createMsgFrame(pCmdBuffer, &cmdData, 1);
        if ( n > 0) {
            ifTransaction.writeCount = n;
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {

            }
        }
        break;
    case CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_GET_ID:
//        ifTransaction.readCount = sizeof(AVDS485Device_pingResponse);
//        ifTransaction.readBuf = &pingResponse;
//        xAVDS485Device_createMsgFrame(&cmd,
//                                       AVDS485_Header_Ping,
//                                       pRxData->command,
//                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
//            AVDS485Device_SteveCommandReturn1Data_t steveResponse1Data;
//            steveResponse1Data.ui8NetworkID = pRxData->address;
//            steveResponse1Data.ui8Value = pingResponse.ui1PowerOnOff;
//            vDevice_sendCharDataMsg (pCharData->retDeviceID,
//                                                     APP_MSG_SERVICE_WRITE,
//                                                     pCharData->connHandle,
//                                                     pCharData->retSvcUUID, pCharData->retParamID,
//                                                     myDeviceID,
//                                                     SERVICE_AVDSRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_POWER_GET_ID,
//                                                     (uint8_t *)&steveResponse1Data, sizeof(AVDS485Device_SteveCommandReturn1Data_t));
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
//
//uint32_t xAVDS485Device_createMsgFrame(AVDS485Device_directCommandData_t *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address)
//{
//    ASSERT(ptsCmdBuf != NULL);
//
//    if (ptsCmdBuf == NULL) {
//        return 0;
//    }
//    ptsCmdBuf->header = header;
//    ptsCmdBuf->command = command;
//    ptsCmdBuf->address = address;
//    return 3;
//}
//
//uint32_t xAVDS485Device_SingleBlueRayDVDCreateMsgFrame(AVDS485Device_directCommandData_t *ptsCmdBuf, uint32_t command, uint32_t address)
//{
//    ASSERT(ptsCmdBuf != NULL);
//    ASSERT(command <= 0xFF);
//    ASSERT((address >= ROSSEN_BlueRayDVD_Network_Address_20 ) && (address <= ROSSEN_BlueRayDVD_Network_Address_27));
//
//    if (ptsCmdBuf == NULL) {
//        return 0;
//    }
//    if ((address < ROSSEN_BlueRayDVD_Network_Address_20 ) || (address > ROSSEN_BlueRayDVD_Network_Address_27)) {
//        return 0;
//    }
//    return xAVDS485Device_createMsgFrame(ptsCmdBuf, AVDS_Header_Single_BlueRay_DVD, command, address);
//}

int xAVDS485Device_createMsgFrame(char *pCmdBuffer, char *pCmdData, uint32_t packetLength)
{
    uint32_t *pui32PacketLength;
    uint8_t *pui8Crc;
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    pCmdBuffer[0] = 0xAA;
    pCmdBuffer[1] = 0x55;
    pui32PacketLength =  (uint32_t *)&pCmdBuffer[2];
    *pui32PacketLength++ = packetLength;
    memcpy(pui32PacketLength, pCmdData, packetLength);

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
    status = CRC_calculateFull(handle, pCmdData, 4, &result);
    if (status != CRC_STATUS_SUCCESS)
    {
        CRC_close(handle);
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        return -2;
    }
    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);

    pui8Crc = (uint8_t *)pui32PacketLength + packetLength;
    *pui8Crc++ = result & 0xFF;
    *pui8Crc = (result >> 8) & 0xFF;

    return (8+packetLength);
}

