/*
 * RosenDevice.c
 *
 *  Created on: Apr 3, 2020
 *      Author: epf
 */



#define __DEVICES_ROSEN_ROSENDEVICE_GLOBAL
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
#include <ti/net/bsd/netdb.h>

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();


Void vRosenDevice_UDPRxtaskFxn(UArg arg0, UArg arg1);

static void vRosenDevice_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1);
static void vRosenDevice_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);
//static void vRosenDevice_CommandsClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);
static void vRosenDevice_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);
static void vRosenDevice_udpTest_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);

int xRosenDevice_udpCmdLineProcessTCP(char *pcCmdLine, char **ppcArgv, uint8_t *pui8Argc);


void vRosenDevice_close(DeviceList_Handler handle);
DeviceList_Handler hRosenDevice_open(DeviceList_Handler handle, void *params);


/* This buffer is not directly accessed by the application */
RosenUDPDevice_MailboxMsgObj g_RosenUDPDevice_mailboxBuffer[ROSEN_UDP_NUMMSGS];



const Device_FxnTable g_RosenDevice_fxnTable =
{
 .closeFxn = vRosenDevice_close,
 .openFxn = hRosenDevice_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};


Void vRosenDevice_taskFxn(UArg arg0, UArg arg1)
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

//            char bufChar[sizeof(char_data_t) + sizeof(RosenUDPDevice_SteveCommandData_t)];
//            char_data_t *pCharData = (char_data_t *)bufChar;
//            pCharData->dataLen = sizeof(RosenUDPDevice_SteveCommandData_t);
//            pCharData->paramID = CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_GET_ID;
//
//            RosenUDPDevice_SteveCommandData_t *pCmdData = (RosenUDPDevice_SteveCommandData_t *)pCharData->data;
//            pCmdData->ui32IPAddress = 0xC0A8031B;
//            vRosenDevice_SteveCommandsService_ValueChangeHandler((char_data_t *)bufChar, arg0, arg1);
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

            vRosenDevice_processApplicationMessage(pMsg, arg0, arg1);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }
}



void vRosenDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vRosenDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_ROSEN;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_RosenDevice_fxnTable;
}


void vRosenDevice_close(DeviceList_Handler handle)
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

DeviceList_Handler hRosenDevice_open(DeviceList_Handler handle, void *params)
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
    paramsUnion.clockParams.period = DEVICES_ROSEN_ROSENDEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vRosenDevice_clockHandler, DEVICES_ROSEN_ROSENDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = DEVICES_ROSEN_ROSENDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = DEVICES_ROSEN_ROSENDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vRosenDevice_taskFxn, &paramsUnion.taskParams, &eb);

    return handle;
}



/* =================================================================================
 * =================================================================================
 *
 */





static void vRosenDevice_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_ROSEN_UDP_ALTO_EMULATOR_UUID:
//          switch (pCharData->paramID) {
//          case CHARACTERISTIC_SERVICE_ROSEN_UDP_ALTO_EMULATOR_DIRECT_COMMAND_ID:
              vRosenDevice_ALTOEmulatorClassService_ValueChangeHandler(pCharData, arg0, arg1);
//              break;
//          default:
//              break;
//          }
          break;
      case SERVICE_ROSEN_UDP_STEVE_COMMANDS_UUID:
          vRosenDevice_SteveCommandsService_ValueChangeHandler(pCharData, arg0, arg1);
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

