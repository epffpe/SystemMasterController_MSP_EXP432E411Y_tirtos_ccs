/*
 * TCPRemoteControllerBinary.c
 *
 *  Created on: May 4, 2018
 *      Author: epenate
 */

#define __DEVICES_TCPRCBINDEVICE_ALTOMULTINETDEVICE_GLOBAL
#include "includes.h"
#undef htonl
#undef htons
#undef ntohl
#undef ntohs

/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <ti/net/slnetutils.h>

#ifndef MAXSOCKETS
/* Must Match with sockets in ndk_tirtos.c */
#define MAXSOCKETS 15
#endif

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

Void vTCPRCBinServerFxn(UArg arg0, UArg arg1);
Void vTCPRCBinWorker(UArg arg0, UArg arg1);
Void vTCPRCBinWorkerSocketTimeout(UArg arg0, UArg arg1);

void vTCPRCBinDevice_close(DeviceList_Handler handle);
DeviceList_Handler hTCPRCBinDevice_open(DeviceList_Handler handle, void *params);
static void vTCPRCBin_processApplicationMessage(device_msg_t *pMsg);



const Device_FxnTable g_TCPRCBinDevice_fxnTable =
{
 .closeFxn = vTCPRCBinDevice_close,
 .openFxn = hTCPRCBinDevice_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};




Void vTCPRCBinDeviceFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
    Event_Handle eventHandle;
    Queue_Handle msgQHandle;
    Clock_Handle clockHandle;
    device_msg_t *pMsg;

    Task_Handle taskTCPServerHandle;
    Task_Params taskParams;
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

    Display_printf(g_SMCDisplay, 0, 0, "TCP Server DeviceId (%d) started\n", myDeviceID);

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TCPBIN_SERVER_HANDLER_STACK;
    taskParams.priority = TCPBIN_SERVER_TASK_PRIORITY;
    taskParams.arg0 = TCPBIN_SERVER_PORT;
    taskTCPServerHandle = Task_create((Task_FuncPtr)vTCPRCBinServerFxn, &taskParams, &eb);
    if (taskTCPServerHandle == NULL) {
        System_printf("netOpenHook: Failed to create tcpHandler Task\n");
    }

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;

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
            Task_delete(&taskTCPServerHandle);

            // Close the file session
            fdCloseSession((void *)Task_self());
            Task_exit();
        }

        // TODO: Implement messages
        // Process messages sent from another task or another context.
        while (!Queue_empty(msgQHandle))
        {
            pMsg = Queue_dequeue(msgQHandle);

            // Process application-layer message probably sent from ourselves.
            vTCPRCBin_processApplicationMessage(pMsg);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }


}

Void vTCPRCBinServerFxn(UArg arg0, UArg arg1)
{
    // TCP variables
    uint32_t           errorCounter;
    int                status;
    int                clientfd;
    int                server;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    int                optval;
    int                optlen = sizeof(optval);
    socklen_t          addrlen = sizeof(clientAddr);
    Task_Handle        taskHandle;
    Task_Params        taskParams;
    Semaphore_Params    semParams;
    Semaphore_Handle    semHandle;

    Clock_Params       clockParams;
    Clock_Handle       clockHandle;
    Error_Block        eb;


    ASSERT(arg0 != NULL);

    Error_init(&eb);
    // Open the file session
    fdOpenSession((void *)Task_self());


    if (arg0 == NULL) {
        return;
    }

    Display_printf(g_SMCDisplay, 0, 0, "TCP Server started on Port (%d)\n", (uint32_t)arg0);

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        System_printf("Error: socket not created.\n");
        System_flush();
        Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinServerFxn: socket failed\n");
        goto shutdown;
    }


    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(arg0);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        System_printf("Error: bind failed.\n");
        System_flush();
        Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinServerFxn: bind failed\n");
        goto shutdown;
    }

    status = listen(server, TCPBINDEVICE_NUMTCPWORKERS);
    if (status == -1) {
        System_printf("Error: listen failed.\n");
        System_flush();
        Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinServerFxn: listen failed\n");
        goto shutdown;
    }

    optval = 1;
    status = setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    if (status == -1) {
//    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        System_printf("Error: setsockopt failed\n");
        System_flush();
        Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinServerFxn: setsockopt failed\n");
        goto shutdown;
    }

    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_COUNTING;
    semHandle = Semaphore_create(MAXSOCKETS - 1, &semParams, &eb);
    if (semHandle == NULL) {
        System_abort("Can't create Socket limit semaphore");
    }

    Clock_Params_init(&clockParams);
    clockParams.period = TCPRCBINDEVICE_SOCKET_PERIOD;
    clockParams.startFlag = TRUE;

    System_printf("vTCPRCBinServerFxn: Semaphore count = %d\n", Semaphore_getCount(semHandle));
    System_flush();

    errorCounter = 0;
