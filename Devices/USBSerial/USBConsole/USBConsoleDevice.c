/*
 * USBConsoleDevice.c
 *
 *  Created on: Jun 5, 2020
 *      Author: epffpe
 */

#define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLEDEVICE_GLOBAL
#include "includes.h"


void vUSBConsoleDevice_close(DeviceList_Handler handle);
DeviceList_Handler hUSBConsoleDevice_open(DeviceList_Handler handle, void *params);
static void vUSBConsoleDevice_processApplicationMessage(device_msg_t *pMsg, uint32_t myDeviceID);

static void vUSBConsoleDevice_ALTOAmplifierClassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID);


const Device_FxnTable g_USBConsoleDevice_fxnTable =
{
 .closeFxn = vUSBConsoleDevice_close,
 .openFxn = hUSBConsoleDevice_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};

Void vUSBConsoleDeviceFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
    Event_Handle eventHandle;
    Queue_Handle msgQHandle;
    Clock_Handle clockHandle;
    device_msg_t *pMsg;

    Task_Handle taskUSBConsoleDeviceWorkerHandle;
    Task_Params taskParams;
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

    Display_printf(g_SMCDisplay, 0, 0, "USB Console DeviceId: (%d) Started\n", myDeviceID);

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = USBCONSOLEWORKER_TASK_STACK_SIZE;
    taskParams.priority = USBCONSOLEWORKER_TASK_PRIORITY;
    taskUSBConsoleDeviceWorkerHandle = Task_create((Task_FuncPtr)vUSBConsoleWorkerFxn, &taskParams, &eb);
    if (taskUSBConsoleDeviceWorkerHandle == NULL) {
        System_printf("Failed to create vUSBConsoleDeviceWorkerFxn Task\n");
    }

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;

//            char *text = "SMC controls USB Console.\r\n";
//            /* Block while the device is NOT connected to the USB */
//            USBCDCD_waitForConnect(USBCDCD_Console, WAIT_FOREVER);
//
//            USBCDCD_sendData(USBCDCD_Console, text, strlen(text)+1, WAIT_FOREVER);
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
            vUSBConsoleDevice_processApplicationMessage(pMsg, myDeviceID);

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
static void vUSBConsoleDevice_processApplicationMessage(device_msg_t *pMsg, uint32_t myDeviceID)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_ALTO_AMP_INPUTSELECT_UUID:
          vUSBConsoleDevice_ALTOAmplifierClassService_ValueChangeHandler(pCharData, myDeviceID);
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


static void vUSBConsoleDevice_ALTOAmplifierClassService_ValueChangeHandler(char_data_t *pCharData, uint32_t myDeviceID)
{
    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_AMP_GET_ID:
//        tFrameTx.operationID = ALTO_Operation_Get;
//        tFrameTx.length = 0x00;
//
//        vALTOFrame_BCC_fill(&tFrameTx);
//        vALTOFrame_create_ASCII(txbuff, &tFrameTx);
//
//        ifTransaction.readCount = 64;
//        ifTransaction.writeCount = 70;
//        transferOk = bIF_transfer(ifHandle, &ifTransaction);
//        if (transferOk) {
//            int8_t i8bcc;
//            xALTOFrame_convert_ASCII_to_binary(rxbuff, &tFrameRx);
//            i8bcc = cALTOFrame_BCC_check(&tFrameRx);
//            if (!i8bcc) {
//                ALTOAmp_volumeData_t *pVolumeData = (ALTOAmp_volumeData_t *)tFrameRx.data;
//                vDevice_sendCharDataMsg (pCharData->retDeviceID,
//                                         APP_MSG_SERVICE_WRITE,
//                                         pCharData->connHandle,
//                                         pCharData->retSvcUUID, pCharData->retParamID,
//                                         myDeviceID,
//                                         g_USBConsoleDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
//                                         (uint8_t *)pVolumeData, sizeof(ALTOAmp_volumeData_t));
//            }else{
//                xDevice_sendErrorMsg (pCharData->retDeviceID,
//                                      pCharData->connHandle,
//                                      pCharData->retSvcUUID, pCharData->retParamID,
//                                      myDeviceID,
//                                      g_USBConsoleDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
//                                      APP_MSG_ERROR_CRC);
//            }
//        }else {
//            xDevice_sendErrorMsg (pCharData->retDeviceID,
//                                  pCharData->connHandle,
//                                  pCharData->retSvcUUID, pCharData->retParamID,
//                                  myDeviceID,
//                                  g_USBConsoleDevice_getVolumeRetrunUUIDMapTable[functionID], CHARACTERISTIC_ALTO_AMP_GET_ID,
//                                  APP_MSG_ERROR_TRANSFER_FAILED);
//        }
        break;
    default:
        break;
    }
}



/*************************************/

void vUSBConsoleDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vUSBConsoleDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_USB_REMOTE_CONTROL_CONSOLE;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_USBConsoleDevice_fxnTable;
}


void vUSBConsoleDevice_close(DeviceList_Handler handle)
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

DeviceList_Handler hUSBConsoleDevice_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_USB_REMOTE_CONTROL_CONSOLE;

//    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


    Clock_Params_init(&paramsUnion.clockParams);
    paramsUnion.clockParams.period = USBCONSOLEDEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vUSBConsoleDevice_clockHandler, USBCONSOLEDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Display_printf(g_SMCDisplay, 0, 0, "Opening USB Console DeviceId: (%d) \n", handle->deviceID);

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = USBCONSOLEDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = USBCONSOLEDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)deviceParams->arg0;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vUSBConsoleDeviceFxn, &paramsUnion.taskParams, &eb);

    return handle;
}




