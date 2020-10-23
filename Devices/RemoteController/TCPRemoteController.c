/*
 * TCPRemoteController.c
 *
 *  Created on: May 2, 2018
 *      Author: epenate
 */


#define __DEVICES_TCPRCDEVICE_ALTOMULTINETDEVICE_GLOBAL
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

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

const unsigned char g_pucTCPCONSOLEWelcome[] = "\r\n>>>>>>> Welcome to the System Master Controller <<<<<<<\r\n";

const unsigned char g_pcTCPCONSOLEprompt[] = "\r" ANSI_COLOR_GREEN "epenate@ALTOAviation: > " ANSI_COLOR_RESET;



Void vTCPRCServerFxn(UArg arg0, UArg arg1);
Void vTCPRCWorker(UArg arg0, UArg arg1);

void vTCPRCDevice_close(DeviceList_Handler handle);
DeviceList_Handler hTCPRCDevice_open(DeviceList_Handler handle, void *params);
static void vTCPRC_processApplicationMessage(device_msg_t *pMsg);

void vTCPRC_ALTOVolumeReturnService_ValueChangeHandler(char_data_t *pCharData);


const Device_FxnTable g_TCPRCDevice_fxnTable =
{
 .closeFxn = vTCPRCDevice_close,
 .openFxn = hTCPRCDevice_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};




Void vTCPRCDeviceFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
//    uint32_t myDeviceID;
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
//    myDeviceID = devHandle->deviceID;



    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TCP_SERVER_HANDLER_STACK;
    taskParams.priority = TCP_SERVER_TASK_PRIORITY;
    taskParams.arg0 = TCP_SERVER_PORT;
    taskTCPServerHandle = Task_create((Task_FuncPtr)vTCPRCServerFxn, &taskParams, &eb);
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
            vTCPRC_processApplicationMessage(pMsg);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }


}

