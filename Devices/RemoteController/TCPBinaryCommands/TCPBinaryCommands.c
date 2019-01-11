/*
 * TCPBinaryCommands.c
 *
 *  Created on: May 7, 2018
 *      Author: epenate
 */
#define __TCPBINARYCOMMANDS_GLOBAL
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


void TCPBin_doNothing(int clientfd, char *payload, int32_t size)
{

}


/****************************************************************/

/****************************************************************/

static const TCPBin_cmdTypeDef g_apfTCPBinCmdTable[] =
{
// [TCP_CMD_doNothing]        = TCPBin_doNothing,

// ALTO Ampliefier

 [TCP_CMD_RAW_CharDataMsg] = TCPBin_RAWCharacteristicData,
 [TCP_CMD_getALTOAmpInputSelect] = TCPBin_ALTOAmpInputSelectGet,
 [TCP_CMD_setALTOAmpInputSelect] = TCPBin_ALTOAmpInputSelectSet,
 [TCP_CMD_getALTOAmpVolume] = TCPBin_ALTOAmpVolumeGet,
 [TCP_CMD_setALTOAmpVolume] = TCPBin_ALTOAmpVolumeSet,
 [TCP_CMD_getALTOAmpBass] = TCPBin_ALTOAmpBassGet,
 [TCP_CMD_setALTOAmpBass] = TCPBin_ALTOAmpBassSet,
 [TCP_CMD_getALTOAmpTreble] = TCPBin_ALTOAmpTrebleGet,
 [TCP_CMD_setALTOAmpTreble] = TCPBin_ALTOAmpTrebleSet,
 [TCP_CMD_getALTOAmpMute] = TCPBin_ALTOAmpMuteGet,
 [TCP_CMD_setALTOAmpMute] = TCPBin_ALTOAmpMuteSet,
 [TCP_CMD_getALTOAmpCompressor] = TCPBin_ALTOAmpCompressorGet,
 [TCP_CMD_setALTOAmpCompressor] = TCPBin_ALTOAmpCompressorSet,
 [TCP_CMD_getALTOAmpLoudness] = TCPBin_ALTOAmpLoudnessGet,
 [TCP_CMD_setALTOAmpLoudness] = TCPBin_ALTOAmpLoudnessSet,

 // ALTO Amplifier Headphones
 [TCP_CMD_getALTOAmpHPInputSelect] = TCPBin_ALTOAmpHPInputSelectGet,
 [TCP_CMD_setALTOAmpHPInputSelect] = TCPBin_ALTOAmpHPInputSelectSet,

 [TCP_CMD_getALTOAmpHPVolume] = TCPBin_ALTOAmpHPVolumeGet,
 [TCP_CMD_setALTOAmpHPVolume] = TCPBin_ALTOAmpHPVolumeSet,

 [TCP_CMD_getALTOAmpHPMute] = TCPBin_ALTOAmpHPMuteGet,
 [TCP_CMD_setALTOAmpHPMute] = TCPBin_ALTOAmpHPMuteSet,

// ALTO Relay boxes
 [TCP_CMD_ALTOMultinet_getRelay] = TCPBin_ALTOMultinetRelayGet,
 [TCP_CMD_ALTOMultinet_setInput] = TCPBin_ALTOMultinetSetSwitchOnOff,

 [TCP_CMD_ALTOMultinet_TriggerLocalRemote] = TCPBin_ALTOMultinetTriggerLocalRemote,
 [TCP_CMD_ALTOMultinet_getGate] = TCPBin_ALTOMultinetGetGate,
 [TCP_CMD_ALTOMultinet_setGate] = TCPBin_ALTOMultinetSetGate,

 [TCP_CMD_System_getMonitorStatus] = vTCPRCBin_SystemControl_getOverAllStatus,
 [TCP_CMD_System_getCompiledTime] = vTCPRCBin_SystemControl_getCompiledTime,
 [TCP_CMD_System_getRAMDeviceList] = vTCPRCBin_SystemControl_getRAMDeviceList,
 [TCP_CMD_System_getFlashDeviceList] = vTCPRCBin_SystemControl_getFlashDeviceList,
 [TCP_CMD_System_getFlashFileData] = vTCPRCBin_SystemControl_getFlashDataForFileName,
 [TCP_CMD_System_setFlashFileData] = vTCPRCBin_SystemControl_setFlashDataForFileName,
 [TCP_CMD_System_deleteFlashFileData] = vTCPRCBin_SystemControl_deleteFlashDataForFileName,

 [TCP_CMD_System_getManufacturerInformationData] = vTCPRCBin_SystemControl_getManufacturerInformationData,
 [TCP_CMD_System_setManufacturerInformationData] = vTCPRCBin_SystemControl_setManufacturerInformationData,



 [TCP_CMD_DiscreteIO_get5VOutputPowerStatus] = vTCPRCBin_DiscreteIO_get5VOutputPowerStatus,
 [TCP_CMD_DiscreteIO_turnOn5VOutputPower] = vTCPRCBin_DiscreteIO_turnOn5VOutputPower,
 [TCP_CMD_DiscreteIO_turnOff5VOutputPower] = vTCPRCBin_DiscreteIO_turnOff5VOutputPower,
 [TCP_CMD_DiscreteIO_set5VOutputPower] = vTCPRCBin_DiscreteIO_set5VOutputPower,



 [TCP_CMD_DiscreteIO_getDIOConfiguration] = vTCPRCBin_DiscreteIO_getDIOConfiguration,
 [TCP_CMD_DiscreteIO_setDIOConfiguration] = vTCPRCBin_DiscreteIO_setDIOConfiguration,
// [TCP_CMD_DiscreteIO_setDIConfiguration] = vTCPRCBin_DiscreteIO_setDIConfiguration,
// [TCP_CMD_DiscreteIO_setDOConfiguration] = vTCPRCBin_DiscreteIO_setDOConfiguration,


// ALTO Temp boxes
 [TCP_CMD_ALTOMultinet_getPot] = TCPBin_ALTOMultinetPotGet,


};
#define TCPBIN_CMD_COUNT   (sizeof (g_apfTCPBinCmdTable) / sizeof (g_apfTCPBinCmdTable[0]))