//    while ((clientfd =
//            accept(server, (struct sockaddr *)&clientAddr, &addrlen)) != -1) {
    do {
        clientfd = accept(server, (struct sockaddr *)&clientAddr, &addrlen);

        if ( clientfd != -1) {
            errorCounter = 0;
            System_printf("vTCPRCBinServerFxn: Creating thread clientfd = %d\n", clientfd);
            System_printf("vTCPRCBinServerFxn: Semaphore count = %d\n", Semaphore_getCount(semHandle));
            System_flush();
            Display_printf(g_SMCDisplay, 0, 0,
                           "vTCPRCBinServerFxn: Creating thread clientfd = %d\n", clientfd);
            /* Init the Error_Block */
            Error_init(&eb);



            /* Initialize the defaults and set the parameters. */
            Task_Params_init(&taskParams);
            taskParams.arg0 = (UArg)clientfd;
            taskParams.arg1 = (UArg)semHandle;
            taskParams.stackSize = TCPBIN_WORKER_HANDLER_STACK;
            taskParams.priority = TCPBIN_WORKER_TASK_PRIORITY;
            taskHandle = Task_create((Task_FuncPtr)vTCPRCBinWorker, &taskParams, &eb);
            if (taskHandle == NULL) {
                System_printf("Error: Failed to create new Task\n");
                System_flush();
                close(clientfd);
            }

            /* addrlen is a value-result param, must reset for next accept call */
            addrlen = sizeof(clientAddr);

            Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);

            System_printf("vTCPRCBinServerFxn: Semaphore Grabbed. Left = %d\n", Semaphore_getCount(semHandle));
            System_flush();
        }else {
            errorCounter++;
            System_printf("Error: accept failed.\n");
            System_flush();
            Task_sleep((unsigned int)1000);
        }
//    }
    }while(errorCounter < 100);

    System_printf("Error: accept failed. Closing TCP Server\n");
    System_flush();
    Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinServerFxn: accept failed.\n");
    shutdown:
    if (server != -1) {
        close(server);
    }
    // Close the file session
    fdCloseSession((void *)Task_self());
}




/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
Void vTCPRCBinWorker(UArg arg0, UArg arg1)
{
    int  clientfd = (int)arg0;
    int  bytesRcvd;
    char buffer[TCPBINDEVICE_PACKETSIZE];
    Task_Params taskParams;
    Task_Handle taskHandle;
    Event_Handle eventHandle;
    Semaphore_Handle    semHandle;


    ASSERT(arg1 != NULL);


    Error_Block eb;
    Error_init(&eb);



    System_printf("vTCPRCBinWorker: start clientfd = 0x%x\n", clientfd);
    System_flush();
    Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinWorker: start clientfd = 0x%x\n", clientfd);

    // Open the file session
    fdOpenSession((void *)Task_self());


    if (arg1 == NULL) {
        close(clientfd);
        // Close the file session
        fdCloseSession((void *)Task_self());
        Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinWorker: semaphore handler invalid\n");

        return;
    }


    eventHandle = Event_create(NULL, &eb);
    Task_Params_init(&taskParams);
    taskParams.stackSize = 512;
    taskParams.priority = TCPBIN_WORKER_TASK_PRIORITY;
    taskParams.arg0 = arg0;
    taskParams.arg1 = (UArg)eventHandle;
    taskHandle = Task_create((Task_FuncPtr)vTCPRCBinWorkerSocketTimeout, &taskParams, &eb);

    semHandle = (Semaphore_Handle)arg1;


    while((bytesRcvd = recv(clientfd, buffer, TCPBINDEVICE_PACKETSIZE, 0)) > 0) {
        Event_post(eventHandle, DEVICE_PERIODIC_EVT);
        TCPBin_decode(clientfd, buffer, bytesRcvd);
    }
//    Task_delete(&taskHandle);

    System_printf("vTCPRCBinWorker stop clientfd = 0x%x\n", clientfd);
    System_flush();
    Display_printf(g_SMCDisplay, 0, 0, "vTCPRCBinWorker stop clientfd = 0x%x\n", clientfd);

    Event_post(eventHandle, DEVICE_APP_KILL_EVT);


    close(clientfd);

    // Close the file session
    fdCloseSession((void *)Task_self());

    Semaphore_post(semHandle);
    System_printf("vTCPRCBinWorker: Semaphore Released. clientfd = 0x%x, count %d\n", clientfd, Semaphore_getCount(semHandle));
    System_flush();
}

