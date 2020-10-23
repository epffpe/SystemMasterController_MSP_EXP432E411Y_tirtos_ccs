/*
 * TCPBinaryCMD_Rosen.c
 *
 *  Created on: Apr 9, 2020
 *      Author: epffpe
 */

#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_ROSEN_TCPBINARYCMD_ROSEN_GLOBAL
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



int xTCPRCBin_ROSEN_directCommand_ValueChangeHandler(char_data_t *pCharData);
int xTCPRCBin_ROSENUDP_SM_ValueChangeHandler(char_data_t *pCharData);


void vTCPRCBin_ROSENService_ValueChangeHandler(char_data_t *pCharData)
{
    switch (pCharData->paramID) {
    case CHARACTERISTIC_TCPRCBIN_ROSEN_DIRECT_COMMAND1_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_DIRECT_COMMAND2_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_POWER_SET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_POWER_GET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SOURCE_SET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SOURCE_GET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_CONTROL_SET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_CONTROL_GET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_LOADED_GET_ID:
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_PLAYING_GET_ID:
        if(pCharData->dataLen) {
            xTCPRCBin_ROSEN_directCommand_ValueChangeHandler(pCharData);
        }
        break;
    case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SM_SOURCE_GET_ID:
        if(pCharData->dataLen) {
            xTCPRCBin_ROSENUDP_SM_ValueChangeHandler(pCharData);
        }
        break;
    default:
        break;
    }
}

void vTCPRCBin_ROSEN_errorHandler(char_data_t *pCharData)
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

int xTCPRCBin_ROSEN_directCommand_ValueChangeHandler(char_data_t *pCharData)
{
    Error_Block eb;
    int bytesSent = 0;

    Error_init(&eb);

    if (pCharData->dataLen <= sizeof(Rosen485Device_SteveCommandReturn1Data_t)) {
        char *buffer = Memory_alloc(NULL, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen, 0, &eb);
        //    char buffer[sizeof(TCPBin_CMD_retFrame_t) + 64];
        if (buffer) {
            TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
            pFrame->retDeviceID = pCharData->retDeviceID;
            pFrame->retSvcUUID = pCharData->retSvcUUID;
            pFrame->retParamID = pCharData->retParamID;

            switch(pCharData->retSvcUUID) {
            case SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID:
                switch(pCharData->paramID) {
                case CHARACTERISTIC_TCPRCBIN_ROSEN_DIRECT_COMMAND1_ID:
                    pFrame->type = TCP_CMD_ROSEN_DirectCommand1Response | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_DIRECT_COMMAND2_ID:
                    pFrame->type = TCP_CMD_ROSEN_DirectCommand2Response | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_POWER_SET_ID:
                    pFrame->type = TCP_CMD_ROSEN_PowerSetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_POWER_GET_ID:
                    pFrame->type = TCP_CMD_ROSEN_PowerGetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SOURCE_SET_ID:
                    pFrame->type = TCP_CMD_ROSEN_SourceSetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SOURCE_GET_ID:
                    pFrame->type = TCP_CMD_ROSEN_SourceGetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_CONTROL_SET_ID:
                    pFrame->type = TCP_CMD_ROSEN_DVDControlSetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_CONTROL_GET_ID:
                    pFrame->type = TCP_CMD_ROSEN_DVDControlGetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_LOADED_GET_ID:
                    pFrame->type = TCP_CMD_ROSEN_DVDLoadedGetResponse | 0x80000000;
                    break;
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_PLAYING_GET_ID:
                    pFrame->type = TCP_CMD_ROSEN_DVDPlayingGetResponse | 0x80000000;
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


int xTCPRCBin_ROSENUDP_SM_ValueChangeHandler(char_data_t *pCharData)
{
    Error_Block eb;
    int bytesSent = 0;

    Error_init(&eb);

    if (pCharData->dataLen <= sizeof(RosenUDPDevice_SteveCommandData_t)) {
        char *buffer = Memory_alloc(NULL, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen, 0, &eb);
        //    char buffer[sizeof(TCPBin_CMD_retFrame_t) + 64];
        if (buffer) {
            TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
            pFrame->retDeviceID = pCharData->retDeviceID;
            pFrame->retSvcUUID = pCharData->retSvcUUID;
            pFrame->retParamID = pCharData->retParamID;

            switch(pCharData->retSvcUUID) {
            case SERVICE_ROSEN_UDP_STEVE_COMMANDS_UUID:
                switch(pCharData->paramID) {
                case CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SM_SOURCE_GET_ID:
                    pFrame->type = TCP_CMD_ROSEN_UDP_SM_SOURCE_GETResponse | 0x80000000;
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

void TCPBin_ROSENGenericDirectCommand1(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_DIRECT_COMMAND1_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_directCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_AVDS_DirectCommand1, ptPayload->deviceID);
    }
}


void TCPBin_ROSENGenericDirectCommand2(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_DIRECT_COMMAND2_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_directCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_AVDS_DirectCommand2, ptPayload->deviceID);
    }
}

void TCPBin_ROSENPowerSetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_POWER_SET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_PowerSet, ptPayload->deviceID);
    }
}


void TCPBin_ROSENPowerGetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_POWER_GET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_PowerGet, ptPayload->deviceID);
    }
}


void TCPBin_ROSENSourceSetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SOURCE_SET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_SourceSet, ptPayload->deviceID);
    }
}

void TCPBin_ROSENSourceGetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SOURCE_GET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_SourceGet, ptPayload->deviceID);
    }
}




void TCPBin_ROSENDVDControlSetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_CONTROL_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_CONTROL_SET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_DVDControlSet, ptPayload->deviceID);
    }
}

void TCPBin_ROSENDVDPowerGetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_POWER_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_CONTROL_GET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_DVDPowerGet, ptPayload->deviceID);
    }
}




void TCPBin_ROSENDVDLoadedGetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_LOADED_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_LOADED_GET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_DVDLoadedGet, ptPayload->deviceID);
    }
}




void TCPBin_ROSENDVDPlayingGetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_PLAYING_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_DVD_PLAYING_GET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(Rosen485Device_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ROSEN_DVDPlayingGet, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ROSENUDPSMSourceSetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSEN_UDP_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SM_SOURCE_SET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(RosenUDPDevice_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_AVDS_DirectCommand2, ptPayload->deviceID);
    }
}

void TCPBin_ROSENUDPSMSourceGetCommand(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ROSEN_genericCommand_payload_t *ptPayload = (TCP_CMD_ROSEN_genericCommand_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ROSEN_UDP_STEVE_COMMANDS_UUID, CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_ROSEN_SERVICE_RETURN_UUID, CHARACTERISTIC_TCPRCBIN_ROSEN_STEVE_COMMAND_SM_SOURCE_GET_ID,
                                 (uint8_t *)ptPayload->data, sizeof(RosenUDPDevice_SteveCommandData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_AVDS_DirectCommand2, ptPayload->deviceID);
    }
}

