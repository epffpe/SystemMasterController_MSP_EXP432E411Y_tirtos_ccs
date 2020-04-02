/*
 * TCPBinaryCMD_AVDS.h
 *
 *  Created on: Mar 30, 2020
 *      Author: epf
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_H_


#define AVDS_MAXIMUM_PACKET_DATA_LENGHT      64

//Characteristic defines
typedef enum
{
    CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND1_ID = 0x01,
    CHARACTERISTIC_TCPRCBIN_ADVS_DIRECT_COMMAND2_ID,
} TCP_CMD_AVDS_service_characteristics_t;

typedef struct {
    uint32_t deviceID;
    uint32_t dataLen;
    uint8_t  data[];
}TCP_CMD_AVDS_genericCommand_payload_t;


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_GLOBAL
    #define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_EXT
#else
    #define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_EXT    extern
#endif

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_EXT
void vTCPRCBin_AVDSService_ValueChangeHandler(char_data_t *pCharData);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_EXT
void vTCPRCBin_AVDS_errorHandler(char_data_t *pCharData);


__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_EXT
void TCPBin_AVDSGenericDirectCommand1(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_EXT
void TCPBin_AVDSGenericDirectCommand2(int clientfd, char *payload, int32_t size);




//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_AVDS_TCPBINARYCMD_AVDS_H_ */
