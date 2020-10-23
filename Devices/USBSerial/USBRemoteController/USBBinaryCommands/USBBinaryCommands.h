/*
 * USBBinaryCommands.h
 *
 *  Created on: May 29, 2020
 *      Author: epffpe
 */

#ifndef DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_H_
#define DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_H_



typedef enum {
    USBBIN_CMD_doNothing = 0,

    USBBIN_CMD_Reboot = 2,
    USBBIN_CMD_RebootResponse,


    USBBIN_CMD_getFirmwareVersion = 14,
    USBBIN_CMD_getFirmwareVersionResponse,
    USBBIN_CMD_firmwareUpgrade = 16,
    USBBIN_CMD_firmwareUpgradeResponse,

    USBBIN_CMD_setMACAddress = 20,
    USBBIN_CMD_setMACAddressResponse,
    USBBIN_CMD_getMACAddress,
    USBBIN_CMD_getMACAddressResponse,
    USBBIN_CMD_setIfConfig,
    USBBIN_CMD_setIfConfigResponse,
    USBBIN_CMD_getIfConfig,
    USBBIN_CMD_getIfConfigResponse,

    USBBIN_CMD_COUNT
} USBBIN_CMD_types_te;


typedef enum {
    USBBIN_CMD_ERROR_commandNotImplemented = 1,
    USBBIN_CMD_ERROR_commandTypeOutOfRange,
    USBBIN_CMD_ERROR_commandFrameNotRecognized,
    USBBIN_CMD_ERROR_deviceIdNotFound,
} USBBIN_CMD_error_te;


typedef struct {
    int type;
    char payload[];
}USBBIN_CMD_t;

typedef struct {
    int errorType;
    char payload[];
}USBBIN_CMD_error_payload_t;

typedef struct {
    uint32_t cmdType;
    uint32_t deviceID;
}USBBIN_CMD_errorDeviceID_payload_t;


typedef struct {
    uint32_t type;
    uint32_t retDeviceID;
    uint16_t retSvcUUID; // UUID of the service
    uint16_t retParamID; // characteristic ID
    char payload[];
}USBBIN_CMD_retFrame_t;

typedef struct {
    uint32_t header;
    uint32_t kind;
    uint32_t crc;
    uint32_t eof;
}tUSBBinaryCMD_noPayload;

typedef struct {
    uint32_t header;
    uint32_t kind;
    uint32_t var;
    uint32_t crc;
    uint32_t eof;
}tUSBBinaryCMD_1Var;

typedef struct {
    uint32_t header;
    uint32_t kind;
    uint32_t var[2];
    uint32_t crc;
    uint32_t eof;
}tUSBBinaryCMD_2Var;

typedef struct {
    uint32_t header;
    uint32_t kind;
    uint32_t var[3];
    uint32_t crc;
    uint32_t eof;
}tUSBBinaryCMD_3Var;

typedef struct {
    uint32_t header;
    uint32_t kind;
    uint32_t var[3];
    uint32_t crc;
    uint32_t eof;
}tUSBBinaryCMD_4Var;


typedef void (*USBBIN_cmdTypeDef)(char *payload, int32_t size);



//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_GLOBAL
    #define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_EXT
#else
    #define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************

__DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_EXT
void vUSBBIN_decode(char *par, int32_t size);
__DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_EXT
void vUSBBIN_sendDeviceIdError(int32_t clientfd, TCPBin_CMD_types_te cmdType, uint32_t deviceID);

__DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_EXT
Void vUSBRCBinaryWorkerFxn(UArg arg0, UArg arg1);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif



#endif /* DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_H_ */
