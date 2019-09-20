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
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(g_TFUartTestErrorCounter) + 2*sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_TEST_getCountersResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;

    for (index = 0; index < IF_COUNT; index++) pFramePayload[index] = g_TFUartTestErrorCounter[index];
    pFramePayload[index++] = g_ui32CANTestFrameError0;
    pFramePayload[index++] = g_ui32CANTestFrameError1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_ProductionTest_resetCounters(int clientfd, char *payload, int32_t size)
{
    int bytesSent, index;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_TEST_resetCountersResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;

    for (index = 0; index < IF_COUNT; index++) g_TFUartTestErrorCounter[index] = 0;
    g_ui32CANTestFrameError0 = 0;
    g_ui32CANTestFrameError1 = 0;
    *pFramePayload = 1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}

void vTCPRCBin_ProductionTest_setGPIOAsInput(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_TEST_setGPIOAsInputResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;



    DISetDebounceEn (DIO_GPI_0, false);
    DISetDebounceEn (DIO_GPI_1, false);
    DISetDebounceEn (DIO_GPI_2, false);
    DISetDebounceEn (DIO_GPI_3, false);
    DISetDebounceEn (DIO_GPI_4, false);
    DISetDebounceEn (DIO_GPI_5, false);

    DICfgMode(DIO_GPI_0, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_1, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_2, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_3, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_4, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_5, DI_MODE_DIRECT);

    GPIO_setConfig(Board_GPI_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    GPIO_setConfig(Board_GPI_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    GPIO_setConfig(Board_GPI_2, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    GPIO_setConfig(Board_GPI_3, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    GPIO_setConfig(Board_GPI_4, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    GPIO_setConfig(Board_GPI_5, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);


    *pFramePayload = 1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}

void vTCPRCBin_ProductionTest_setGPIOAsOutput(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_TEST_setGPIOAsOutputResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;



    DOCfgMode (DIO_GPO_0, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_1, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_2, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_3, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_4, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_5, DO_MODE_DIRECT, false);

    GPIO_setConfig(Board_GPI_0, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPI_1, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPI_2, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPI_3, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPI_4, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPI_5, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);

    *pFramePayload = 1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}

void vTCPRCBin_ProductionTest_setGPIOOutputValue(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_TEST_setGPIOOutputValueResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;

    uint32_t *pValue = (uint32_t *)payload;

    if(*pValue) {
        DOSet (DIO_GPO_0, true);
        DOSet (DIO_GPO_1, true);
        DOSet (DIO_GPO_2, true);
        DOSet (DIO_GPO_3, true);
        DOSet (DIO_GPO_4, true);
        DOSet (DIO_GPO_5, true);
    }else {
        DOSet (DIO_GPO_0, false);
        DOSet (DIO_GPO_1, false);
        DOSet (DIO_GPO_2, false);
        DOSet (DIO_GPO_3, false);
        DOSet (DIO_GPO_4, false);
        DOSet (DIO_GPO_5, false);
    }

    *pFramePayload = 1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}

void vTCPRCBin_ProductionTest_getGPIOInputValue(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + 6 * sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_TEST_getGPIOInputValueResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    uint32_t *pFramePayload = (uint32_t *)pFrame->payload;

    for(bytesSent = 0; bytesSent < 6; bytesSent++) {
        pFramePayload[bytesSent] = DIGet(DIO_GPI_0 + bytesSent);
    }


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


