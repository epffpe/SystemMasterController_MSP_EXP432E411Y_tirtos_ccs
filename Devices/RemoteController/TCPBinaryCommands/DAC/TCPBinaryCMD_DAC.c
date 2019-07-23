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
    float *pfVoltageValue = (float *)payload;
    vSPIDAC_setVoltageLevel(*pfVoltageValue);
//    TCP_CMD_DAC_setVoltageResponse
}

