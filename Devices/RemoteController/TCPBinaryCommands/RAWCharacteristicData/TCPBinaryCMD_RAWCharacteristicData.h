/*
 * TCPBinaryCMD_RAWCharacteristicData.h
 *
 *  Created on: Aug 9, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_TCPBINARYCMD_RAWCHARACTERISTICDATA_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_TCPBINARYCMD_RAWCHARACTERISTICDATA_H_



typedef struct {
    uint32_t deviceID;
    ALTOMultinetDeviceRelay_getStatusRelay payload;
}TCP_CMD_RAWCharacteristicData_msg_payload_t;



//Characteristic defines
typedef enum
{
    CHARACTERISTIC_RAWCHARACTERISTICDATA_RESPONSE_ID = 0x01,
} TCP_CMD_RAWCharacteristicData_service_characteristics_t;



#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  __TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_GLOBAL
    #define __TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_EXT
#else
    #define __TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_EXT    extern
#endif


//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************
__TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_EXT
void vTCPRCBin_RAWCharacteristicData_returnMsg(char_data_t *pCharData);
__TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_EXT
void TCPBin_RAWCharacteristicData(int clientfd, char *payload, int32_t size);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif



#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_RAWCHARACTERISTICDATA_TCPBINARYCMD_RAWCHARACTERISTICDATA_H_ */
