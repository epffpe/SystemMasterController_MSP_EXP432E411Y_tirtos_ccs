/*
 * TCPBinaryCMD_RAWCharacteristicData.c
 *
 *  Created on: Aug 9, 2018
 *      Author: epenate
 */

#define __TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_GLOBAL
#include "includes.h"

void vTCPRCBin_RAWCharacteristicData_returnMsg(char_data_t *pCharData);


void vTCPRCBin_RAWCharacteristicDataReturnService_ValueChangeHandler(char_data_t *pCharData)
{
    switch (pCharData->paramID) {
    case CHARACTERISTIC_RAWCHARACTERISTICDATA_RESPONSE_ID:
        if(pCharData->dataLen) {
            vTCPRCBin_RAWCharacteristicData_returnMsg(pCharData);
        }
    default:
        break;
    }
}


void vTCPRCBin_RAWCharacteristicData_returnMsg(char_data_t *pCharData)
{
    int bytesSent;

    Error_Block eb;

    ASSERT(pCharData != NULL);

    if (pCharData == NULL) {
        return;
    }

    char *buffer = Memory_alloc(NULL, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen, 0, &eb);

    if (buffer != NULL) {
        TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;
        pFrame->retDeviceID = pCharData->retDeviceID;
        pFrame->retSvcUUID = pCharData->retSvcUUID;
        pFrame->retParamID = pCharData->retParamID;

        pFrame->type = TCP_CMD_RAW_CharDataMsg_Response | 0x80000000;

        memcpy(pFrame->payload, pCharData->data, pCharData->dataLen);

        bytesSent = send(pCharData->connHandle, buffer, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen, 0);

        bytesSent = bytesSent;

        Memory_free(NULL, buffer, sizeof(TCPBin_CMD_retFrame_t) + pCharData->dataLen);

    }

}


void TCPBin_RAWCharacteristicData(int clientfd, char *payload, int32_t size)
{
    TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *)payload;

    if (!xDevice_sendCharDataMsg( ptPayload->deviceID,
                                  APP_MSG_SERVICE_WRITE,
                                  clientfd,
                                  SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID, CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_GATE_ID,
                                  TCPRCBINDEVICE_ID,
                                  SERVICE_TCPBIN_REMOTECONTROL_RAWCHARACTERISTICDATA_CLASS_RETURN_UUID, CHARACTERISTIC_RAWCHARACTERISTICDATA_RESPONSE_ID,
                                  (uint8_t *)&ptPayload->payload, sizeof(ALTOMultinetDeviceRelay_setSwitchOnOff) )) {

        TCPBin_sendDeviceIdError(clientfd, TCP_CMD_ALTOMultinet_setInput, ptPayload->deviceID);
    }
}