void TCPBin_decode(int clientfd, char *payload, int32_t size)
{
    unsigned int ui32Type;
    int  bytesSent;
    TCPBin_CMD_error_payload_t *pTCPErrorPayload;
    TCPBin_CMD_t *pTCPBinCmd = (TCPBin_CMD_t *) payload;

    char cmdbuf[64];
    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)cmdbuf;
    ui32Type = pTCPBinCmd->type;


    if (ui32Type & 0x80000000UL){
        ui32Type &= 0x00FFFFFFFUL;
        if ( ui32Type < TCPBIN_CMD_COUNT) {
            if (g_apfTCPBinCmdTable[ui32Type]){
                g_apfTCPBinCmdTable[ui32Type](clientfd, pTCPBinCmd->payload, size - 4);
            }else {
                //Command not implemented
                pFrame->type = TCP_CMD_error | 0x80000000;
                pFrame->retDeviceID = TCPRCBINDEVICE_ID;
                pFrame->retSvcUUID = 0;
                pFrame->retParamID = 0;
                pTCPErrorPayload = (TCPBin_CMD_error_payload_t *)pFrame->payload;
                pTCPErrorPayload->errorType = TCP_CMD_ERROR_commandNotImplemented;

//                n = sprintf(cmdbuf, "Error: Command type not implemented\n\r");
                bytesSent = send(clientfd, cmdbuf, (sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_error_payload_t)), 0);
                if (bytesSent < 0)  {
                    System_printf("Error: Command type not implemented\n");
                    System_flush();
                }
            }

        }else{
//            n = sprintf(cmdbuf, "Error: Command type out of range\n\r");
            pFrame->type = TCP_CMD_error | 0x80000000;
            pFrame->retDeviceID = TCPRCBINDEVICE_ID;
            pFrame->retSvcUUID = 0;
            pFrame->retParamID = 0;
            pTCPErrorPayload = (TCPBin_CMD_error_payload_t *)pFrame->payload;
            pTCPErrorPayload->errorType = TCP_CMD_ERROR_commandTypeOutOfRange;
            bytesSent = send(clientfd, cmdbuf, (sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_error_payload_t)), 0);
            if (bytesSent < 0)  {
                System_printf("Error: Kind out of range\n");
                System_flush();
            }
        }
    }else{
//        n = sprintf(cmdbuf, "Error: Frame not recognized\n\r");
        pFrame->type = TCP_CMD_error | 0x80000000;
        pFrame->retDeviceID = TCPRCBINDEVICE_ID;
        pFrame->retSvcUUID = 0;
        pFrame->retParamID = 0;
        pTCPErrorPayload = (TCPBin_CMD_error_payload_t *)pFrame->payload;
        pTCPErrorPayload->errorType = TCP_CMD_ERROR_commandFrameNotRecognized;
        bytesSent = send(clientfd, cmdbuf, (sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_error_payload_t)), 0);
        if (bytesSent < 0)  {
            System_printf("Error: Frame not recognized\n");
            System_flush();
        }
    }

}

void TCPBin_sendDeviceIdError(int32_t clientfd, TCPBin_CMD_types_te cmdType, uint32_t deviceID)
{
    char cmdbuf[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_error_payload_t) + sizeof(TCPBin_CMD_errorDeviceID_payload_t)];
    int  bytesSent;
    TCPBin_CMD_retFrame_t *pTCPBinCmd;
    TCPBin_CMD_error_payload_t *pTCPErrorPayload;
    TCPBin_CMD_errorDeviceID_payload_t *pDeviceIdError;

    pTCPBinCmd = (TCPBin_CMD_retFrame_t *) cmdbuf;
    pTCPErrorPayload = (TCPBin_CMD_error_payload_t *)pTCPBinCmd->payload;
    pDeviceIdError = (TCPBin_CMD_errorDeviceID_payload_t *)pTCPErrorPayload->payload;

    pTCPBinCmd->type = TCP_CMD_error | 0x80000000;
    pTCPBinCmd->retDeviceID = TCPRCBINDEVICE_ID;
    pTCPBinCmd->retSvcUUID = 0;
    pTCPBinCmd->retParamID = 0;
    pTCPErrorPayload->errorType = TCP_CMD_ERROR_deviceIdNotFound;
    pDeviceIdError->cmdType = cmdType;
    pDeviceIdError->deviceID = deviceID;

    bytesSent = send(clientfd, cmdbuf, sizeof(cmdbuf), 0);
    if (bytesSent < 0)  {
        System_printf("Error: DeviceID not found\n");
        System_flush();
    }
}


