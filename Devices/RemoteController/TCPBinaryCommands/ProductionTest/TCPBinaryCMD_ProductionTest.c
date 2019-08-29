/*
 * TCPBinaryCMD_ProductionTest.c
 *
 *  Created on: Aug 27, 2019
 *      Author: epenate
 */


#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_GLOBAL
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




void vTCPRCBin_ProductionTest_getCounters(int clientfd, char *payload, int32_t size)
{
    int bytesSent, index;
    Memory_Stats memStats;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(g_TFUartTestErrorCounter) + 2*sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;

    Memory_getStats(NULL, &memStats);

    pFrame->type = TCP_CMD_TEST_getCountersResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;

    for (index = 0; index < IF_COUNT; index++) pFramePayload[index] = g_TFUartTestErrorCounter[index];
    pFramePayload[index++] = 0;
    pFramePayload[index++] = 0;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_ProductionTest_resetCounters(int clientfd, char *payload, int32_t size)
{
    int bytesSent, index;
    Memory_Stats memStats;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;

    Memory_getStats(NULL, &memStats);

    pFrame->type = TCP_CMD_TEST_resetCountersResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;

    for (index = 0; index < IF_COUNT; index++) g_TFUartTestErrorCounter[index] = 0;
    *pFramePayload = 1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}
