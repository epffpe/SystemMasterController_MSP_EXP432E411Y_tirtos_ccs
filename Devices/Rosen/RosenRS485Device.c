/*
 * RosenRS485.c
 *
 *  Created on: Apr 6, 2020
 *      Author: epf
 */


#define __DEVICES_ROSEN_ROSENRS485DEVICE_GLOBAL
#include "includes.h"



static void vRosen485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static void vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static void vRosen485Device_CommandsClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static void vRosen485Device_UniversalLiftService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);
static void vRosen485Device_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff);

uint32_t xRosen485Device_createMsgFrame(Rosen485Device_directCommandData_t *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address);
uint32_t xRosen485Device_SingleBlueRayDVDCreateMsgFrame(Rosen485Device_directCommandData_t *ptsCmdBuf, uint32_t command, uint32_t address);


void vRosen485Device_close(DeviceList_Handler handle);
DeviceList_Handler hRosen485Device_open(DeviceList_Handler handle, void *params);


const Device_FxnTable g_Rosen485Device_fxnTable =
{
 .closeFxn = vRosen485Device_close,
 .openFxn = hRosen485Device_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};


Void vRosen485Device_taskFxn(UArg arg0, UArg arg1)
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

    Display_printf(g_SMCDisplay, 0, 0, "Rosen DeviceId (%d) started\n", myDeviceID);

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
            Rosen485Device_SteveCommandData_t *pRxData = (Rosen485Device_SteveCommandData_t *)pCharData.data;
            pCharData.paramID = CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_CONTROL_SET_ID;
            pRxData->command = ROSENSingleBlueRayDVDCommand_Menu_Up;
////            pRxData->command = 0x55;
//            pCharData.paramID = CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_POWER_GET_ID;
//            pRxData->command = 0x01;
            pRxData->address = ROSSEN_BlueRayDVD_Network_Address_20;
////            vRosen485Device_SteveCommandsService_ValueChangeHandler(&pCharData, NULL, NULL, ifHandle, txbuff, rxbuff);
            vRosen485Device_SteveCommandsService_ValueChangeHandler(&pCharData, arg0, arg1, ifHandle, NULL, NULL);
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

//            vRosen485Device_processApplicationMessage(pMsg, arg0, arg1, ifHandle, txbuff, rxbuff);
            vRosen485Device_processApplicationMessage(pMsg, arg0, arg1, ifHandle, NULL, NULL);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }
}



void vRosen485Device_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vRosen485Device_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_ROSEN485;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_Rosen485Device_fxnTable;
}


void vRosen485Device_close(DeviceList_Handler handle)
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

DeviceList_Handler hRosen485Device_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_ROSEN;


//    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


    Clock_Params_init(&paramsUnion.clockParams);
    paramsUnion.clockParams.period = DEVICES_ROSEN_ROSENRS485DEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vRosen485Device_clockHandler, DEVICES_ROSEN_ROSENRS485DEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = DEVICES_ROSEN_ROSENRS485DEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = DEVICES_ROSEN_ROSENRS485DEVICE_TASK_PRIORITY;
//    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg0 = (UArg)deviceParams->arg0;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vRosen485Device_taskFxn, &paramsUnion.taskParams, &eb);

    return handle;
}









static void vRosen485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_UUID:
          vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(pCharData, arg0, arg1, ifHandle, txbuff, rxbuff);
//          switch (pCharData->paramID) {
//          case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_GET_ID:
//          case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_SET_ID:
//              vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(pCharData, arg0, arg1, ifHandle, txbuff, rxbuff);
//              break;
//          default:
//              break;
//          }
          break;
      case SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_UUID:
          break;
      case SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_UUID:
          break;
      case SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_UUID:
          break;
      case SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_UUID:
          break;
      case SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_UUID:
          break;
      case SERVICE_ROSENRS485DEVICE_DUAL_BLUERAY_DVD_UUID:
          break;
      case SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID:
          vRosen485Device_SteveCommandsService_ValueChangeHandler(pCharData, arg0, arg1, ifHandle, txbuff, rxbuff);
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

