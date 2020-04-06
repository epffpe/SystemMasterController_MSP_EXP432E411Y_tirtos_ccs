/*
 * RosenRS485.c
 *
 *  Created on: Apr 6, 2020
 *      Author: epf
 */


#define __DEVICES_ROSEN_ROSENRS485DEVICE_GLOBAL
#include "includes.h"



static void vRosen485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1);
static void vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);
static void vRosen485Device_CommandsClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);


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

//    Task_Handle taskTCPServerHandle;
//    Task_Params taskParams;
    Error_Block eb;

    // Open the file session
    fdOpenSession((void *)Task_self());

    ASSERT(arg1 != NULL);


    if (arg1 == NULL) {
        return;
    }
    devHandle = (DeviceList_Handler)arg1;
    eventHandle = devHandle->eventHandle;
    msgQHandle = devHandle->msgQHandle;
    clockHandle = devHandle->clockHandle;
    myDeviceID = devHandle->deviceID;

    Display_printf(g_SMCDisplay, 0, 0, "Rosen DeviceId (%d) started\n", myDeviceID);

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;
            vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(NULL, NULL, NULL);
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

            // Close the file session
            fdCloseSession((void *)Task_self());
            Task_exit();
        }

        // TODO: Implement messages
        // Process messages sent from another task or another context.
        while (!Queue_empty(msgQHandle))
        {
            pMsg = Queue_dequeue(msgQHandle);

            vRosen485Device_processApplicationMessage(pMsg, arg0, arg1);

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
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vRosen485Device_taskFxn, &paramsUnion.taskParams, &eb);

    return handle;
}









static void vRosen485Device_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_ROSEN_ALTO_EMULATOR_UUID:
          switch (pCharData->paramID) {
          case CHARACTERISTIC_SERVICE_ROSEN_ALTO_EMULATOR_DIRECT_COMMAND_ID:
              vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(pCharData, arg0, arg1);
              break;
          default:
              break;
          }
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

static void vRosen485Device_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1)
{
    tsROSENCommand cmd;



    IF_Handle ifHandle;
    IF_Params params;
    IF_Transaction ifTransaction;
    bool transferOk;
    ALTO_Frame_t tFrameTx, tFrameRx;
    char rxbuff[70];
    char txbuff[70];

    vIF_Params_init(&params, IF_Params_Type_Uart);
    params.uartParams.writeDataMode = UART_DATA_BINARY;
    params.uartParams.readDataMode = UART_DATA_BINARY;
    params.uartParams.readReturnMode = UART_RETURN_FULL;
    params.uartParams.readEcho = UART_ECHO_OFF;
    params.uartParams.baudRate = 9600;
//    ifHandle = hIF_open(IF_SERIAL_4, &params);
    ifHandle = hIF_open(IF_SERIAL_1, &params);


    /*
     * Prepare the transfer
     */
    memset(&ifTransaction, 0, sizeof(IF_Transaction));
    ifTransaction.readCount = sizeof(rxbuff);
    ifTransaction.readBuf = rxbuff;
    ifTransaction.readTimeout = 30;
    ifTransaction.transactionRxProtocol = IF_TRANSACTION_RX_PROTOCOL_NONE;
//    ifTransaction.writeBuf = txbuff;
    ifTransaction.writeBuf = &cmd;
    ifTransaction.writeCount = sizeof(tsROSENCommand);
    ifTransaction.writeTimeout = BIOS_WAIT_FOREVER;
    ifTransaction.transferType = IF_TRANSFER_TYPE_NONE;



    xROSENBlueRayDVDCreateMsgFrame(&cmd,
                                   ROSENSingleBlueRayDVDCommand_Menu_Up,
                                   ROSSEN_BlueRayDVD_Network_Address_20);
    ifTransaction.readCount = 0;
    ifTransaction.writeCount = sizeof(tsROSENCommand);

    transferOk = bIF_transfer(ifHandle, &ifTransaction);
    if (transferOk) {

    }
}


