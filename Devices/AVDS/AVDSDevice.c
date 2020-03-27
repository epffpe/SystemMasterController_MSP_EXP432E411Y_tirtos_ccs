/*
 * AVDSDevice.c
 *
 *  Created on: Mar 25, 2020
 *      Author: epf
 */



#define __DEVICES_AVDS_AVDSDEVICE_GLOBAL
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



static void vAVDSDevice_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1);
static void vAVDSDevice_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);
static void vAVDSDevice_CommandsClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1);


void vAVDSDevice_close(DeviceList_Handler handle);
DeviceList_Handler hAVDSDevice_open(DeviceList_Handler handle, void *params);


const Device_FxnTable g_AVDSDevice_fxnTable =
{
 .closeFxn = vAVDSDevice_close,
 .openFxn = hAVDSDevice_open,
 .sendMsgFxn = vDevice_enqueueCharDataMsg,
};


Void vAVDSDevice_taskFxn(UArg arg0, UArg arg1)
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

    Display_printf(g_SMCDisplay, 0, 0, "AVDS DeviceId (%d) started\n", myDeviceID);

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

//    /*
//     *  Create the Task that farms out incoming TCP connections.
//     *  arg0 will be the port that this task listens to.
//     */
//    Task_Params_init(&taskParams);
//    taskParams.stackSize = TCPBIN_SERVER_HANDLER_STACK;
//    taskParams.priority = TCPBIN_SERVER_TASK_PRIORITY;
//    taskParams.arg0 = TCPBIN_SERVER_PORT;
//    taskTCPServerHandle = Task_create((Task_FuncPtr)vTCPRCBinServerFxn, &taskParams, &eb);
//    if (taskTCPServerHandle == NULL) {
//        System_printf("netOpenHook: Failed to create tcpHandler Task\n");
//    }

    while(1) {
        events = Event_pend(eventHandle, Event_Id_NONE, DEVICE_ALL_EVENTS, 100); //BIOS_WAIT_FOREVER

        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;
            vAVDSDevice_ALTOEmulatorClassService_ValueChangeHandler(NULL, NULL, NULL);
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

            // Process application-layer message probably sent from ourselves.
//            vTCPRCBin_processApplicationMessage(pMsg);
            vAVDSDevice_processApplicationMessage(pMsg, arg0, arg1);

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
        }
    }
}



void vAVDSDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}



void vAVDSDevice_Params_init(Device_Params *params, uint32_t address)
{
    params->deviceID = address;
    params->deviceType = DEVICE_TYPE_AVDS;
    params->arg0 = NULL;
    params->arg1 = NULL;
    params->fxnTablePtr = (Device_FxnTable *)&g_AVDSDevice_fxnTable;
}


void vAVDSDevice_close(DeviceList_Handler handle)
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

DeviceList_Handler hAVDSDevice_open(DeviceList_Handler handle, void *params)
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
    paramsUnion.clockParams.period = DEVICES_AVDS_AVDSDEVICE_CLOCK_PERIOD;
    paramsUnion.clockParams.startFlag = TRUE;
    paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
    handle->clockHandle = Clock_create(vAVDSDevice_clockHandler, DEVICES_AVDS_AVDSDEVICE_CLOCK_TIMEOUT, &paramsUnion.clockParams, &eb);
    if (handle->clockHandle == NULL) {
        System_abort("Clock create failed");
    }

    Task_Params_init(&paramsUnion.taskParams);
    paramsUnion.taskParams.stackSize = DEVICES_AVDS_AVDSDEVICE_TASK_STACK_SIZE;
    paramsUnion.taskParams.priority = DEVICES_AVDS_AVDSDEVICE_TASK_PRIORITY;
    paramsUnion.taskParams.arg0 = (UArg)handle->deviceID;
    paramsUnion.taskParams.arg1 = (UArg)handle;
    handle->taskHandle = Task_create((Task_FuncPtr)vAVDSDevice_taskFxn, &paramsUnion.taskParams, &eb);

    return handle;
}