static void vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    Rosen485Device_directCommandData_t cmd;

//    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
    Rosen485Device_directCommandData_t *pRxData = (Rosen485Device_directCommandData_t *)pCharData->data;

    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 0;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_NONE;
//    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeBuf = &cmd;
    ifTransaction.writeCount = sizeof(Rosen485Device_directCommandData_t);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    xRosen485Device_createMsgFrame(&cmd,
                                   pRxData->header,
                                   pRxData->command,
                                   pRxData->address);
    switch (pCharData->paramID) {
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_GET_ID:
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_SET_ID:
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    default:
        break;
    }
}

static void vRosen485Device_UniversalLiftService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    Rosen485Device_directCommandData_t cmd;

//    IF_Params params;
    IF_Transaction ifTransaction;
    uint8_t ui8NetworkId;
    bool transferOk;

//    char_data_t *pCharData = (char_data_t *)pMsg->pdu;
    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = 0;
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_NONE;
//    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeBuf = &cmd;
    ifTransaction.writeCount = sizeof(Rosen485Device_directCommandData_t);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;


    ui8NetworkId = ((((uint32_t)arg0) & ROSENRS485DEVICE_NETWORKID_MASK) >> ROSENRS485DEVICE_NETWORKID_SHIFT);


    switch (pCharData->paramID) {
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_UP_ID:
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen_Header_Universal_Lift,
                                       0x01,
                                       ui8NetworkId);
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_DOWN_ID:
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen_Header_Universal_Lift,
                                       0x0e,
                                       ui8NetworkId);
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_STATUS_REQUEST_ID:
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen_Header_Universal_Lift_Status,
                                       0x55,
                                       ui8NetworkId);
        break;
    default:
        break;
    }


    transferOk = bIF_transfer(ifHandle, &ifTransaction);
    if (transferOk) {

    }
}

static void vRosen485Device_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
    uint32_t myDeviceID;
    DeviceList_Handler devHandle;

    Rosen485Device_directCommandData_t cmd;
    Rosen485Device_pingResponse pingResponse;
    Rosen485Device_generalStatusResponse statusResponse;