Void vTCPRCBinWorkerSocketTimeout(UArg arg0, UArg arg1)
{
    int  clientfd = (int)arg0;
    uint32_t events;
    Event_Handle eventHandle = (Event_Handle)arg1;

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, TCPRCBINDEVICE_SOCKET_PERIOD); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;
        }else {
            break;
        }
    }
    System_printf("vTCPRCBinWorkerSocketTimeoutFxn stop clientfd = 0x%x\n", clientfd);
    System_flush();
    // Open the file session
    fdOpenSession((void *)Task_self());
    close(clientfd);
    // Close the file session
    fdCloseSession((void *)Task_self());
    Event_delete(&eventHandle);
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
static void vTCPRCBin_processApplicationMessage(device_msg_t *pMsg)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type) {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
  {
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID:
          vTCPRCBin_ALTOVolumeReturnService_ValueChangeHandler(pCharData);
          break;
      case SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID:
          vTCPRCBin_ALTOMultinetGetDetailedStatusReturnService_ValueChangeHandler(pCharData);
          break;
      case SERVICE_TCPBIN_REMOTECONTROL_RAWCHARACTERISTICDATA_CLASS_RETURN_UUID:
          vTCPRCBin_RAWCharacteristicData_returnMsg(pCharData);
          break;
      case SERVICE_TCPBIN_REMOTECONTROL_AVDS_CLASS_RETURN_UUID:
          vTCPRCBin_AVDSService_ValueChangeHandler(pCharData);
          break;
      case SERVICE_TCPBIN_REMOTECONTROL_AVDS_SERIAL_CLASS_RETURN_UUID:
          vTCPRCBin_AVDSSerialService_ValueChangeHandler(pCharData);
          break;
      case SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID:
          vTCPRCBin_ROSENService_ValueChangeHandler(pCharData);
          break;
      }
      break;
  }
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

  case APP_MSG_ERROR_HANDLER: /*  */
      switch(pCharData->svcUUID) {
      case SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID:
      case SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID:
          vTCPRCBin_ALTOAmplifier_errorHandler(pCharData);
          break;
      }
      break;

  }
}


void vTCPRCBinDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vTCPRCBinDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_TCP_REMOTE_CONTROLLER_BINARY;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_TCPRCBinDevice_fxnTable;
}


void vTCPRCBinDevice_close(DeviceList_Handler handle)
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

DeviceList_Handler hTCPRCBinDevice_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_TCP_REMOTE_CONTROLLER_BINARY;


//    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


//    Clock_Params_init(&paramsUnion.clockParams);
//    paramsUnion.clockParams.period = TCPRCBinDEVICE_CLOCK_PERIOD;
//    paramsUnion.clockParams.startFlag = TRUE;
//    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
//    handle->clockHandle = Clock_create(vTCPRCBinDevice_clockHandler, TCPRCBinDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
//    if (handle->clockHandle == NULL) {
//        System_abort("Clock create failed");
//    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = TCPRCBINDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = TCPRCBINDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vTCPRCBinDeviceFxn, &paramsUnion.taskParams, &eb);

    return handle;
}




