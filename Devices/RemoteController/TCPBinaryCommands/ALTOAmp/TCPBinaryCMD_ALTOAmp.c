/*
 * TCPBinaryCMD_ALTOAmp.c
 *
 *  Created on: May 8, 2018
 *      Author: epenate
 */
#define __TCPBINARYCOMMANDS_ALTOAMP_GLOBAL
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


int xTCPRCBin_ALTOAmp_AmplifierClass_GetValueChangeHandler(char_data_t *pCharData);
int xTCPRCBin_ALTOAmp_AmplifierClass_SetValueChangeHandler(char_data_t *pCharData);
int xTCPRCBin_ALTOAmp_HeadPhoneClass_GetValueChangeHandler(char_data_t *pCharData);
int xTCPRCBin_ALTOAmp_HeadPhoneClass_SetValueChangeHandler(char_data_t *pCharData);


void vTCPRCBin_ALTOVolumeReturnService_ValueChangeHandler(char_data_t *pCharData)
{
    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID:
        if(pCharData->dataLen) {
            switch(pCharData->retParamID) {
            case CHARACTERISTIC_ALTO_AMP_GET_ID:
                xTCPRCBin_ALTOAmp_AmplifierClass_GetValueChangeHandler(pCharData);
                break;
            case CHARACTERISTIC_ALTO_AMP_SET_ID:
                xTCPRCBin_ALTOAmp_AmplifierClass_SetValueChangeHandler(pCharData);
                break;
            default:
                break;
            }
        }
        break;
    case CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID:
        if(pCharData->dataLen) {
            switch(pCharData->retParamID) {
            case CHARACTERISTIC_ALTO_AMP_GET_ID:
                xTCPRCBin_ALTOAmp_HeadPhoneClass_GetValueChangeHandler(pCharData);
                break;
            case CHARACTERISTIC_ALTO_AMP_SET_ID:
                xTCPRCBin_ALTOAmp_HeadPhoneClass_SetValueChangeHandler(pCharData);
                break;
            default:
                break;
            }
        }
        break;
    }
}


void vTCPRCBin_ALTOAmplifier_errorHandler(char_data_t *pCharData)
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