//    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
    Rosen485Device_SteveCommandData_t *pRxData = (Rosen485Device_SteveCommandData_t *)pCharData->data;

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
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_ROSEN485;
//    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeBuf = &cmd;
    ifTransaction.writeCount = sizeof(Rosen485Device_directCommandData_t);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;



    switch (pCharData->paramID) {
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_SET_ID:
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_Display_Power,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_GET_ID:
        ifTransaction.readCount = sizeof(Rosen485Device_pingResponse);
        ifTransaction.readBuf = &pingResponse;
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_Ping,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            Rosen485Device_SteveCommandReturn1Data_t steveResponse1Data;
            steveResponse1Data.ui8NetworkID = pRxData->address;
            steveResponse1Data.ui8Value = pingResponse.ui1PowerOnOff;
            vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                                     APP_MSG_SERVICE_WRITE,
                                                     pCharData->connHandle,
                                                     pCharData->retSvcUUID, pCharData->retParamID,
                                                     myDeviceID,
                                                     SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_GET_ID,
                                                     (uint8_t *)&steveResponse1Data, sizeof(Rosen485Device_SteveCommandReturn1Data_t));
        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_SET_ID:
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_Display_Input_Source_Selection,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_GET_ID:
        ifTransaction.readCount = sizeof(Rosen485Device_pingResponse);
        ifTransaction.readBuf = &pingResponse;
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_Ping,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            Rosen485Device_SteveCommandReturn1Data_t steveResponse1Data;
            steveResponse1Data.ui8NetworkID = pRxData->address;
            steveResponse1Data.ui8Value = pingResponse.ui4CurrentSource;
            vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                     APP_MSG_SERVICE_WRITE,
                                     pCharData->connHandle,
                                     pCharData->retSvcUUID, pCharData->retParamID,
                                     myDeviceID,
                                     SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_GET_ID,
                                     (uint8_t *)&steveResponse1Data, sizeof(Rosen485Device_SteveCommandReturn1Data_t));
        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_CONTROL_SET_ID:
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_Single_Disc_DVD,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {

        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_POWER_GET_ID:
        ifTransaction.readCount = sizeof(Rosen485Device_generalStatusResponse);
        ifTransaction.readBuf = &statusResponse;
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_DVD_General_Status_Request,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            Rosen485Device_SteveCommandReturn1Data_t steveResponse1Data;
            steveResponse1Data.ui8NetworkID = pRxData->address;
            steveResponse1Data.ui8Value = statusResponse.ui1DiscUnitPowerStatus;
            vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                     APP_MSG_SERVICE_WRITE,
                                     pCharData->connHandle,
                                     pCharData->retSvcUUID, pCharData->retParamID,
                                     myDeviceID,
                                     SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_POWER_GET_ID,
                                     (uint8_t *)&steveResponse1Data, sizeof(Rosen485Device_SteveCommandReturn1Data_t));
        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_LOADED_GET_ID:
        ifTransaction.readCount = sizeof(Rosen485Device_generalStatusResponse);
        ifTransaction.readBuf = &statusResponse;
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_DVD_General_Status_Request,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            Rosen485Device_SteveCommandReturn1Data_t steveResponse1Data;
            steveResponse1Data.ui8NetworkID = pRxData->address;
            steveResponse1Data.ui8Value = statusResponse.ui1DiscLoaded;
            vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                     APP_MSG_SERVICE_WRITE,
                                     pCharData->connHandle,
                                     pCharData->retSvcUUID, pCharData->retParamID,
                                     myDeviceID,
                                     SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_LOADED_GET_ID,
                                     (uint8_t *)&steveResponse1Data, sizeof(Rosen485Device_SteveCommandReturn1Data_t));
        }
        break;
    case CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_PLAYING_GET_ID:
        ifTransaction.readCount = sizeof(Rosen485Device_generalStatusResponse);
        ifTransaction.readBuf = &statusResponse;
        xRosen485Device_createMsgFrame(&cmd,
                                       Rosen485_Header_DVD_General_Status_Request,
                                       pRxData->command,
                                       pRxData->address);
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            Rosen485Device_SteveCommandReturn1Data_t steveResponse1Data;
            steveResponse1Data.ui8NetworkID = pRxData->address;
            steveResponse1Data.ui8Value = statusResponse.ui1DiscPlaying;
            vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                     APP_MSG_SERVICE_WRITE,
                                     pCharData->connHandle,
                                     pCharData->retSvcUUID, pCharData->retParamID,
                                     myDeviceID,
                                     SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_PLAYING_GET_ID,
                                     (uint8_t *)&steveResponse1Data, sizeof(Rosen485Device_SteveCommandReturn1Data_t));
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

uint32_t xRosen485Device_createMsgFrame(Rosen485Device_directCommandData_t *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address)
{
    ASSERT(ptsCmdBuf != NULL);

    if (ptsCmdBuf == NULL) {
        return 0;
    }
    ptsCmdBuf->header = header;
    ptsCmdBuf->command = command;
    ptsCmdBuf->address = address;
    return 3;
}

uint32_t xRosen485Device_SingleBlueRayDVDCreateMsgFrame(Rosen485Device_directCommandData_t *ptsCmdBuf, uint32_t command, uint32_t address)
{
    ASSERT(ptsCmdBuf != NULL);
    ASSERT(command <= 0xFF);
    ASSERT((address >= ROSSEN_BlueRayDVD_Network_Address_20 ) && (address <= ROSSEN_BlueRayDVD_Network_Address_27));

    if (ptsCmdBuf == NULL) {
        return 0;
    }
    if ((address < ROSSEN_BlueRayDVD_Network_Address_20 ) || (address > ROSSEN_BlueRayDVD_Network_Address_27)) {
        return 0;
    }
    return xRosen485Device_createMsgFrame(ptsCmdBuf, Rosen_Header_Single_BlueRay_DVD, command, address);
}

