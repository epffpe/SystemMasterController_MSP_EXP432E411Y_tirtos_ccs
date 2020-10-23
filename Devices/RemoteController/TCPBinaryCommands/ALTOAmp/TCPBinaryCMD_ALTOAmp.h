/*
 * TCPBinaryCMD_ALTOAmp.h
 *
 *  Created on: May 8, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_TCPBINARYCMD_ALTOAMP_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_TCPBINARYCMD_ALTOAMP_H_


//Characteristic defines
typedef enum
{
    CHARACTERISTIC_ALTO_AMP_AMPLIFIER_CLASS_ID = 0x01,
    CHARACTERISTIC_ALTO_AMP_HEADPHONE_CLASS_ID,
    CHARACTERISTIC_ALTO_AMP_DIRECT_COMMAND_CLASS_ID,
} TCP_CMD_ALTOAmp_service_characteristics_t;

typedef struct {
    uint32_t deviceID;
}TCP_CMD_ALTOAmp_getVolume_payload_t;


typedef struct {
    uint32_t deviceID;
    ALTOAmp_zoneVolumeData_t volume;
}TCP_CMD_ALTOAmp_setVolume_payload_t;


typedef struct {
    uint32_t deviceID;
    uint8_t hpNum;
}TCP_CMD_ALTOAmp_getHP_payload_t;


typedef struct {
    uint32_t deviceID;
    ALTOAmp_headphoneData_t headphone;
}TCP_CMD_ALTOAmp_setHeadPhone_payload_t;

typedef struct {
    uint32_t deviceID;
    ALTOAmp_directCommandData_t directCMD;
}TCP_CMD_ALTOAmp_directCommand_payload_t;

typedef struct
{
    uint32_t    errorDescription;
} TCP_CMD_ALTOAmp_error_msg_t;


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  __TCPBINARYCOMMANDS_ALTOAMP_GLOBAL
    #define __TCPBINARYCOMMANDS_ALTOAMP_EXT
#else
    #define __TCPBINARYCOMMANDS_ALTOAMP_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void vTCPRCBin_ALTOVolumeReturnService_ValueChangeHandler(char_data_t *pCharData);

__TCPBINARYCOMMANDS_ALTOAMP_EXT
void vTCPRCBin_ALTOAmplifier_errorHandler(char_data_t *pCharData);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpInputSelectSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpInputSelectGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpVolumeSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpVolumeGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpBassSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpBassGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpTrebleSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpTrebleGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpMuteSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpMuteGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpCompressorSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpCompressorGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpLoudnessSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpLoudnessGet(int clientfd, char *payload, int32_t size);



__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpHPInputSelectSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpHPInputSelectGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpHPVolumeSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpHPVolumeGet(int clientfd, char *payload, int32_t size);


__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpHPMuteSet(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpHPMuteGet(int clientfd, char *payload, int32_t size);



__TCPBINARYCOMMANDS_ALTOAMP_EXT
void TCPBin_ALTOAmpDirectCommand1(int clientfd, char *payload, int32_t size);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_TCPBINARYCMD_ALTOAMP_H_ */