int xTCPRCBin_ALTOAmp_AmplifierClass_GetValueChangeHandler(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(ALTOAmp_volumeData_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_AMP_INPUTSELECT_UUID:
        pFrame->type = TCP_CMD_getALTOAmpInputSelectResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_VOLUME_UUID:
        pFrame->type = TCP_CMD_getALTOAmpVolumeResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_BASS_UUID:
        pFrame->type = TCP_CMD_getALTOAmpBassResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_TREBLE_UUID:
        pFrame->type = TCP_CMD_getALTOAmpTrebleResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_MUTE_UUID:
        pFrame->type = TCP_CMD_getALTOAmpMuteResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_COMPRESSOR_UUID:
        pFrame->type = TCP_CMD_getALTOAmpCompressorResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_LOUDNESS_UUID:
        pFrame->type = TCP_CMD_getALTOAmpLoudnessResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    ALTOAmp_volumeData_t *pFramePayload = (ALTOAmp_volumeData_t *)pFrame->payload;
    ALTOAmp_volumeData_t *pDataPayload = (ALTOAmp_volumeData_t *)pCharData->data;
    pFramePayload->zone1Volume = pDataPayload->zone1Volume;
    pFramePayload->zone2Volume = pDataPayload->zone2Volume;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    return bytesSent;
}

int xTCPRCBin_ALTOAmp_AmplifierClass_SetValueChangeHandler(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(char)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_AMP_INPUTSELECT_UUID:
        pFrame->type = TCP_CMD_setALTOAmpInputSelectResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_VOLUME_UUID:
        pFrame->type = TCP_CMD_setALTOAmpVolumeResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_BASS_UUID:
        pFrame->type = TCP_CMD_setALTOAmpBassResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_TREBLE_UUID:
        pFrame->type = TCP_CMD_setALTOAmpTrebleResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_MUTE_UUID:
        pFrame->type = TCP_CMD_setALTOAmpMuteResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_COMPRESSOR_UUID:
        pFrame->type = TCP_CMD_setALTOAmpCompressorResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_LOUDNESS_UUID:
        pFrame->type = TCP_CMD_setALTOAmpLoudnessResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    char *pFramePayload = (char *)pFrame->payload;
    char *pDataPayload = (char *)pCharData->data;
    *pFramePayload = *pDataPayload;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    return bytesSent;
}


int xTCPRCBin_ALTOAmp_HeadPhoneClass_GetValueChangeHandler(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(ALTOAmp_headphoneData_t)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID:
        pFrame->type = TCP_CMD_getALTOAmpHPInputSelectResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_HP_VOLUME_UUID:
        pFrame->type = TCP_CMD_getALTOAmpHPVolumeResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_HP_MUTE_UUID:
        pFrame->type = TCP_CMD_getALTOAmpHPMuteResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    ALTOAmp_headphoneData_t *pFramePayload = (ALTOAmp_headphoneData_t *)pFrame->payload;
    ALTOAmp_headphoneData_t *pDataPayload = (ALTOAmp_headphoneData_t *)pCharData->data;
    pFramePayload->hpNum = pDataPayload->hpNum;
    pFramePayload->value = pDataPayload->value;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    return bytesSent;
}


int xTCPRCBin_ALTOAmp_HeadPhoneClass_SetValueChangeHandler(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(char)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID:
        pFrame->type = TCP_CMD_setALTOAmpHPInputSelectResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_HP_VOLUME_UUID:
        pFrame->type = TCP_CMD_setALTOAmpHPVolumeResponse | 0x80000000;
        break;
    case SERVICE_ALTO_AMP_HP_MUTE_UUID:
        pFrame->type = TCP_CMD_setALTOAmpHPMuteResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    char *pFramePayload = (char *)pFrame->payload;
    char *pDataPayload = (char *)pCharData->data;
    *pFramePayload = *pDataPayload;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    return bytesSent;
}



/*****************************************************************************************************************
 *
 */


void TCPBin_ALTOAmpInputSelectSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_AMP_INPUTSELECT_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                  (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) ) ) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpInputSelect, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpInputSelectGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_AMP_INPUTSELECT_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                  (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpInputSelect, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpVolumeSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_AMP_VOLUME_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                  (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpVolume, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpVolumeGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_VOLUME_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpVolume, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpBassSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_BASS_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpBass, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpBassGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_BASS_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpBass, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpTrebleSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_TREBLE_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpTreble, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpTrebleGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_TREBLE_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpTreble, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpMuteSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_MUTE_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpMute, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpMuteGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_MUTE_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpMute, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpCompressorSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_COMPRESSOR_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpCompressor, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpCompressorGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_COMPRESSOR_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpCompressor, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpLoudnessSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_LOUDNESS_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)&ptPayload->volume, sizeof(ALTOAmp_zoneVolumeData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpLoudness, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpLoudnessGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getVolume_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getVolume_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_LOUDNESS_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID,
                                 (uint8_t *)0, 0 )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpLoudness, ptPayload->deviceID);
    }
}


/*****************************************************************************************************************
 *
 */


void TCPBin_ALTOAmpHPInputSelectSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setHeadPhone_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setHeadPhone_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
                                 (uint8_t *)&ptPayload->headphone, sizeof(ALTOAmp_headphoneData_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpHPInputSelect, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpHPInputSelectGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getHP_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getHP_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
                                 (uint8_t *)&ptPayload->hpNum, sizeof(uint8_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpHPInputSelect, ptPayload->deviceID);
    }
}

/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpHPVolumeSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setHeadPhone_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setHeadPhone_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_HP_VOLUME_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
                                 (uint8_t *)&ptPayload->headphone, sizeof(ALTOAmp_headphoneData_t))) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpHPVolume, ptPayload->deviceID);
    }
}



void TCPBin_ALTOAmpHPVolumeGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getHP_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getHP_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_HP_VOLUME_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
                                 (uint8_t *)&ptPayload->hpNum, sizeof(uint8_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpHPVolume, ptPayload->deviceID);
    }
}



/*****************************************************************************************************************
 *
 */

void TCPBin_ALTOAmpHPMuteSet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_setHeadPhone_payload_t *ptPayload = (TCP_CMD_ALTOAmp_setHeadPhone_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_HP_MUTE_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
                                 (uint8_t *)&ptPayload->headphone, sizeof(ALTOAmp_headphoneData_t))) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_setALTOAmpHPMute, ptPayload->deviceID);
    }
}


void TCPBin_ALTOAmpHPMuteGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOAmp_getHP_payload_t *ptPayload = (TCP_CMD_ALTOAmp_getHP_payload_t *)payload;
    if(!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                 APP_MSG_SERVICE_WRITE,
                                 clientfd,
                                 SERVICE_ALTO_AMP_HP_MUTE_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                 TCPRCBINDEVICE_ID,
                                 SERVICE_TCPBIN_REMOTECONTROL_AMPLIFIER_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
                                 (uint8_t *)&ptPayload->hpNum, sizeof(uint8_t) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_getALTOAmpHPMute, ptPayload->deviceID);
    }
}