Void vTCPRCServerFxn(UArg arg0, UArg arg1)
{
    // TCP variables
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
    Error_Block        eb;


    ASSERT(arg0 != NULL);

    // Open the file session
    fdOpenSession((void *)Task_self());


    if (arg0 == NULL) {
        return;
    }


    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        System_printf("Error: socket not created.\n");
        System_flush();
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
        goto shutdown;
    }

    status = listen(server, TCPDEVICE_NUMTCPWORKERS);
    if (status == -1) {
        System_printf("Error: listen failed.\n");
        System_flush();
        goto shutdown;
    }

    optval = 1;
    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        System_printf("Error: setsockopt failed\n");
        System_flush();
        goto shutdown;
    }


    while ((clientfd =
            accept(server, (struct sockaddr *)&clientAddr, &addrlen)) != -1) {

        System_printf("vTCPRCServerFxn: Creating thread clientfd = %d\n", clientfd);
        System_flush();
        /* Init the Error_Block */
        Error_init(&eb);

        /* Initialize the defaults and set the parameters. */
        Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg)clientfd;
        taskParams.stackSize = TCP_WORKER_HANDLER_STACK;
        taskParams.priority = TCP_WORKER_TASK_PRIORITY;
        taskHandle = Task_create((Task_FuncPtr)vTCPRCWorker, &taskParams, &eb);
        if (taskHandle == NULL) {
            System_printf("Error: Failed to create new Task\n");
            System_flush();
            close(clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    System_printf("Error: accept failed.\n");
    System_flush();
    shutdown:
    if (server > 0) {
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
Void vTCPRCWorker(UArg arg0, UArg arg1)
{
    int  clientfd = (int)arg0;
    int  bytesRcvd;
    int  bytesSent;
    int  n;
    int32_t i32CmdStatus;
    char buffer[TCPDEVICE_PACKETSIZE];

//    DevicesList_t *pDeviceList;
//    Queue_Elem *elem;

    System_printf("vTCPRCWorker: start clientfd = 0x%x\n", clientfd);
    System_flush();

    // Open the file session
    fdOpenSession((void *)Task_self());

    bytesSent = send(clientfd, g_pucTCPCONSOLEWelcome, sizeof(g_pucTCPCONSOLEWelcome), 0);

    while(1) {
        bytesSent = send(clientfd, g_pcTCPCONSOLEprompt, sizeof(g_pcTCPCONSOLEprompt), 0);
        if (bytesSent != sizeof(g_pcTCPCONSOLEprompt)) {
            System_printf("Error socket: %d failed.\n", clientfd);
            System_flush();
            break;
        }

        bytesRcvd = recv(clientfd, buffer, TCPDEVICE_PACKETSIZE, 0);
        if (bytesRcvd <= 0) {
            System_printf("Closing socket: %d.\n", clientfd);
            System_flush();
            break;
        }
        buffer[bytesRcvd - 2] = 0;
//        System_printf("0x%x -> %s\n", clientfd, buffer);
//        System_flush();

        //
        // Pass the line from the user to the command processor.
        // It will be parsed and valid commands executed.
        //
        i32CmdStatus = CmdLineProcessTCP(clientfd, buffer);

        //
        // Handle the case of bad command.
        //
        if(i32CmdStatus == CMDLINE_BAD_CMD)
        {
            n = sprintf(buffer, ANSI_COLOR_RED "Command not recognized!\n" ANSI_COLOR_RESET);
            send(clientfd, buffer, n, 0);
        }

        //
        // Handle the case of too many arguments.
        //
        else if(i32CmdStatus == CMDLINE_TOO_MANY_ARGS)
        {
            n = sprintf(buffer, ANSI_COLOR_RED "Too many arguments for command processor!\n" ANSI_COLOR_RESET);
            send(clientfd, buffer, n, 0);
        }

        //
        // Handle the case of too few arguments.
        //
        else if(i32CmdStatus == CMDLINE_TOO_FEW_ARGS)
        {
            n = sprintf(buffer, ANSI_COLOR_RED "Too few arguments for command processor!\n" ANSI_COLOR_RESET);
            send(clientfd, buffer, n, 0);
        }

    }

//    while ((bytesRcvd = recv(clientfd, buffer, TCPDEVICE_PACKETSIZE, 0)) > 0) {
////        bytesSent = send(clientfd, buffer, bytesRcvd, 0);
//        buffer[bytesRcvd] = 0;
//        System_printf("0x%x -> %s\n", clientfd, buffer);
//        System_flush();
////        if (bytesSent < 0 || bytesSent != bytesRcvd) {
////            System_printf("Error: send failed.\n");
////            System_flush();
////            break;
////        }
//        switch(buffer[0]) {
//        case '1':
//            for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
//                pDeviceList = (DevicesList_t *)elem;
//                if (pDeviceList->deviceID == 33) {
//                    vDevice_enqueueCharDataMsg( pDeviceList,
//                                                APP_MSG_SERVICE_WRITE,
//                                                clientfd,
//                                                SERVICE_ALTO_VOLUME_UUID, SERVICE_ALTO_VOLUME_CHARACTERISTIC_GET_ID,
//                                                TCPRCDEVICE_ID,
//                                                SERVICE_TCP_REMOTECONTROL_VOLUMEN_RETURN_UUID, 1,
//                                                (uint8_t *)0, 0 );
//                }
//            }
//            break;
//        case '2':
//            for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
//                pDeviceList = (DevicesList_t *)elem;
//                if (pDeviceList->deviceID == 33) {
//                    ALTOAmp_volumeData_t volumeData;
//                    volumeData.zone = 0x03;
//                    volumeData.zone1Volume = 10;
//                    volumeData.zone2Volume = 20;
//                    vDevice_enqueueCharDataMsg( pDeviceList,
//                                                APP_MSG_SERVICE_WRITE,
//                                                clientfd,
//                                                SERVICE_ALTO_VOLUME_UUID, SERVICE_ALTO_VOLUME_CHARACTERISTIC_SET_ID,
//                                                TCPRCDEVICE_ID,
//                                                SERVICE_TCP_REMOTECONTROL_VOLUMEN_RETURN_UUID, 1,
//                                                (uint8_t *)&volumeData, sizeof(ALTOAmp_volumeData_t) );
//                }
//            }
//            break;
////        case '3':
////            for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
////                pDeviceList = (DevicesList_t *)elem;
////                if (pDeviceList->deviceID == 33) {
////                    vDevice_enqueueCharDataMsg( pDeviceList,
////                                                APP_MSG_SERVICE_WRITE,
////                                                clientfd,
////                                                SERVICE_ALTO_VOLUME_UUID, SERVICE_ALTO_VOLUME_CHARACTERISTIC_INC_ID,
////                                                TCPRCDEVICE_ID,
////                                                SERVICE_TCP_REMOTECONTROL_VOLUMEN_RETURN_UUID, 1,
////                                                (uint8_t *)0, 0 );
////                }
////            }
////            break;
////        case '4':
////            for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
////                pDeviceList = (DevicesList_t *)elem;
////                if (pDeviceList->deviceID == 33) {
////                    vDevice_enqueueCharDataMsg( pDeviceList,
////                                                APP_MSG_SERVICE_WRITE,
////                                                clientfd,
////                                                SERVICE_ALTO_VOLUME_UUID, SERVICE_ALTO_VOLUME_CHARACTERISTIC_DEC_ID,
////                                                TCPRCDEVICE_ID,
////                                                SERVICE_TCP_REMOTECONTROL_VOLUMEN_RETURN_UUID, 1,
////                                                (uint8_t *)0, 0 );
////                }
////            }
//            break;
//        default:
//            break;
//        }
//    }


    System_printf("vTCPRCWorker stop clientfd = 0x%x\n", clientfd);
    System_flush();

    close(clientfd);

    // Close the file session
    fdCloseSession((void *)Task_self());
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
static void vTCPRC_processApplicationMessage(device_msg_t *pMsg)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
    case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
        case SERVICE_TCP_REMOTECONTROL_ASCII_VOLUMEN_RETURN_UUID:
            vTCPRC_ALTOVolumeReturnService_ValueChangeHandler(pCharData);
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

void vTCPRC_ALTOVolumeReturnService_ValueChangeHandler(char_data_t *pCharData)
{
    int bytesSent, n;
    char buff[64];
    switch (pCharData->paramID) {
    case 1:
        if(pCharData->dataLen) {
            ALTOAmp_volumeData_t *pDataPayload = (ALTOAmp_volumeData_t *)pCharData->data;
            n = sprintf(buff, "\r Zone 1 Volume: %d, Zone 2 Volume: %d\r\n\n", pDataPayload->zone1Volume, pDataPayload->zone2Volume);
            bytesSent = send(pCharData->connHandle, buff, n, 0);
            bytesSent = send(pCharData->connHandle, g_pcTCPCONSOLEprompt, sizeof(g_pcTCPCONSOLEprompt), 0);
            bytesSent = bytesSent;
        }
        break;
    }
}




void vTCPRCDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vTCPRCDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_TCP_REMOTE_CONTROLLER_ASCII;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_TCPRCDevice_fxnTable;
}


void vTCPRCDevice_close(DeviceList_Handler handle)
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

DeviceList_Handler hTCPRCDevice_open(DeviceList_Handler handle, void *params)
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
    handle->deviceType = DEVICE_TYPE_TCP_REMOTE_CONTROLLER_ASCII;


//    Event_Params_init(&paramsUnion.eventParams);
    handle->eventHandle = Event_create(NULL, &eb);

    handle->msgQHandle = Queue_create(NULL, NULL);


//    Clock_Params_init(&paramsUnion.clockParams);
//    paramsUnion.clockParams.period = TCPRCDEVICE_CLOCK_PERIOD;
//    paramsUnion.clockParams.startFlag = TRUE;
//    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
//    handle->clockHandle = Clock_create(vTCPRCDevice_clockHandler, TCPRCDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
//    if (handle->clockHandle == NULL) {
//        System_abort("Clock create failed");
//    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = TCPRCDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = TCPRCDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vTCPRCDeviceFxn, &paramsUnion.taskParams, &eb);

    return handle;
}



