/*
 * UDPAVDSServer.c
 *
 *  Created on: Mar 25, 2020
 *      Author: epf
 */



#include <string.h>
#include <stdint.h>


#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <pthread.h>
/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include <ti/net/slnetutils.h>

#include <ti/display/Display.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Types.h>

/* BIOS module Headers */
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateMutexPri.h>

//#include <ti/drivers/Board.h>



#define __NDK_UDPAVDSSERVER_GLOBAL
#include "NDK/UDPAVDSServer.h"
#include "Deprecated/AVDS/AVDSClient.h"

#define UDPPACKETSIZE 1472
#define MAXPORTLEN    6


extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

extern Display_Handle g_SMCDisplay;


Void vAVDSUDP_clockHandlerFxn(UArg arg0);


GateMutexPri_Handle g_hAVDSUDP_gate;
/*
 *  ======== echoFxn ========
 *  Echoes UDP messages.
 *
 */

tsUDPAVDSInfoMsg g_sUDPAVDSConnectionInfo =
{
 .ipAddress = 0,
 .portNumber = 0,
 .isValid = false,
};

void *pvUDPAVDSFinder_taskFxn(void *arg0)
{
    unsigned int        key;
    int                bytesRcvd;
//    int                bytesSent;
    int                status;
    int                server;
//    int n;
    tsAVDSInfoMsg *ptsAVDSInfo;
    fd_set             readSet;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    socklen_t          addrlen;
    char               buffer[UDPPACKETSIZE];
    uint16_t           portNumber = *(uint16_t *)arg0;
    uint32_t IPTmp;
//    char response[64];
//    int err;
    /* Construct BIOS Objects */
    Clock_Params clkParams;
    Clock_Handle clkHandle;
    Error_Block eb;
    Error_init(&eb);


    fdOpenSession(TaskSelf());

    Display_printf(g_SMCDisplay, 0, 0, "UDP Server Started on Port (%d)\n", portNumber);

    server = socket(AF_INET, SOCK_DGRAM, 0);
    if (server == -1) {
        Display_printf(g_SMCDisplay, 0, 0, "Error: socket not created.\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(portNumber);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
//        err = fdError();
//        Display_printf(g_SMCDisplay, 0, 0, "Error: bind failed. err=%d\n", err);
        Display_printf(g_SMCDisplay, 0, 0, "Error: bind failed.\n");
        goto shutdown;
    }

    Clock_Params_init(&clkParams);
    clkParams.period = 0; //5000/Clock_tickPeriod;
    clkParams.startFlag = FALSE;
    clkHandle = Clock_create(vAVDSUDP_clockHandlerFxn, 5000, &clkParams, &eb);
    if (clkHandle == NULL) {
        System_abort("Clock create failed");
    }

    g_hAVDSUDP_gate = GateMutexPri_create(NULL, &eb);
    if (g_hAVDSUDP_gate == NULL) {
        System_abort("Could not create AVDS UDP gate");
    }

    do {
        /*
         *  readSet and addrlen are value-result arguments, which must be reset
         *  in between each select() and recvfrom() call
         */
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);
        addrlen = sizeof(clientAddr);

        /* Wait forever for the reply */
        status = select(server + 1, &readSet, NULL, NULL, NULL);
        if (status > 0) {
            if (FD_ISSET(server, &readSet)) {
                bytesRcvd = recvfrom(server, buffer, UDPPACKETSIZE, 0,
                        (struct sockaddr *)&clientAddr, &addrlen);
//                buffer[bytesRcvd] = 0;
                if (bytesRcvd > 0) {
                    IPTmp = ntohl(clientAddr.sin_addr.s_addr);
                    ptsAVDSInfo = (tsAVDSInfoMsg *)buffer;
                    IPTmp = ntohl(ptsAVDSInfo->ipAddress);

                    Display_printf(g_SMCDisplay, 0, 0, "AVDS remoteIp:\t:%d.%d.%d.%d:%d\n",
                                   (uint8_t)(IPTmp>>24)&0xFF,
                                   (uint8_t)(IPTmp>>16)&0xFF,
                                   (uint8_t)(IPTmp>>8)&0xFF,
                                   (uint8_t)IPTmp&0xFF,
//                                   ntohs(clientAddr.sin_port)
                                   ntohs(ptsAVDSInfo->portNumber)
                    );

//                    System_printf("AVDS remoteIp:\t:%d.%d.%d.%d:%d\n", (uint8_t)(IPTmp>>24)&0xFF,
//                                  (uint8_t)(IPTmp>>16)&0xFF,
//                                  (uint8_t)(IPTmp>>8)&0xFF,
//                                  (uint8_t)IPTmp&0xFF,
//                                  ntohs(ptsAVDSInfo->portNumber));
//                    System_flush();

                    Clock_stop(clkHandle);
                    /* Need exclusive access to prevent a race condition */
                    key = GateMutexPri_enter(g_hAVDSUDP_gate);
                    g_sUDPAVDSConnectionInfo.ipAddress = ntohl(ptsAVDSInfo->ipAddress);
                    g_sUDPAVDSConnectionInfo.portNumber = ntohs(ptsAVDSInfo->portNumber);
                    g_sUDPAVDSConnectionInfo.isValid = true;
                    GateMutexPri_leave(g_hAVDSUDP_gate, key);
                    Clock_start(clkHandle);
                }
            }
        }
    } while (status > 0);

shutdown:
    if (server != -1) {
        close(server);
    }

    fdCloseSession(TaskSelf());

    return (NULL);
}



Void vAVDSUDP_clockHandlerFxn(UArg arg)
{
//    ASSERT(arg != NULL);
//    if (arg == NULL) {
//        return;
//    }
    g_sUDPAVDSConnectionInfo.isValid = false;
}

bool bAVDSUDP_isIPValid()
{
    unsigned int key;
    bool retVal;
    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(g_hAVDSUDP_gate);
    retVal = g_sUDPAVDSConnectionInfo.isValid;
    GateMutexPri_leave(g_hAVDSUDP_gate, key);
    return retVal;
}

uint32_t xAVDSUDP_getIPAddress()
{
    unsigned int key;
    uint32_t retVal;
    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(g_hAVDSUDP_gate);
    retVal = g_sUDPAVDSConnectionInfo.ipAddress;
    GateMutexPri_leave(g_hAVDSUDP_gate, key);
    return retVal;
}

uint16_t xsAVDSUDP_getPortNumber()
{
    unsigned int key;
    uint16_t retVal;
    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(g_hAVDSUDP_gate);
    retVal = g_sUDPAVDSConnectionInfo.portNumber;
    GateMutexPri_leave(g_hAVDSUDP_gate, key);
    return retVal;
}
