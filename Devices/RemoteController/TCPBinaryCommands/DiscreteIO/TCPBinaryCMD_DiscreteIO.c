/*
 * TCPBinaryCMD_DiscreteIO.c
 *
 *  Created on: Oct 12, 2018
 *      Author: epenate
 */

#define __TCPBINARYCOMMANDS_DISCRETEIO_GLOBAL
#include "includes.h"


void vTCPRCBin_DiscreteIO_get5VOutputPowerStatus(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_5VPowerStatus_payload_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_DiscreteIO_get5VOutputPowerStatusResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    TCPBin_CMD_5VPowerStatus_payload_t *pFramePayload = (TCPBin_CMD_5VPowerStatus_payload_t *)pFrame->payload;


    pFramePayload->v5Status = DIGet(DIO_5V_OUT_STAT);

    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_DiscreteIO_turnOn5VOutputPower(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_5VPowerStatus_payload_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_DiscreteIO_turnOn5VOutputPowerResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
    pFrame->retParamID = 2;

    TCPBin_CMD_5VPowerStatus_payload_t *pFramePayload = (TCPBin_CMD_5VPowerStatus_payload_t *)pFrame->payload;


    DOSet(DIO_5V_OUT_EN, DO_ON);
    pFramePayload->v5Status = DIGet(DIO_5V_OUT_STAT);

    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_DiscreteIO_turnOff5VOutputPower(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_5VPowerStatus_payload_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_DiscreteIO_turnOff5VOutputPowerResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
    pFrame->retParamID = 3;

    TCPBin_CMD_5VPowerStatus_payload_t *pFramePayload = (TCPBin_CMD_5VPowerStatus_payload_t *)pFrame->payload;

    DOSet(DIO_5V_OUT_EN, DO_OFF);
    pFramePayload->v5Status = DIGet(DIO_5V_OUT_STAT);

    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_DiscreteIO_set5VOutputPower(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_5VPowerStatus_payload_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_DiscreteIO_set5VOutputPowerResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
    pFrame->retParamID = 4;

    TCPBin_CMD_5VPowerStatus_payload_t *pFramePayload = (TCPBin_CMD_5VPowerStatus_payload_t *)pFrame->payload;


    pFramePayload->v5Status = DIGet(DIO_5V_OUT_STAT);

    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


