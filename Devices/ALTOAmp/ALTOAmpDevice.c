/*
 * ALTOAmpDevice.c
 *
 *  Created on: Apr 27, 2018
 *      Author: epenate
 */

#define __DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_GLOBAL
#include "includes.h"

void vALTOAmpDevice_close(DeviceList_Handler handle);
DeviceList_Handler hALTOAmpDevice_open(DeviceList_Handler handle, void *params);
static void vALTOAmp_processApplicationMessage(device_msg_t *pMsg, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff);

static void vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID);
static void vALTOAmp_ALTOHPClassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID);
//static void vALTOAmp_ALTOVolumeService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff);
//static void vALTOAmp_ALTOBassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff);

const Device_FxnTable g_ALTOAmpDevice_fxnTable =
{
 .closeFxn = vALTOAmpDevice_close,
 .openFxn = hALTOAmpDevice_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};

const uint16_t g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[] =
{
 [ALTO_Function_InputSelect] = SERVICE_ALTO_AMP_INPUTSELECT_UUID,
 [ALTO_Function_Volume] = SERVICE_ALTO_AMP_VOLUME_UUID,
 [ALTO_Function_Bass] = SERVICE_ALTO_AMP_BASS_UUID,
 [ALTO_Function_Treble] = SERVICE_ALTO_AMP_TREBLE_UUID,
 [ALTO_Function_Mute] = SERVICE_ALTO_AMP_MUTE_UUID,
 [ALTO_Function_Compressor] = SERVICE_ALTO_AMP_COMPRESSOR_UUID,
 [ALTO_Function_Loudness] = SERVICE_ALTO_AMP_LOUDNESS_UUID,
};

const uint16_t g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[] =
{
 [ALTO_Function_InputSelect] = SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID,
 [ALTO_Function_Volume] = SERVICE_ALTO_AMP_HP_VOLUME_UUID,
 [ALTO_Function_Mute] = SERVICE_ALTO_AMP_HP_MUTE_UUID,
};

Void vALTOAmpDeviceFxn(UArg arg0, UArg arg1)
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
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;
    char rxbuff[70];
    char txbuff[70];


    ASSERT(arg1 != NULL);

    if (arg1 == NULL) {
        return;
    }
    devHandle = (DeviceList_Handler)arg1;
    eventHandle = devHandle->eventHandle;
    msgQHandle = devHandle->msgQHandle;
    clockHandle = devHandle->clockHandle;
    myDeviceID = devHandle->deviceID;

    Display_printf(g_SMCDisplay, 0, 0, "ALTO Amplifier Device (%d) Started\n", myDeviceID);
    /*
     * Initialize interface
     */
    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = (unsigned int)115200;
