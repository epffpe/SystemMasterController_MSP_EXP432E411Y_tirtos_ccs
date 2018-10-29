/*
 * TCPBinaryCMD_ALTOMultinet.c
 *
 *  Created on: Jun 1, 2018
 *      Author: epenate
 */
#define __TCPBINARYCOMMANDS_ALTOMULTINET_GLOBAL
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

void vTCPRCBin_ALTOMultinet_remoteGetRelay(char_data_t *pCharData);
void vTCPRCBin_ALTOMultinet_remoteSetInput(char_data_t *pCharData);
void vTCPRCBin_ALTOMultinet_remoteGetPot(char_data_t *pCharData);

void vTCPRCBin_ALTOMultinetGetDetailedStatusReturnService_ValueChangeHandler(char_data_t *pCharData)
{
    switch (pCharData->paramID) {
    case CHARACTERISTIC_ALTO_MULTINET_REMOTE_DETAILED_STATUS_GET_ID:
        if(pCharData->dataLen) {
            switch(pCharData->retParamID) {
            case CHARACTERISTIC_ALTO_MULTINET_STATUS_RELAY_GET_ID:
                vTCPRCBin_ALTOMultinet_remoteGetRelay(pCharData);
                break;
            case CHARACTERISTIC_ALTO_MULTINET_DETAILED_POT_GET_ID:
                vTCPRCBin_ALTOMultinet_remoteGetPot(pCharData);
                break;
            default:
                break;
            }
        }
        break;
    case CHARACTERISTIC_ALTO_MULTINET_REMOTE_LOCALREMOTE_SET_ID:
        vTCPRCBin_ALTOMultinet_remoteSetInput(pCharData);
        break;
    default:
        break;
    }
}

void vTCPRCBin_ALTOMultinet_remoteGetRelay(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(ALTOMultinetDeviceRelay_getStatusRelayResponse)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID:
        pFrame->type = TCP_CMD_ALTOMultinet_getRelayResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    ALTOMultinetDeviceRelay_getStatusRelayResponse *pFramePayload = (ALTOMultinetDeviceRelay_getStatusRelayResponse *)pFrame->payload;
    ALTOMultinetDeviceRelay_getStatusRelayResponse *pDataPayload = (ALTOMultinetDeviceRelay_getStatusRelayResponse *)pCharData->data;

//    pFramePayload->ui8MultinetAddress = pDataPayload->ui8MultinetAddress;
//    pFramePayload->relayBitPosition = pDataPayload->relayBitPosition;
//    pFramePayload->relayState = pDataPayload->relayState;
    *pFramePayload = *pDataPayload;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    bytesSent = bytesSent;
}


void vTCPRCBin_ALTOMultinet_remoteGetPot(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(ALTOMultinetDeviceRelay_getDetailedStatusPotResponse)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID:
        pFrame->type = TCP_CMD_ALTOMultinet_getPotResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    ALTOMultinetDeviceRelay_getDetailedStatusPotResponse *pFramePayload = (ALTOMultinetDeviceRelay_getDetailedStatusPotResponse *)pFrame->payload;
    ALTOMultinetDeviceRelay_getDetailedStatusPotResponse *pDataPayload = (ALTOMultinetDeviceRelay_getDetailedStatusPotResponse *)pCharData->data;

    *pFramePayload = *pDataPayload;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    bytesSent = bytesSent;
}


void vTCPRCBin_ALTOMultinet_remoteSetInput(char_data_t *pCharData)
{
    int bytesSent;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(ALTOMultinetDeviceRelay_setSwitchOnOffResponse)];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
    pFrame->retDeviceID = pCharData->retDeviceID;
    pFrame->retSvcUUID = pCharData->retSvcUUID;
    pFrame->retParamID = pCharData->retParamID;

    switch(pCharData->retSvcUUID) {
    case SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID:
        pFrame->type = TCP_CMD_ALTOMultinet_setInputResponse | 0x80000000;
        break;
    default:
        pFrame->type = 0 | 0x80000000;
        break;
    }

    ALTOMultinetDeviceRelay_setSwitchOnOffResponse *pFramePayload = (ALTOMultinetDeviceRelay_setSwitchOnOffResponse *)pFrame->payload;
    ALTOMultinetDeviceRelay_setSwitchOnOffResponse *pDataPayload = (ALTOMultinetDeviceRelay_setSwitchOnOffResponse *)pCharData->data;
//    pFramePayload->deviceID = pCharData->retDeviceID;
    *pFramePayload = *pDataPayload;
    bytesSent = send(pCharData->connHandle, buffer, sizeof(buffer), 0);

    bytesSent = bytesSent;
}



void TCPBin_ALTOMultinetRelayGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_getRelay_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_getRelay_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                             APP_MSG_SERVICE_WRITE,
                             clientfd,
                             SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID, CHARACTERISTIC_ALTO_MULTINET_STATUS_RELAY_GET_ID,
                             TCPRCBINDEVICE_ID,
                             SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_MULTINET_REMOTE_DETAILED_STATUS_GET_ID,
                             (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDeviceRelay_getStatusRelay) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_getRelay, ptPayload->deviceID);
    }
}



void TCPBin_ALTOMultinetSetSwitchOnOff(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_MULTINET_REMOTE_LOCALREMOTE_SET_ID,
                                  (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOff) )) {

        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_setInput, ptPayload->deviceID);
    }
}

void TCPBin_ALTOMultinetTriggerLocalRemote(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_TRIGGER_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_MULTINET_REMOTE_LOCALREMOTE_SET_ID,
                                  (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOff) )) {

        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_setInput, ptPayload->deviceID);
    }
}

void TCPBin_ALTOMultinetGetGate(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_GET_GATE_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_MULTINET_REMOTE_LOCALREMOTE_SET_ID,
                                  (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOff) )) {

        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_setInput, ptPayload->deviceID);
    }
}

void TCPBin_ALTOMultinetSetGate(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_GATE_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_MULTINET_REMOTE_LOCALREMOTE_SET_ID,
                                  (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOff) )) {

        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_setInput, ptPayload->deviceID);
    }
}


void TCPBin_ALTOMultinetPotGet(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_getPot_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_getPot_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                             APP_MSG_SERVICE_WRITE,
                             clientfd,
                             SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID, CHARACTERISTIC_ALTO_MULTINET_DETAILED_POT_GET_ID,
                             TCPRCBINDEVICE_ID,
                             SERVICE_TCPBIN_REMOTECONTROL_ALTOMULTINET_CLASS_RETURN_UUID, CHARACTERISTIC_ALTO_MULTINET_REMOTE_DETAILED_STATUS_GET_ID,
                             (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDevice_multinetAddress) )) {
        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_getPot, ptPayload->deviceID);
    }
}


