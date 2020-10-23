/*
 * TCPBinaryCMD_DAC.c
 *
 *  Created on: Jul 23, 2019
 *      Author: epenate
 */

#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_DAC_TCPBINARYCMD_DAC_GLOBAL
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


void vTCPRCBin_DAC_setVoltageValue(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    Memory_Stats memStats;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;

    Memory_getStats(NULL, &memStats);

    pFrame->type = TCP_CMD_DAC_setVoltageResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;


    float *pfVoltageValue = (float *)payload;
    vSPIDAC_setVoltageLevel(*pfVoltageValue);
//    TCP_CMD_DAC_setVoltageResponse

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;
    *pFramePayload = 1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;

}

