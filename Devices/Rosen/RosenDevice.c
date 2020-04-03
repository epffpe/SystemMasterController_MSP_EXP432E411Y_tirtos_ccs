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



static void vRosenDevice_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1);
static void vRosenDevice_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);
static void vRosenDevice_CommandsClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);


void vRosenDevice_close(DeviceList_Handler handle);
DeviceList_Handler hRosenDevice_open(DeviceList_Handler handle, void *params);


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
            vRosenDevice_ALTOEmulatorClassService_ValueChangeHandler(NULL, NULL, NULL);
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









static void vRosenDevice_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1)
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
              vRosenDevice_ALTOEmulatorClassService_ValueChangeHandler(pCharData, arg0, arg1);
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
    clientAddr.sin_port = htons(7399);

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




