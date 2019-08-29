/*
 * TCPBinaryCMD_DAC.c
 *
 *  Created on: Jul 23, 2019
 *      Author: epenate
 */

#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_DAC_TCPBINARYCMD_DAC_GLOBAL
#include "includes.h"



void vTCPRCBin_DAC_setVoltageValue(int clientfd, char *payload, int32_t size)
{
    int bytesSent, index;
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