static void vAVDSDevice_processApplicationMessage(device_msg_t *pMsg, UArg arg0, UArg arg1)
{
  char_data_t *pCharData = (char_data_t *)pMsg->pdu;

  switch (pMsg->type)
  {
  case APP_MSG_SERVICE_WRITE: /* Message about received value write */
      /* Call different handler per service */
      switch(pCharData->svcUUID) {
      case SERVICE_AVDS_ALTO_EMULATOR_UUID:
          switch (pCharData->paramID) {
          case CHARACTERISTIC_AVDS_ALTO_EMULATOR_GET_ID:
          case CHARACTERISTIC_AVDS_ALTO_EMULATOR_SET_ID:
              vAVDSDevice_ALTOEmulatorClassService_ValueChangeHandler(pCharData, arg0, arg1);
              break;
          default:
              break;
          }
          break;
      case SERVICE_AVDS_COMMANDS_UUID:
          vAVDSDevice_CommandsClassService_ValueChangeHandler(pCharData, arg0, arg1);
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

static void vAVDSDevice_ALTOEmulatorClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1)
{
    int sockfd;
    int numbytes;
    int status;
    uint32_t myDeviceID;
    DeviceList_Handler devHandle;
    struct sockaddr_in servaddr;
//    struct hostent *he;
//    tsAVDSCommad *psAVDSCmd;

    char buf[AVDS_MAX_PACKET_SIZE];

    ASSERT(pCharData != NULL);
    ASSERT(arg1 != NULL);

    if (pCharData == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }

    devHandle = (DeviceList_Handler)arg1;
    myDeviceID = devHandle->deviceID;

//    if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
//        herror("gethostbyname");
//        exit(1);
//    }
    if(!bAVDSUDP_isIPValid()) return;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        goto shutdown;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;      /* host byte order */
    servaddr.sin_port = htons(xsAVDSUDP_getPortNumber());    /* short, network byte order */
//    servaddr.sin_addr = *((struct in_addr *)he->h_addr);
//    servaddr.sin_addr.s_addr = inet_addr("10.0.0.13");
    servaddr.sin_addr.s_addr = htonl(xAVDSUDP_getIPAddress());
//    bzero(&(servaddr.sin_zero), 8);     /* zero the rest of the struct */

    status = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
    if ( status == -1) {
        goto shutdown;
    }


//    psAVDSCmd = (tsAVDSCommad *)buf;
//    psAVDSCmd->commandID = AVDS_CMD_Get_Channel;
//
//    tsAVDSCommandGetChannel *pGetChannelCmd = (tsAVDSCommandGetChannel *)psAVDSCmd->commandData;
//    pGetChannelCmd->outputChannel = htons(0x0001);
//
//    status = send(sockfd, buf, sizeof(tsAVDSCommad) + sizeof(tsAVDSCommandGetChannel), 0);

    status = send(sockfd, pCharData->data, pCharData->dataLen, 0);
    if (status == -1){
        goto shutdown;
    }
    //        printf("After the send function \n");

    numbytes = recv(sockfd, buf, AVDS_MAX_PACKET_SIZE, 0);
    if ( numbytes == -1) {
        goto shutdown;
    }

    switch (pCharData->paramID) {
    case CHARACTERISTIC_AVDS_ALTO_EMULATOR_GET_ID:
        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 pCharData->connHandle,
                                 pCharData->retSvcUUID, pCharData->retParamID,
                                 myDeviceID,
                                 SERVICE_AVDS_ALTO_EMULATOR_UUID, CHARACTERISTIC_AVDS_ALTO_EMULATOR_GET_ID,
                                 (uint8_t *)buf, numbytes);
        break;
    case CHARACTERISTIC_AVDS_ALTO_EMULATOR_SET_ID:
        vDevice_sendCharDataMsg (pCharData->retDeviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 pCharData->connHandle,
                                 pCharData->retSvcUUID, pCharData->retParamID,
                                 myDeviceID,
                                 SERVICE_AVDS_ALTO_EMULATOR_UUID, CHARACTERISTIC_AVDS_ALTO_EMULATOR_SET_ID,
                                 (uint8_t *)buf, numbytes);
        break;
    default:
        break;
    }

shutdown:
    if (sockfd != -1) {
        close(sockfd);
    }
}


static void vAVDSDevice_CommandsClassService_ValueChangeHandler(char_data_t *pCharData, UArg arg0, UArg arg1)
{

}