static void vRosenDevice_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1)
{
    int sockfd;
    int bytesRcvd;
    int bytesSent;
    int status;
    int n;
    fd_set             readSet, write_fds;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    socklen_t          addrlen;

    uint32_t IPTmp;

    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
    struct sockaddr_in servaddr;

    char buffer[ROSEN_MAX_PACKET_SIZE];

    ASSERT(pCharData != NULL);
    ASSERT(arg1 != NULL);

//    if (pCharData == NULL) {
//        return;
//    }
//    if (arg1 == NULL) {
//        return;
//    }
//
//    devHandle = (DeviceList_Handler)arg1;
//    myDeviceID = devHandle->deviceID;


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        Display_printf(g_SMCDisplay, 0, 0, "Error: socket not created.\n");
        goto shutdown;
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(0xC0A8032A);
    clientAddr.sin_port = htons(ROSEN_UDP_PORT);

//    n = sprintf(buf, "System Master Controller ID: %06d", pManufacturerInformation->unitSerialNumber);
//
//    numbytes = sendto(server, buf, n, 0, (struct sockaddr *)&servaddr, sizeof(servaddr););
//
//    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
//                    MSG_WAITALL, (struct sockaddr *) &servaddr,
//                    &len);


    n = sprintf(buffer, "do_down");
    addrlen = sizeof(clientAddr);

    bytesSent = sendto(sockfd, buffer, n, 0, (struct sockaddr *)&clientAddr, addrlen);

    if(bytesSent != n){
        Display_printf(g_SMCDisplay, 0, 0, "Error: sendto failed.\n");
        //                        System_printf("Error: udp sendto failed.\n");
        //                        System_flush();

    }

    /*
     *  readSet and addrlen are value-result arguments, which must be reset
     *  in between each select() and recvfrom() call
     */
    FD_ZERO(&readSet);
    FD_SET(sockfd, &readSet);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100e3;

    /* Wait forever for the reply */
    status = select(sockfd + 1, &readSet, &write_fds, NULL, &timeout);
    if (status > 0) {
        if (FD_ISSET(sockfd, &readSet)) {
            memset(&clientAddr, 0, sizeof(clientAddr));
            clientAddr.sin_family = AF_INET;
            clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            clientAddr.sin_port = htons(7399);

            bytesRcvd = recvfrom(sockfd, buffer, ROSEN_MAX_PACKET_SIZE, 0,
                                 (struct sockaddr *)&clientAddr, &addrlen);
            buffer[bytesRcvd] = 0;
            if (bytesRcvd > 0) {
                IPTmp = ntohl(clientAddr.sin_addr.s_addr);
                Display_printf(g_SMCDisplay, 0, 0, "UDP remoteIp:\t:%d.%d.%d.%d:%d\n", (uint8_t)(IPTmp>>24)&0xFF,
                               (uint8_t)(IPTmp>>16)&0xFF,
                               (uint8_t)(IPTmp>>8)&0xFF,
                               (uint8_t)IPTmp&0xFF,
                               ntohs(clientAddr.sin_port)
                );
                System_printf("RosenIp:\t:%d.%d.%d.%d:%d -> ", (uint8_t)(IPTmp>>24)&0xFF,
                              (uint8_t)(IPTmp>>16)&0xFF,
                              (uint8_t)(IPTmp>>8)&0xFF,
                              (uint8_t)IPTmp&0xFF,
                              clientAddr.sin_port);
                System_printf("%s\n", buffer);
                System_flush();
            }
            FD_CLR(sockfd, &readSet);
        }
    }


//    switch (pCharData->paramID) {
//    case CHARACTERISTIC_AVDS_ALTO_EMULATOR_DIRECT_COMMAND_ID:
//        vDevice_sendCharDataMsg (pCharData->retDeviceID,
//                                 APP_MSG_SERVICE_WRITE,
//                                 pCharData->connHandle,
//                                 pCharData->retSvcUUID, pCharData->retParamID,
//                                 myDeviceID,
//                                 SERVICE_AVDS_ALTO_EMULATOR_UUID, CHARACTERISTIC_AVDS_ALTO_EMULATOR_DIRECT_COMMAND_ID,
//                                 (uint8_t *)buf, numbytes);
//        break;
//    default:
//        break;
//    }

shutdown:
    if (sockfd != -1) {
        close(sockfd);
    }
}








