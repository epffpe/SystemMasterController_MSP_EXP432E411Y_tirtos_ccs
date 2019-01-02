/*
 * TCPBinaryCMD_SystemControl.h
 *
 *  Created on: Nov 27, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_H_


#ifndef IFS_FILE_DESCRIPTION_LENGTH
#define IFS_FILE_DESCRIPTION_LENGTH          (64)
#endif

#ifndef IFS_FILE_NAME_LENGTH
#define IFS_FILE_NAME_LENGTH          SPIFFS_OBJ_NAME_LEN
#endif


//typedef enum {
//
//}TCPBin_CMD_SystemControl_CharacteristicID;

typedef struct {
    uint32_t numbOfDevices;
    char payload[];
}TCPBin_CMD_SystemControl_RAM_deviceList_payload_t;

typedef struct {
    uint32_t deviceID;
    uint32_t deviceType;
    char fileName[IFS_FILE_NAME_LENGTH];
    char description[IFS_FILE_DESCRIPTION_LENGTH];
}TCPBin_CMD_SystemControl_devicesList_t;

typedef struct {
    uint32_t size;
    char fileName[IFS_FILE_NAME_LENGTH];
}TCPBin_CMD_SystemControl_FlashFileName_payload_t;

typedef struct {
    uint32_t fileSize;
    char fileName[IFS_FILE_NAME_LENGTH];
    char payload[];
}TCPBin_CMD_SystemControl_FlashFileData_payload_t;

typedef struct {
    float f28Voltage;
    float f5MainVoltage;
    float f5PerVoltage;
    float fLogicCurrent;
    float fPerCurrent;
    float fMCUCurrent;
    float fDAC;
    float fADCCH[3];
    float fMCUTemp;
    float fBoardTemp;
    uint32_t ui32HeapTotalSize;
    uint32_t ui32HeapTotalFreeSize;
    uint32_t ui32HeapLargestFreeSize;
    union {
        uint32_t ui32Status;
        struct {
            uint32_t bGpio0             : 1;
            uint32_t bGpio1             : 1;
            uint32_t bGpio2             : 1;
            uint32_t bGpio3             : 1;
            uint32_t bGpio4             : 1;
            uint32_t bGpio5             : 1;
            uint32_t bGpioReserved      : 2;

            uint32_t bLed0              : 1;
            uint32_t bLed1              : 1;
            uint32_t bLed2              : 1;
            uint32_t bLed3              : 1;
            uint32_t bLed4              : 1;
            uint32_t bLedReserved       : 3;

            uint32_t b5VOutEnabled      : 1;
            uint32_t b5VOutStatus       : 1;
            uint32_t bUSBOutEnabled     : 1;
            uint32_t bSWStatus          : 1;
            uint32_t bDebugMode         : 1;
            uint32_t bSerial5Enabled    : 1;
            uint32_t bPowerPeripheralEnabled    : 1;
        };
    };
}TCPBin_CMD_SystemControl_monitorStatus_payload_t;


#ifdef  __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_GLOBAL
    #define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
#else
    #define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif


__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_getOverAllStatus(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_getRAMDeviceList(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_getFlashDeviceList(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_getCompiledTime(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_getFlashDataForFileName(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_setFlashDataForFileName(int clientfd, char *payload, int32_t size);

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_EXT
void vTCPRCBin_SystemControl_deleteFlashDataForFileName(int clientfd, char *payload, int32_t size);


#ifdef __cplusplus
}
#endif




#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_H_ */
