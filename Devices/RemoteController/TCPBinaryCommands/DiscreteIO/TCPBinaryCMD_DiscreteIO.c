/*
 * TCPBinaryCMD_DiscreteIO.c
 *
 *  Created on: Oct 12, 2018
 *      Author: epenate
 */
#define __TCPBINARYCOMMANDS_DISCRETEIO_GLOBAL
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

typedef enum {
    TCPRCBin_DiscreteIO_DIOIndex_0 = 0,
    TCPRCBin_DiscreteIO_DIOIndex_1,
    TCPRCBin_DiscreteIO_DIOIndex_2,
    TCPRCBin_DiscreteIO_DIOIndex_3,
    TCPRCBin_DiscreteIO_DIOIndex_4,
    TCPRCBin_DiscreteIO_DIOIndex_5,

    TCPRCBin_DiscreteIO_DIOIndex_Count,
}TCPRCBin_DiscreteIO_DIO_Index_Names;

const uint8_t g_TCPRCBin_DiscreteIO_GPIMapTable[] =
{
 Board_GPI_0,
 Board_GPI_1,
 Board_GPI_2,
 Board_GPI_3,
 Board_GPI_4,
 Board_GPI_5,
};

const uint8_t g_TCPRCBin_DiscreteIO_GPOMapTable[] =
{
 Board_GPO_0,
 Board_GPO_1,
 Board_GPO_2,
 Board_GPO_3,
 Board_GPO_4,
 Board_GPO_5,
};

const uint8_t g_TCPRCBin_DiscreteIO_DIMapTable[] =
{
 DIO_GPI_0,
 DIO_GPI_1,
 DIO_GPI_2,
 DIO_GPI_3,
 DIO_GPI_4,
 DIO_GPI_5,
};

const uint8_t g_TCPRCBin_DiscreteIO_DOMapTable[] =
{
 DIO_GPO_0,
 DIO_GPO_1,
 DIO_GPO_2,
 DIO_GPO_3,
 DIO_GPO_4,
 DIO_GPO_5,
};


void vTCPRCBin_DiscreteIO_getDIOConfiguration(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    uint8_t index;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_DIOConfig_payload_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_DiscreteIO_getDIOConfigurationResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
    pFrame->retParamID = 4;

    TCPBin_CMD_DIOConfig_payload_t *pFramePayload = (TCPBin_CMD_DIOConfig_payload_t *)pFrame->payload;
    index = *(uint8_t *)payload;

    if (index < TCPRCBin_DiscreteIO_DIOIndex_Count) {
//        GPIO_getConfig(g_TCPRCBin_DiscreteIO_GPIMapTable[index], (GPIO_PinConfig *)&pFramePayload->pinConfig);
        pFramePayload->pinConfig = g_sEEPROMDIOCfgData.dioCfg[index];
        pFramePayload->index = index;
        switch (pFramePayload->pinConfig & GPIO_CFG_IO_MASK) {
        case GPIO_CFG_IN_NOPULL:
        case GPIO_CFG_IN_PU:
        case GPIO_CFG_IN_PD:
            pFramePayload->DIVal = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIVal;
            pFramePayload->DIDebounceDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIDebounceDly;
            pFramePayload->DIRptStartDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIRptStartDly;
            pFramePayload->DIRptDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIRptDly;
            pFramePayload->DIIn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIIn;
            pFramePayload->DIBypassEn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIBypassEn;
            pFramePayload->DIModeSel = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIModeSel;
            pFramePayload->DIDebounceEn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIDebounceEn;
            break;
        case GPIO_CFG_OUT_STD:
        case GPIO_CFG_OUT_OD_NOPULL:
        case GPIO_CFG_OUT_OD_PU:
        case GPIO_CFG_OUT_OD_PD:
            pFramePayload->DOA = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOA;
            pFramePayload->DOB = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOB;
            pFramePayload->DOBCtr = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBCtr;
            pFramePayload->DOOut = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOOut;
            pFramePayload->DOCtrl = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOCtrl;
            pFramePayload->DOBypass = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBypass;
            pFramePayload->DOBypassEn = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBypassEn;
            pFramePayload->DOModeSel = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOModeSel;
            pFramePayload->DOBlinkEnSel = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBlinkEnSel;
            pFramePayload->DOInv = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOInv;
            break;
        default:
            break;
        }
        bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
        bytesSent = bytesSent;
    }
}


void vTCPRCBin_DiscreteIO_setDIOConfiguration(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    uint8_t index;
    TCPBin_CMD_DIOConfigDirection_payload_t *pConfg;
    volatile tEEPROM_DIOCfgData *pInfo;
    tEEPROM_DIOCfgData info;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_DIOConfig_payload_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_DiscreteIO_setDIOConfigurationResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
    pFrame->retParamID = 4;

    TCPBin_CMD_DIOConfig_payload_t *pFramePayload = (TCPBin_CMD_DIOConfig_payload_t *)pFrame->payload;
    pConfg = (TCPBin_CMD_DIOConfigDirection_payload_t *)payload;
    index = pConfg->index;
    pInfo = psEEPDIOConfg_get();
    info = *pInfo;

    if (index < TCPRCBin_DiscreteIO_DIOIndex_Count) {
        pFramePayload->index = index;

        switch (pConfg->pinConfig & GPIO_CFG_IO_MASK) {
        case GPIO_CFG_IN_NOPULL:
        case GPIO_CFG_IN_PU:
        case GPIO_CFG_IN_PD:
            GPIO_setConfig(g_TCPRCBin_DiscreteIO_GPIMapTable[index], GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
            info.dioCfg[index] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES;
            vEEPDIOConfg_set(&info);
            pFramePayload->pinConfig = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES;
            pFramePayload->DIVal = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIVal;
            pFramePayload->DIDebounceDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIDebounceDly;
            pFramePayload->DIRptStartDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIRptStartDly;
            pFramePayload->DIRptDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIRptDly;
            pFramePayload->DIIn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIIn;
            pFramePayload->DIBypassEn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIBypassEn;
            pFramePayload->DIModeSel = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIModeSel;
            pFramePayload->DIDebounceEn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIDebounceEn;
            break;
        case GPIO_CFG_OUT_STD:
        case GPIO_CFG_OUT_OD_NOPULL:
        case GPIO_CFG_OUT_OD_PU:
        case GPIO_CFG_OUT_OD_PD:
            GPIO_setConfig(g_TCPRCBin_DiscreteIO_GPOMapTable[index], GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
            info.dioCfg[index] = GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW;
            vEEPDIOConfg_set(&info);
            pFramePayload->pinConfig = GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW;
            pFramePayload->DOA = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOA;
            pFramePayload->DOB = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOB;
            pFramePayload->DOBCtr = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBCtr;
            pFramePayload->DOOut = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOOut;
            pFramePayload->DOCtrl = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOCtrl;
            pFramePayload->DOBypass = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBypass;
            pFramePayload->DOBypassEn = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBypassEn;
            pFramePayload->DOModeSel = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOModeSel;
            pFramePayload->DOBlinkEnSel = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBlinkEnSel;
            pFramePayload->DOInv = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOInv;
            break;
        default:
            break;
        }
        bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
        bytesSent = bytesSent;
    }
}