static void vRosenDevice_SteveCommandsService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1)
{
    int sockfd;
//    int bytesRcvd;
    int bytesSent;
//    int status;
    int n;
//    fd_set             readSet, write_fds;
//    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    socklen_t          addrlen;
    RosenUDPDevice_SteveCommandData_t *pCmdData;



    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
//    struct sockaddr_in servaddr;

    char buffer[ROSEN_MAX_PACKET_SIZE];


    Task_Params     taskParams;
    Task_Handle     taskHandle;
    Mailbox_Params  mbxParams;

    Mailbox_Handle mbxHandle;

    Error_Block eb;




    ASSERT(pCharData != NULL);
    ASSERT(arg1 != NULL);

    if (pCharData == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }

    Error_init(&eb);

    if(pCharData->dataLen == sizeof(RosenUDPDevice_SteveCommandData_t)) {

        devHandle = (DeviceList_Handler)arg1;
        myDeviceID = devHandle->deviceID;

        pCmdData = (RosenUDPDevice_SteveCommandData_t *)pCharData->data;

        sockfd = -1;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            Display_printf(g_SMCDisplay, 0, 0, "Error: socket not created.\n");
            goto shutdown;
        }

        memset(&clientAddr, 0, sizeof(clientAddr));
        clientAddr.sin_family = AF_INET;
            clientAddr.sin_addr.s_addr = htonl(pCmdData->ui32IPAddress);
//        clientAddr.sin_addr.s_addr = htonl(0xC0A8032A);
        clientAddr.sin_port = htons(ROSEN_UDP_PORT);


        addrlen = sizeof(clientAddr);

        switch (pCharData->paramID) {
        case CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_SET_ID:
        {
            switch(pCmdData->ui8Value) {
            case 0:
                n = sprintf(buffer, "launch_app -n com.rosen.rosenplayer/.MainActivity -e source SDI1");
                break;
            case 1:
                n = sprintf(buffer, "launch_app -n com.rosen.rosenplayer/.MainActivity -e source SDI2");
                break;
            case 2:
                n = sprintf(buffer, "launch_app -n com.rosen.rosenplayer/.MainActivity -e source HDMI");
                break;
            case 3:
                n = sprintf(buffer, "launch_app -n com.rosen.rosenplayer/.MainActivity -e source COMPOSITE");
                break;
            default:
                n = 0;
                break;
            }


            bytesSent = sendto(sockfd, buffer, n, 0, (struct sockaddr *)&clientAddr, addrlen);

            if(bytesSent != n){
                Display_printf(g_SMCDisplay, 0, 0, "Error: sendto failed.\n");

            }
            break;
        }
        case CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_GET_ID:
            n = sprintf(buffer, "get_rosen_state video_player.source");

            bytesSent = sendto(sockfd, buffer, n, 0, (struct sockaddr *)&clientAddr, addrlen);
            if(bytesSent != n){
                Display_printf(g_SMCDisplay, 0, 0, "Error: sendto failed.\n");
            }

            /* Construct a Mailbox instance */
            Mailbox_Params_init(&mbxParams);
            mbxParams.buf = (Ptr)g_RosenUDPDevice_mailboxBuffer;
            mbxParams.bufSize = sizeof(g_RosenUDPDevice_mailboxBuffer);
//            Mailbox_construct(&mbxStruct, sizeof(MsgObj), NUMMSGS, &mbxParams, NULL);
//            mbxHandle = Mailbox_handle(&mbxStruct);
            mbxHandle = Mailbox_create(sizeof(RosenUDPDevice_MsgObj), ROSEN_UDP_NUMMSGS, &mbxParams, &eb);


            Task_Params_init(&taskParams);
            taskParams.stackSize = DEVICES_ROSEN_ROSENDEVICE_TASK_STACK_SIZE;
            taskParams.priority = DEVICES_ROSEN_ROSENDEVICE_TASK_PRIORITY - 1;
            taskParams.arg0 = (UArg)sockfd;
            taskParams.arg1 = (UArg)mbxHandle;
            taskHandle = Task_create((Task_FuncPtr)vRosenDevice_UDPRxtaskFxn, &taskParams, &eb);

            RosenUDPDevice_MsgObj msg;

//            Task_sleep((unsigned int)50);
//            Bool isRetValid;
//            isRetValid = Mailbox_pend(mbxHandle, &msg, ROSEN_UDP_TIMEOUT);

            if (Mailbox_pend(mbxHandle, &msg, ROSEN_UDP_TIMEOUT)) {
                RosenUDPDevice_SteveCommandData_t steveResponseData;
                steveResponseData.ui32IPAddress = msg.id;
                steveResponseData.ui8Value = msg.val;
                vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                         APP_MSG_SERVICE_WRITE,
                                         pCharData->connHandle,
                                         pCharData->retSvcUUID, pCharData->retParamID,
                                         myDeviceID,
                                         SERVICE_ROSEN_UDP_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_GET_ID,
                                         (uint8_t *)&steveResponseData, sizeof(RosenUDPDevice_SteveCommandData_t));

            }

            Mailbox_delete(&mbxHandle);
            break;
        default:
            break;
        }


        shutdown:
        if (sockfd != -1) {
            close(sockfd);
        }
    }
}





Void vRosenDevice_UDPRxtaskFxn(UArg arg0, UArg arg1)
{
    int serverfd;
    int                bytesRcvd;
    struct sockaddr_in serverAddr;
    socklen_t          addrlen;
    Mailbox_Handle      mbxHandle;
    char               buffer[64];

    char *ppcRosenDevice_UDPArgv[ROSEN_UDP_CMDLINE_MAX_ARGS + 1];
    uint8_t ui8Argc;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }

    mbxHandle = (Mailbox_Handle)arg1;

    fdOpenSession(TaskSelf());

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(ROSEN_UDP_PORT);

    serverfd = (int)arg0;
    addrlen = sizeof(serverAddr);


//    bytesRcvd = 1;
    bytesRcvd = recvfrom(serverfd, buffer, 64, 0,
                         (struct sockaddr *)&serverAddr, &addrlen);
    if (bytesRcvd > 0) {

        buffer[bytesRcvd] = 0;
//        sprintf(buffer, "OKAY get_rosen_state.video_player.source 1");
        ui8Argc = ROSEN_UDP_CMDLINE_MAX_ARGS;
        if (xRosenDevice_udpCmdLineProcessTCP(buffer, ppcRosenDevice_UDPArgv, &ui8Argc) > 2) {

            RosenUDPDevice_MsgObj msg;
            msg.id = ntohl(serverAddr.sin_addr.s_addr);
            msg.val = *ppcRosenDevice_UDPArgv[3] - '0';

            if (Mailbox_post(mbxHandle, &msg, BIOS_NO_WAIT)) {
                System_printf("Mailbox Write: ID = %d and Value = '%c'.\n",
                              msg.id, msg.val);
            } else {
                System_printf("Mailbox Write Failed: ID = %d and Value = '%c'.\n",
                              msg.id, msg.val);
            }
        }
    }


