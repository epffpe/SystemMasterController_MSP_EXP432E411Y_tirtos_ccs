/*
 * TCPBinaryCMD_FlashFileSystem.c
 *
 *  Created on: Dec 17, 2019
 *      Author: epenate
 */

#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_FLASHFILESYSTEM_TCPBINARYCMD_FLASHFILESYSTEM_GLOBAL
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



void vTCPRCBin_FlashFileSystem_getInternalFlashStatus(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    SFFS_Handle hSFFS;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_FlashMM_IFS_getStatusResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    int32_t *pFramePayload = (int32_t *)pFrame->payload;

//    *pFramePayload = vIFS_getStatus();

    hSFFS = hSFFS_open(SFFS_Internal);
    *pFramePayload = xSFFS_getStatus(hSFFS, BIOS_WAIT_FOREVER);
    vSFFS_close(hSFFS);

    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_FlashFileSystem_formatInternalFlashStatus(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    SFFS_Handle hSFFS;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_FlashMM_IFS_formatResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    int32_t *pFramePayload = (int32_t *)pFrame->payload;

//    *pFramePayload = vIFS_format();

    hSFFS = hSFFS_open(SFFS_Internal);
    *pFramePayload = xSFFS_format(hSFFS, BIOS_WAIT_FOREVER);
    vSFFS_close(hSFFS);


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}

void vTCPRCBin_FlashFileSystem_getExternalFlashStatus(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    SFFS_Handle hSFFS;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_FlashMM_EFS_getStatusResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    int32_t *pFramePayload = (int32_t *)pFrame->payload;

//    *pFramePayload = vEFS_getStatus();

    hSFFS = hSFFS_open(SFFS_External);
    *pFramePayload = xSFFS_getStatus(hSFFS, BIOS_WAIT_FOREVER);
    vSFFS_close(hSFFS);


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


void vTCPRCBin_FlashFileSystem_formatExternalFlashStatus(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    SFFS_Handle hSFFS;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(uint32_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;


    pFrame->type = TCP_CMD_FlashMM_EFS_formatResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    int32_t *pFramePayload = (int32_t *)pFrame->payload;

//    *pFramePayload = vEFS_format();
    hSFFS = hSFFS_open(SFFS_External);
    *pFramePayload = xSFFS_format(hSFFS, BIOS_WAIT_FOREVER);
    vSFFS_close(hSFFS);


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


