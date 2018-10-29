/*
 * TCPBinaryCMD_ALTOMultinet.h
 *
 *  Created on: Jun 1, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_ALTOMULTINET_TCPBINARYCMD_ALTOMULTINET_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_ALTOMULTINET_TCPBINARYCMD_ALTOMULTINET_H_




typedef struct {
    uint32_t deviceID;
    ALTOMultinetDeviceRelay_getStatusRelay payload;
}TCP_CMD_ALTOMultinet_getRelay_payload_t;

typedef struct {
    uint32_t deviceID;
    ALTOMultinetDevice_multinetAddress payload;
}TCP_CMD_ALTOMultinet_getPot_payload_t;


typedef struct {
    uint32_t deviceID;
    int8_t ack;
}TCP_CMD_ALTOMultinet_setInput_payload_t;

typedef struct {
    uint32_t deviceID;
    ALTOMultinetDeviceRelay_setSwitchOnOff payload;
}TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t;


//Characteristic defines
typedef enum
{
    CHARACTERISTIC_ALTO_MULTINET_REMOTE_DETAILED_STATUS_GET_ID = 0x01,
    CHARACTERISTIC_ALTO_MULTINET_REMOTE_STATUS_RELAY_GET_ID,
    CHARACTERISTIC_ALTO_MULTINET_REMOTE_LOCALREMOTE_SET_ID,
} TCP_CMD_ALTOMultinet_service_detailedStatus_characteristics_t;


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  __TCPBINARYCOMMANDS_ALTOMULTINET_GLOBAL
    #define __TCPBINARYCOMMANDS_ALTOMULTINET_EXT
#else
    #define __TCPBINARYCOMMANDS_ALTOMULTINET_EXT    extern
#endif


//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************
__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void vTCPRCBin_ALTOMultinetGetDetailedStatusReturnService_ValueChangeHandler(char_data_t *pCharData);

__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void TCPBin_ALTOMultinetRelayGet(int clientfd, char *payload, int32_t size);

__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void TCPBin_ALTOMultinetSetSwitchOnOff(int clientfd, char *payload, int32_t size);

__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void TCPBin_ALTOMultinetTriggerLocalRemote(int clientfd, char *payload, int32_t size);

__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void TCPBin_ALTOMultinetGetGate(int clientfd, char *payload, int32_t size);

__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void TCPBin_ALTOMultinetSetGate(int clientfd, char *payload, int32_t size);

__TCPBINARYCOMMANDS_ALTOMULTINET_EXT
void TCPBin_ALTOMultinetPotGet(int clientfd, char *payload, int32_t size);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_ALTOMULTINET_TCPBINARYCMD_ALTOMULTINET_H_ */