//shutdown:
    if (serverfd != -1) {
        close(serverfd);
    }

    fdCloseSession(TaskSelf());


}

static void vRosenDevice_udpTest_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1)
{
    Task_Params     taskParams;
    Task_Handle     taskHandle;

    int                status;
    int                serverfd;

    struct sockaddr_in localAddr;

    Error_Block eb;


    Error_init(&eb);


    serverfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverfd == -1) {
        Display_printf(g_SMCDisplay, 0, 0, "Error: socket not created.\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(ROSEN_UDP_PORT);

    status = bind(serverfd, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        Display_printf(g_SMCDisplay, 0, 0, "Error: bind failed.\n");
        goto shutdown;
    }


    Task_Params_init(&taskParams);
    taskParams.stackSize = DEVICES_ROSEN_ROSENDEVICE_TASK_STACK_SIZE;
    taskParams.priority = DEVICES_ROSEN_ROSENDEVICE_TASK_PRIORITY - 1;
    taskParams.arg0 = (UArg)serverfd;
    taskParams.arg1 = (UArg)NULL;
    taskHandle = Task_create((Task_FuncPtr)vRosenDevice_UDPRxtaskFxn, &taskParams, &eb);

    Task_sleep((unsigned int)50);

shutdown:
    if (serverfd != -1) {
        close(serverfd);
    }
}

int xRosenDevice_udpCmdLineProcessTCP(char *pcCmdLine, char **ppcArgv, uint8_t *pui8Argc)
{
    char *pcChar;
    char cmdMaxArgs;
    bool bFindArg = true;
//    uint_fast8_t ui8Argc;
//    tTCPCmdLineEntry *psCmdEntry;

    ASSERT(pcCmdLine != NULL);
    ASSERT(ppcArgv != NULL);
    ASSERT(pui8Argc != NULL);

    if (pcCmdLine == NULL) {
        return CMDLINE_BAD_CMD;
    }
    if (ppcArgv == NULL) {
        return CMDLINE_BAD_CMD;
    }
    if (pui8Argc == NULL) {
        return CMDLINE_BAD_CMD;
    }

    //
    // Initialize the argument counter, and point to the beginning of the
    // command line string.
    //
    cmdMaxArgs = *pui8Argc;

    *pui8Argc = 0;
    pcChar = pcCmdLine;

    //
    // Advance through the command line until a zero character is found.
    //
    while (*pcChar) {
        //
        // If there is a space, then replace it with a zero, and set the flag
        // to search for the next argument.
        //
        if (*pcChar == ' ') {
            *pcChar = 0;
            bFindArg = true;
        }

        //
        // Otherwise it is not a space, so it must be a character that is part
        // of an argument.
        //
        else {
            //
            // If bFindArg is set, then that means we are looking for the start
            // of the next argument.
            //
            if (bFindArg) {
                //
                // As long as the maximum number of arguments has not been
                // reached, then save the pointer to the start of this new arg
                // in the argv array, and increment the count of args, argc.
                //
                if (*pui8Argc < cmdMaxArgs) {
                    ppcArgv[*pui8Argc] = pcChar;
                    (*pui8Argc)++;
                    bFindArg = false;
                }

                //
                // The maximum number of arguments has been reached so return
                // the error.
                //
                else {
                    return (CMDLINE_TOO_MANY_ARGS);
                }
            }
        }

        //
        // Advance to the next character in the command line.
        //
        pcChar++;
    }
//
//    //
//    // If one or more arguments was found, then process the command.
//    //
//    if (ui8Argc) {
//        //
//        // Start at the beginning of the command table, to look for a matching
//        // command.
//        //
//        psCmdEntry = &g_psTCPCmdTable[0];
//
//        //
//        // Search through the command table until a null command string is
//        // found, which marks the end of the table.
//        //
//        while (psCmdEntry->pcCmd) {
//            //
//            // If this command entry command string matches argv[0], then call
//            // the function for this command, passing the command line
//            // arguments.
//            //
//            if (!strcmp(g_ppcTCPArgv[0], psCmdEntry->pcCmd)) {
//                return (psCmdEntry->pfnCmd(sockfd, ui8Argc, g_ppcTCPArgv));
//            }
//
//            //
//            // Not found, so advance to the next entry.
//            //
//            psCmdEntry++;
//        }
//    }

    //
    // Fall through to here means that no matching command was found, so return
    // an error.
    //
//    return (CMDLINE_BAD_CMD);
    return (*pui8Argc);
}