//    ifHandle = hIF_open(IF_SERIAL_4, &params);
    ifHandle = hIF_open((uint32_t)arg0, &params);

    /*
     * Fill the tx frame
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Diagnostic;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.functionID = ALTO_Function_ManufacturingInformation;
    tFrameTx.length = 0x00;
    tFrameTx.uin4unitType = ALTO_ALTONet_Amp;
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
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;

            char i8address;

            tFrameTx.uin4unitType = ALTO_ALTONet_Amp;
//            for (i8address = 0; i8address < ALTOMULTINET_NUM_RELAYS_DEVICES; i8address++) {
//            }

            i8address = 0;
            tFrameTx.ui4address = i8address & 0x1F;
            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            ifTransaction.readCount = 64;
            ifTransaction.writeCount = sizeof(txbuff);
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {
                int8_t i8bcc;
                xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    i8bcc = i8bcc;
                }
            }


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

            // Process application-layer message probably sent from ourselves.
            vALTOAmp_processApplicationMessage(pMsg, myDeviceID, ifHandle, txbuff, rxbuff);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
//            Task_sleep(3000);
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
static void vALTOAmp_processApplicationMessage(device_msg_t *pMsg, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_ALTO_AMP_INPUTSELECT_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_InputSelect);
          break;
      case SERVICE_ALTO_AMP_VOLUME_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Volume);
          //          vALTOAmp_ALTOVolumeService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff);
          break;
      case SERVICE_ALTO_AMP_BASS_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Bass);
          //          vALTOAmp_ALTOBassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff);
          break;
      case SERVICE_ALTO_AMP_TREBLE_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Treble);
          break;
      case SERVICE_ALTO_AMP_MUTE_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Mute);
          break;
      case SERVICE_ALTO_AMP_COMPRESSOR_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Compressor);
          break;
      case SERVICE_ALTO_AMP_LOUDNESS_UUID:
          vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Loudness);
          break;
      case SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID:
          vALTOAmp_ALTOHPClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_InputSelect);
          break;
      case SERVICE_ALTO_AMP_HP_VOLUME_UUID:
          vALTOAmp_ALTOHPClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Volume);
          break;
      case SERVICE_ALTO_AMP_HP_MUTE_UUID:
          vALTOAmp_ALTOHPClassService_ValueChangeHandler(pCharData, myDeviceID, ifHandle, txbuff, rxbuff, ALTO_Function_Mute);
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


static void vALTOAmp_ALTOAmplifierClassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID)
{
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;

    /*
     * Fill the tx frame
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Amplifier;
    tFrameTx.functionID = functionID;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.length = 0x00;
    tFrameTx.uin4unitType = ALTO_ALTONet_Amp;
    tFrameTx.ui4address = 0 & 0x1F;
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


    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_AMP_GET_ID:
        tFrameTx.operationID = ALTO_Operation_Get;
        tFrameTx.length = 0x00;

        vALTOFrame_BCC_fill(&tFrameTx);
        vALTOFrame_create_ASCII(txbuff, &tFrameTx);

        ifTransaction.readCount = 64;
        ifTransaction.writeCount = 70;
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            int8_t i8bcc;
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                ALTOAmp_volumeData_t *pVolumeData = (ALTOAmp_volumeData_t *)tFrameRx.data;
                vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                         APP_MSG_SERVICE_WRITE,
                                         pCharData->connHandle,
                                         pCharData->retSvcUUID, pCharData->retParamID,
                                         myDeviceID,
                                         g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
                                         (uint8_t *)pVolumeData, sizeof(ALTOAmp_volumeData_t));
            }else{
                xDevice_sendErrorMsg (pCharData->retDeviceID,
                                      pCharData->connHandle,
                                      pCharData->retSvcUUID, pCharData->retParamID,
                                      myDeviceID,
                                      g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
                                      APP_MSG_ERROR_CRC);
            }
        }else {
            xDevice_sendErrorMsg (pCharData->retDeviceID,
                                  pCharData->connHandle,
                                  pCharData->retSvcUUID, pCharData->retParamID,
                                  myDeviceID,
                                  g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
                                  APP_MSG_ERROR_TRANSFER_FAILED);
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_SET_ID:
        if (pCharData->dataLen == sizeof(ALTOAmp_zoneVolumeData_t)) {
            ALTOAmp_zoneVolumeData_t *pVolumeData = (ALTOAmp_zoneVolumeData_t *)pCharData->data;
            tFrameTx.operationID = ALTO_Operation_Set;
            tFrameTx.length = 0x03;
            tFrameTx.data[0] = pVolumeData->zone;
            tFrameTx.data[1] = pVolumeData->zone1Volume;
            tFrameTx.data[2] = pVolumeData->zone2Volume;

            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            ifTransaction.readCount = 64;
            ifTransaction.writeCount = 70;
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {
                int8_t i8bcc;
                xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                             APP_MSG_SERVICE_WRITE,
                                             pCharData->connHandle,
                                             pCharData->retSvcUUID, pCharData->retParamID,
                                             myDeviceID,
                                             g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_SET_ID,
                                             (uint8_t *)tFrameRx.data, sizeof(char));
                }else{
                    xDevice_sendErrorMsg (pCharData->retDeviceID,
                                          pCharData->connHandle,
                                          pCharData->retSvcUUID, pCharData->retParamID,
                                          myDeviceID,
                                          g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_SET_ID,
                                          APP_MSG_ERROR_CRC);
                }
            }else {
                xDevice_sendErrorMsg (pCharData->retDeviceID,
                                      pCharData->connHandle,
                                      pCharData->retSvcUUID, pCharData->retParamID,
                                      myDeviceID,
                                      g_ALTOAmpDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_SET_ID,
                                      APP_MSG_ERROR_TRANSFER_FAILED);
            }
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_INC_ID:
        tFrameTx.operationID = ALTO_Operation_Volume_Inc;
        tFrameTx.length = 0x01;
        tFrameTx.data[0] = 0x02;

        vALTOFrame_BCC_fill(&tFrameTx);
        vALTOFrame_create_ASCII(txbuff, &tFrameTx);

        ifTransaction.readCount = 64;
        ifTransaction.writeCount = 70;
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            int8_t i8bcc;
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                i8bcc = i8bcc;
            }
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_DEC_ID:
        tFrameTx.operationID = ALTO_Operation_Volume_Dec;
        tFrameTx.length = 0x01;
        tFrameTx.data[0] = 0x01;

        vALTOFrame_BCC_fill(&tFrameTx);
        vALTOFrame_create_ASCII(txbuff, &tFrameTx);

        ifTransaction.readCount = 64;
        ifTransaction.writeCount = 70;
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            int8_t i8bcc;
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                i8bcc = i8bcc;
            }
        }
        break;
    default:
        break;
    }
}


static void vALTOAmp_ALTOHPClassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID, IF_Handle ifHandle, char *txbuff, char *rxbuff, uint8_t functionID)
{
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;

    /*
     * Fill the tx frame
     */
    vALTOFrame_Params_init(&tFrameTx);
    tFrameTx.classID = ALTO_Class_Headphone;
    tFrameTx.functionID = functionID;
    tFrameTx.operationID = ALTO_Operation_Get;
    tFrameTx.length = 0x00;
    tFrameTx.uin4unitType = ALTO_ALTONet_Amp;
    tFrameTx.ui4address = 0 & 0x1F;
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


    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_AMP_GET_ID:
        if (pCharData->dataLen == sizeof(uint8_t)) {
            uint8_t *pHPNum = (uint8_t *)pCharData->data;
            tFrameTx.operationID = ALTO_Operation_Get;
            tFrameTx.length = sizeof(uint8_t);
            tFrameTx.data[0] = *pHPNum;

            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            ifTransaction.readCount = 64;
            ifTransaction.writeCount = 70;
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {
                int8_t i8bcc;
                xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    ALTOAmp_headphoneData_t *pHPData = (ALTOAmp_headphoneData_t *)tFrameRx.data;
                    vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                             APP_MSG_SERVICE_WRITE,
                                             pCharData->connHandle,
                                             pCharData->retSvcUUID, pCharData->retParamID,
                                             myDeviceID,
                                             g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
                                             (uint8_t *)pHPData, sizeof(ALTOAmp_headphoneData_t) );
                }else{
                    xDevice_sendErrorMsg (pCharData->retDeviceID,
                                          pCharData->connHandle,
                                          pCharData->retSvcUUID, pCharData->retParamID,
                                          myDeviceID,
                                          g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
                                          APP_MSG_ERROR_CRC);
                }
            }else {
                xDevice_sendErrorMsg (pCharData->retDeviceID,
                                      pCharData->connHandle,
                                      pCharData->retSvcUUID, pCharData->retParamID,
                                      myDeviceID,
                                      g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
                                      APP_MSG_ERROR_TRANSFER_FAILED);
            }
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_SET_ID:
        if (pCharData->dataLen == sizeof(ALTOAmp_headphoneData_t)) {
            ALTOAmp_headphoneData_t *pHPData = (ALTOAmp_headphoneData_t *)pCharData->data;
            tFrameTx.operationID = ALTO_Operation_Set;
            tFrameTx.length = sizeof(ALTOAmp_headphoneData_t);
            tFrameTx.data[0] = pHPData->hpNum;
            tFrameTx.data[1] = pHPData->value;

            vALTOFrame_BCC_fill(&tFrameTx);
            vALTOFrame_create_ASCII(txbuff, &tFrameTx);

            ifTransaction.readCount = 64;
            ifTransaction.writeCount = 70;
            transferOk = bIF_transfer(ifHandle, &ifTransaction);
            if (transferOk) {
                int8_t i8bcc;
                xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
                i8bcc = cALTOFrame_BCC_check(&tFrameRx);
                if (!i8bcc) {
                    vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                             APP_MSG_SERVICE_WRITE,
                                             pCharData->connHandle,
                                             pCharData->retSvcUUID, pCharData->retParamID,
                                             myDeviceID,
                                             g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_SET_ID,
                                             (uint8_t *)tFrameRx.data, sizeof(char));
                }else{
                    xDevice_sendErrorMsg (pCharData->retDeviceID,
                                          pCharData->connHandle,
                                          pCharData->retSvcUUID, pCharData->retParamID,
                                          myDeviceID,
                                          g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_SET_ID,
                                          APP_MSG_ERROR_CRC);
                }
            }else {
                xDevice_sendErrorMsg (pCharData->retDeviceID,
                                      pCharData->connHandle,
                                      pCharData->retSvcUUID, pCharData->retParamID,
                                      myDeviceID,
                                      g_ALTOAmpDevice_getHeadPhoneRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_SET_ID,
                                      APP_MSG_ERROR_TRANSFER_FAILED);
            }
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_INC_ID:
        tFrameTx.operationID = ALTO_Operation_Volume_Inc;
        tFrameTx.length = 0x01;
        tFrameTx.data[0] = 0x02;

        vALTOFrame_BCC_fill(&tFrameTx);
        vALTOFrame_create_ASCII(txbuff, &tFrameTx);

        ifTransaction.readCount = 64;
        ifTransaction.writeCount = 70;
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            int8_t i8bcc;
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                i8bcc = i8bcc;
            }
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_DEC_ID:
        tFrameTx.operationID = ALTO_Operation_Volume_Dec;
        tFrameTx.length = 0x01;
        tFrameTx.data[0] = 0x01;

        vALTOFrame_BCC_fill(&tFrameTx);
        vALTOFrame_create_ASCII(txbuff, &tFrameTx);

        ifTransaction.readCount = 64;
        ifTransaction.writeCount = 70;
        transferOk = bIF_transfer(ifHandle, &ifTransaction);
        if (transferOk) {
            int8_t i8bcc;
            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
            if (!i8bcc) {
                i8bcc = i8bcc;
            }
        }
        break;
    default:
        break;
    }
}

void vALTOAmpDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vALTOAmpDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_ALTO_AMP;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_ALTOAmpDevice_fxnTable;
}


void vALTOAmpDevice_close(DeviceList_Handler handle)
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

DeviceList_Handler hALTOAmpDevice_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_ALTO_AMP;

//    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


    Clock_Params_init(&paramsUnion.clockParams);
    paramsUnion.clockParams.period = ALTOAMPDEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vALTOAmpDevice_clockHandler, ALTOAMPDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Display_printf(g_SMCDisplay, 0, 0, "Opening ALTO Amplifier Device (%d) \n", handle->deviceID);

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = ALTOAMPDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = ALTOAMPDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)deviceParams->arg0;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vALTOAmpDeviceFxn, &paramsUnion.taskParams, &eb);

    return handle;
}




