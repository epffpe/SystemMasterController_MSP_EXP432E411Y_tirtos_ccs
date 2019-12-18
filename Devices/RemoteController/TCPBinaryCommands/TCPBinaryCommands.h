/*
 * TCPBinaryCommands.h
 *
 *  Created on: May 7, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_TCPBINARYCOMMANDS_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_TCPBINARYCOMMANDS_H_



typedef enum {
    TCP_CMD_doNothing = 0,

    TCP_CMD_error = 1,
    TCP_CMD_RAW_CharDataMsg,
    TCP_CMD_RAW_CharDataMsg_Response,

    TCP_CMD_getHeartbeat = 10,
    TCP_CMD_getHeartbeatResponse,

    TCP_CMD_getInputsNames = 20,
    TCP_CMD_getRelayOutputNames,
    TCP_CMD_getStatusOutputNames,
    TCP_CMD_getSequenceNames,

    TCP_CMD_setLED0 = 30,
    TCP_CMD_getInputDescription,
    TCP_CMD_getRelaysOutputDescription,
    TCP_CMD_getSequeceDescription,


    TCP_CMD_setFunctionForInput = 50,
    TCP_CMD_setTargetRelaysForInput,
    TCP_CMD_setTargetStatusForInput,
    TCP_CMD_setParam0ForInput,
    TCP_CMD_setParam1ForInput,
    TCP_CMD_setHoldDlyForInput,
    TCP_CMD_setPullUpForInput,
    TCP_CMD_setPriorityForInput,
    TCP_CMD_setOverwriteForInput,
    TCP_CMD_setBypassEnForInput,
    TCP_CMD_setBypassForInput,
    TCP_CMD_setDebounceEnForInput,
    TCP_CMD_setCfgDebounceForInput,
    TCP_CMD_setPullUpVoltageForInputA,
    TCP_CMD_setDICfgModeForInput,

    TCP_CMD_setDOCfgModeForOutput = 70,
    TCP_CMD_setDOSetBypassEnForOutput,
    TCP_CMD_setDOSetBypassForOutput,
    TCP_CMD_setDOCfgBlinkForOutput,
    TCP_CMD_setDOSetSyncCtrMax,

    TCP_CMD_setSequenceStepConfigure = 80,

// Amplifier Class
    TCP_CMD_getALTOAmpInputSelect = 100,
    TCP_CMD_getALTOAmpInputSelectResponse = 101,
    TCP_CMD_setALTOAmpInputSelect = 102,
    TCP_CMD_setALTOAmpInputSelectResponse,

    TCP_CMD_getALTOAmpVolume = 104,
    TCP_CMD_getALTOAmpVolumeResponse,
    TCP_CMD_setALTOAmpVolume,
    TCP_CMD_setALTOAmpVolumeResponse,

    TCP_CMD_getALTOAmpBass = 108,
    TCP_CMD_getALTOAmpBassResponse,
    TCP_CMD_setALTOAmpBass,
    TCP_CMD_setALTOAmpBassResponse,

    TCP_CMD_getALTOAmpTreble = 112,
    TCP_CMD_getALTOAmpTrebleResponse,
    TCP_CMD_setALTOAmpTreble,
    TCP_CMD_setALTOAmpTrebleResponse,

    TCP_CMD_getALTOAmpMute = 116,
    TCP_CMD_getALTOAmpMuteResponse,
    TCP_CMD_setALTOAmpMute,
    TCP_CMD_setALTOAmpMuteResponse,

    TCP_CMD_getALTOAmpCompressor = 120,
    TCP_CMD_getALTOAmpCompressorResponse,
    TCP_CMD_setALTOAmpCompressor,
    TCP_CMD_setALTOAmpCompressorResponse,

    TCP_CMD_getALTOAmpLoudness = 124,
    TCP_CMD_getALTOAmpLoudnessResponse,
    TCP_CMD_setALTOAmpLoudness,
    TCP_CMD_setALTOAmpLoudnessResponse,

// Headphone Class
    TCP_CMD_getALTOAmpHPInputSelect = 128,
    TCP_CMD_getALTOAmpHPInputSelectResponse,
    TCP_CMD_setALTOAmpHPInputSelect,
    TCP_CMD_setALTOAmpHPInputSelectResponse,

    TCP_CMD_getALTOAmpHPVolume = 132,
    TCP_CMD_getALTOAmpHPVolumeResponse,
    TCP_CMD_setALTOAmpHPVolume,
    TCP_CMD_setALTOAmpHPVolumeResponse,

    TCP_CMD_getALTOAmpHPMute = 136,
    TCP_CMD_getALTOAmpHPMuteResponse,
    TCP_CMD_setALTOAmpHPMute,
    TCP_CMD_setALTOAmpHPMuteResponse,

// Light Class
    TCP_CMD_ALTOMultinet_getRelay = 150,
    TCP_CMD_ALTOMultinet_getRelayResponse,

    TCP_CMD_ALTOMultinet_setInput = 160,
    TCP_CMD_ALTOMultinet_setInputResponse,

    TCP_CMD_ALTOMultinet_TriggerLocalRemote,
    TCP_CMD_ALTOMultinet_TriggerLocalRemoteResponse,

    TCP_CMD_ALTOMultinet_getGate,
    TCP_CMD_ALTOMultinet_getGateResponse,

    TCP_CMD_ALTOMultinet_setGate,
    TCP_CMD_ALTOMultinet_setGateResponse,


// Temp Controller Class
    TCP_CMD_ALTOMultinet_getPot = 180,
    TCP_CMD_ALTOMultinet_getPotResponse,


//System Control
    TCP_CMD_System_getMonitorStatus = 200,
    TCP_CMD_System_getMonitorStatusResponse,
    TCP_CMD_System_getCompiledTime,
    TCP_CMD_System_getCompiledTimeResponse,
    TCP_CMD_System_getRAMDeviceList,
    TCP_CMD_System_getRAMDeviceListResponse,
    TCP_CMD_System_getFlashDeviceList,
    TCP_CMD_System_getFlashDeviceListResponse,
    TCP_CMD_System_getFlashFileData,
    TCP_CMD_System_getFlashFileDataResponse,
    TCP_CMD_System_setFlashFileData,
    TCP_CMD_System_setFlashFileDataResponse,
    TCP_CMD_System_deleteFlashFileData,
    TCP_CMD_System_deleteFlashFileDataResponse,

    TCP_CMD_System_getManufacturerInformationData,
    TCP_CMD_System_getManufacturerInformationDataResponse,
    TCP_CMD_System_setManufacturerInformationData,
    TCP_CMD_System_setManufacturerInformationDataResponse,


    TCP_CMD_System_getConfigurationFile,
    TCP_CMD_System_getConfigurationFileResponse,
    TCP_CMD_System_setConfigurationFile,
    TCP_CMD_System_setConfigurationFileResponse,

// Flash Memory Manager
    TCP_CMD_FlashMM_IFS_getStatus = 290,
    TCP_CMD_FlashMM_IFS_getStatusResponse,
    TCP_CMD_FlashMM_IFS_format,
    TCP_CMD_FlashMM_IFS_formatResponse,
    TCP_CMD_FlashMM_EFS_getStatus,
    TCP_CMD_FlashMM_EFS_getStatusResponse,
    TCP_CMD_FlashMM_EFS_format,
    TCP_CMD_FlashMM_EFS_formatResponse,

//DiscreteIO
    TCP_CMD_DiscreteIO_get5VOutputPowerStatus = 320,
    TCP_CMD_DiscreteIO_get5VOutputPowerStatusResponse,
    TCP_CMD_DiscreteIO_turnOn5VOutputPower,
    TCP_CMD_DiscreteIO_turnOn5VOutputPowerResponse,
    TCP_CMD_DiscreteIO_turnOff5VOutputPower,
    TCP_CMD_DiscreteIO_turnOff5VOutputPowerResponse,
    TCP_CMD_DiscreteIO_set5VOutputPower,
    TCP_CMD_DiscreteIO_set5VOutputPowerResponse,

    TCP_CMD_DiscreteIO_getDIOConfiguration,
    TCP_CMD_DiscreteIO_getDIOConfigurationResponse,
    TCP_CMD_DiscreteIO_setDIODirection,
    TCP_CMD_DiscreteIO_setDIODirectionResponse,
    TCP_CMD_DiscreteIO_setDIConfiguration,
    TCP_CMD_DiscreteIO_setDIConfigurationResponse,
    TCP_CMD_DiscreteIO_setDOConfiguration,
    TCP_CMD_DiscreteIO_setDOConfigurationResponse,

    TCP_CMD_DiscreteIO_setDOValue,
    TCP_CMD_DiscreteIO_setDOValueResponse,
    TCP_CMD_DiscreteIO_setDOToggle,
    TCP_CMD_DiscreteIO_setDOToggleResponse,

//Panasonic PCU
    TCP_CMD_Panasonic_getMonitorStatus = 340,
    TCP_CMD_Panasonic_getMonitorStatusResponse,

//Analog Output
    TCP_CMD_DAC_setVoltage = 350,
    TCP_CMD_DAC_setVoltageResponse,

//Production Test and qual
    TCP_CMD_TEST_resetCounters = 400,
    TCP_CMD_TEST_resetCountersResponse,
    TCP_CMD_TEST_getCounters,
    TCP_CMD_TEST_getCountersResponse,
    TCP_CMD_TEST_setGPIOAsInput,
    TCP_CMD_TEST_setGPIOAsInputResponse,
    TCP_CMD_TEST_setGPIOAsOutput,
    TCP_CMD_TEST_setGPIOAsOutputResponse,
    TCP_CMD_TEST_setGPIOOutputValue,
    TCP_CMD_TEST_setGPIOOutputValueResponse,
    TCP_CMD_TEST_getGPIOInputValue,
    TCP_CMD_TEST_getGPIOInputValueResponse,

    TCP_CMD_TEST_MemoryTestEEPROM,
    TCP_CMD_TEST_MemoryTestEEPROMResponse,
    TCP_CMD_TEST_MemoryTestInternalFlash,
    TCP_CMD_TEST_MemoryTestInternalFlashResponse,
    TCP_CMD_TEST_MemoryTestExternalFlash,
    TCP_CMD_TEST_MemoryTestExternalFlashResponse,

    TCP_CMD_TEST_setIROutputValue,
    TCP_CMD_TEST_setIROutputValueResponse,
    TCP_CMD_TEST_getIRInputValue,
    TCP_CMD_TEST_getIRInputValueResponse,

    TCP_CMD_COUNT
} TCPBin_CMD_types_te;


typedef enum {
    TCP_CMD_ERROR_commandNotImplemented = 1,
    TCP_CMD_ERROR_commandTypeOutOfRange,
    TCP_CMD_ERROR_commandFrameNotRecognized,
    TCP_CMD_ERROR_deviceIdNotFound,
    TCP_CMD_ERROR_ALTOCommunication,
} TCPBin_CMD_error_te;


typedef void (*TCPBin_cmdTypeDef)(int  clientfd, char *payload, int32_t size);


typedef struct {
    int type;
    char payload[];
}TCPBin_CMD_t;

typedef struct {
    int errorType;
    char payload[];
}TCPBin_CMD_error_payload_t;

typedef struct {
    uint32_t cmdType;
    uint32_t deviceID;
}TCPBin_CMD_errorDeviceID_payload_t;

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

#ifdef  __TCPBINARYCOMMANDS_GLOBAL
    #define __TCPBINARYCOMMANDS_EXT
#else
    #define __TCPBINARYCOMMANDS_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************

__TCPBINARYCOMMANDS_EXT void TCPBin_decode(int clientfd, char *par, int32_t size);
__TCPBINARYCOMMANDS_EXT void TCPBin_sendDeviceIdError(int32_t clientfd, TCPBin_CMD_types_te cmdType, uint32_t deviceID);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif



#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_TCPBINARYCOMMANDS_H_ */
