/*
 * TCPBinaryCMD_AVDS.c
 *
 *  Created on: Mar 30, 2020
 *      Author: epf
 */


#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_GLOBAL
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


int xTCPRCBin_AVDS_directCommand_ValueChangeHandler(char_data_t *pCharData);


void vTCPRCBin_AVDSService_ValueChangeHandler(char_data_t *pCharData)
{
    switch (pCharData->paramID) {
    case CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND1_ID:
    case CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND2_ID:
        if(pCharData->dataLen) {
            xTCPRCBin_AVDS_directCommand_ValueChangeHandler(pCharData);
        }
        break;
    default:
        break;
    }
}

void vTCPRCBin_AVDS_errorHandler(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_error_payload_t) + sizeof(TCP_CMD_ALTOAmp_error_msg_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->type = TCP_CMD_error | 0x80000000;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    TCPBin_CMD_error_payload_t *pFramePayload = (TCPBin_CMD_error_payload_t *)pFrame->payload;
    TCP_CMD_ALTOAmp_error_msg_t *pDataPayload = (TCP_CMD_ALTOAmp_error_msg_t *)pCharData->data;
    TCP_CMD_ALTOAmp_error_msg_t *pMsgPayload = (TCP_CMD_ALTOAmp_error_msg_t *)pFramePayload->payload;

    pFramePayload->errorType = TCP_CMD_ERROR_ALTOCommunication;
    pMsgPayload->errorDescription = pDataPayload->errorDescription;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}

int xTCPRCBin_AVDS_directCommand_ValueChangeHandler(char_data_t *pCharData)
{
    Error_Block eb;
    int bytesSent = 0;

    Error_init(&eb);

    if (pCharData->dataLen < AVDS_MAXIMUM_PACKET_DATA_LENGHT) {
        char *buffer = Memory_alloc(NULL, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen, 0, &eb);
        //    char buffer[sizeof(TCPBin_CMD_retFrame_t) + 64];
        if (buffer) {
            TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
            pFrame->retDeviceID = pCharData->retDeviceID;
            pFrame->retSvcUUID = pCharData->retSvcUUID;
            pFrame->retParamID = pCharData->retParamID;

            switch(pCharData->retSvcUUID) {
            case SERVICE_AVDS_ALTO_EMULATOR_UUID:
                switch(pCharData->paramID) {
                case CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND1_ID:
                    pFrame->type = TCP_CMD_AVDS_DirectCommand1Response | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND2_ID:
                    pFrame->type = TCP_CMD_AVDS_DirectCommand2Response | 0x80000000;
                    break;
                default:
                    pFrame->type = 0 | 0x80000000;
                    break;
                }
                break;
                default:
                    pFrame->type = 0 | 0x80000000;
                    break;
            }

            memcpy(pFrame->payload, pCharData->data, pCharData->dataLen);
            bytesSent = send(pCharData->connHandle, buffer, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen, 0);

            Memory_free(NULL, buffer, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen);
        }
    }
    return bytesSent;
}


/*****************************************************************************************************************
 *
 */

void TCPBin_AVDSGenericDirectCommand1(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_AVDS_genericCommand_payload_t *ptPayload = (TCP_CMD_AVDS_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_AVDS_ALTO_EMULATOR_UUID, CHARACTERISTIC_AVDS_ALTO_EMULATOR_DIRECT_COMMAND_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND1_ID,
                                 (uint8_t *)ptPayload->data, ptPayload->dataLen )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_AVDS_DirectCommand1, ptPayload->deviceID);
    }
}


void TCPBin_AVDSGenericDirectCommand2(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_AVDS_genericCommand_payload_t *ptPayload = (TCP_CMD_AVDS_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_AVDS_ALTO_EMULATOR_UUID, CHARACTERISTIC_AVDS_ALTO_EMULATOR_DIRECT_COMMAND_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND2_ID,
                                 (uint8_t *)ptPayload->data, ptPayload->dataLen )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_AVDS_DirectCommand2, ptPayload->deviceID);
    }
}


