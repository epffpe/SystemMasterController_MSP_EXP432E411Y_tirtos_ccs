/*
 * USBBinaryCommands.c
 *
 *  Created on: May 29, 2020
 *      Author: epffpe
 */

#define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_USBBINARYCOMMANDS_GLOBAL
#include "includes.h"

int xUSBRCBinary_receiveFrameData(char *pStr, unsigned int length, unsigned int timeout);

void vUSBBIN_doNothing(char *payload, int32_t size)
{

}


/****************************************************************/

/****************************************************************/

static const USBBIN_cmdTypeDef g_apfUSBBinCmdTable[] =
{
 [USBBIN_CMD_doNothing]        = vUSBBIN_doNothing,

 [USBBIN_CMD_Reboot]        = vUSBBIN_doNothing,

 [USBBIN_CMD_getFirmwareVersion] = vUSBBinaryCMD_getFirmwareVersion,
 [USBBIN_CMD_firmwareUpgrade] = vUSBBinaryCMD_enterProgrammingMode,


 [USBBIN_CMD_setMACAddress] = vUSBBinaryCMD_setMACAddress,
 [USBBIN_CMD_getMACAddress] = vUSBBinaryCMD_getMACAddress,
 [USBBIN_CMD_setIfConfig] = vUSBBinaryCMD_setIfConfig,
 [USBBIN_CMD_getIfConfig] = vUSBBinaryCMD_getIfConfig,

};
#define USBBIN_CMD_COUNT   (sizeof (g_apfUSBBinCmdTable) / sizeof (g_apfUSBBinCmdTable[0]))



void vUSBBIN_decode(char *payload, int32_t size)
{
    unsigned int ui32Type;
//    int  bytesSent;
    USBBIN_CMD_error_payload_t *pUSBErrorPayload;
    USBBIN_CMD_t *pTCPBinCmd = (USBBIN_CMD_t *) payload;

    char cmdbuf[64];
    USBBIN_CMD_retFrame_t *pFrame = (USBBIN_CMD_retFrame_t *)cmdbuf;
    ui32Type = pTCPBinCmd->type;


    if (ui32Type & 0x80000000UL){
        ui32Type &= 0x00FFFFFFFUL;
        if ( ui32Type < USBBIN_CMD_COUNT) {
            if (g_apfUSBBinCmdTable[ui32Type]){
//                g_apfUSBBinCmdTable[ui32Type](pTCPBinCmd->payload, size - 8); // size - kind - endOfFrame
                g_apfUSBBinCmdTable[ui32Type](payload, size - 4); // size - endOfFrame
            }else {
                //Command not implemented
                pFrame->type = TCP_CMD_error | 0x80000000;
                pFrame->retDeviceID = USBRCBINDEVICE_ID;
                pFrame->retSvcUUID = 0;
                pFrame->retParamID = 0;
                pUSBErrorPayload = (USBBIN_CMD_error_payload_t *)pFrame->payload;
                pUSBErrorPayload->errorType = TCP_CMD_ERROR_commandNotImplemented;
                USBCDCD_sendData(USBCDCD_RemoteControl, cmdbuf, (sizeof(USBBIN_CMD_retFrame_t) + sizeof(USBBIN_CMD_error_payload_t)), WAIT_FOREVER);
            }
        }else{
//            n = sprintf(cmdbuf, "Error: Command type out of range\n\r");
            pFrame->type = TCP_CMD_error | 0x80000000;
            pFrame->retDeviceID = USBRCBINDEVICE_ID;
            pFrame->retSvcUUID = 0;
            pFrame->retParamID = 0;
            pUSBErrorPayload = (USBBIN_CMD_error_payload_t *)pFrame->payload;
            pUSBErrorPayload->errorType = TCP_CMD_ERROR_commandTypeOutOfRange;
            USBCDCD_sendData(USBCDCD_RemoteControl, cmdbuf, (sizeof(USBBIN_CMD_retFrame_t) + sizeof(USBBIN_CMD_error_payload_t)), WAIT_FOREVER);
        }
    }else{
//        n = sprintf(cmdbuf, "Error: Frame not recognized\n\r");
        pFrame->type = TCP_CMD_error | 0x80000000;
        pFrame->retDeviceID = USBRCBINDEVICE_ID;
        pFrame->retSvcUUID = 0;
        pFrame->retParamID = 0;
        pUSBErrorPayload = (USBBIN_CMD_error_payload_t *)pFrame->payload;
        pUSBErrorPayload->errorType = TCP_CMD_ERROR_commandFrameNotRecognized;
        USBCDCD_sendData(USBCDCD_RemoteControl, cmdbuf, (sizeof(USBBIN_CMD_retFrame_t) + sizeof(USBBIN_CMD_error_payload_t)), WAIT_FOREVER);
    }

}

void vUSBBIN_sendDeviceIdError(int32_t clientfd, TCPBin_CMD_types_te cmdType, uint32_t deviceID)
{
    char cmdbuf[sizeof(USBBIN_CMD_retFrame_t) + sizeof(USBBIN_CMD_error_payload_t) + sizeof(USBBIN_CMD_errorDeviceID_payload_t)];
//    int  bytesSent;
    USBBIN_CMD_retFrame_t *pUSBBinCmd;
    USBBIN_CMD_error_payload_t *pUSBErrorPayload;
    USBBIN_CMD_errorDeviceID_payload_t *pDeviceIdError;

    pUSBBinCmd = (USBBIN_CMD_retFrame_t *) cmdbuf;
    pUSBErrorPayload = (USBBIN_CMD_error_payload_t *)pUSBBinCmd->payload;
    pDeviceIdError = (USBBIN_CMD_errorDeviceID_payload_t *)pUSBErrorPayload->payload;

    pUSBBinCmd->type = TCP_CMD_error | 0x80000000;
    pUSBBinCmd->retDeviceID = USBRCBINDEVICE_ID;
    pUSBBinCmd->retSvcUUID = 0;
    pUSBBinCmd->retParamID = 0;
    pUSBErrorPayload->errorType = TCP_CMD_ERROR_deviceIdNotFound;
    pDeviceIdError->cmdType = cmdType;
    pDeviceIdError->deviceID = deviceID;

    USBCDCD_sendData(USBCDCD_RemoteControl, cmdbuf, sizeof(cmdbuf), WAIT_FOREVER);
}




Void vUSBRCBinaryWorkerFxn(UArg arg0, UArg arg1)
{
    int received;
    char buffer[128];
    while(1) {
        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(USBCDCD_RemoteControl, WAIT_FOREVER);

        received = xUSBRCBinary_receiveFrameData(buffer, sizeof(buffer), WAIT_FOREVER);
        if (received) {
//            Display_printf(displayHandle, 0, 0, "Received \"%s\" (%d bytes)\r\n", data, received);
            vUSBBIN_decode(&buffer[4], received - 4);
        }

    }
}




typedef enum {
    USBRCBIN_FRAME_STATE_HEADER1,
    USBRCBIN_FRAME_STATE_HEADER2,
    USBRCBIN_FRAME_STATE_HEADER3,
    USBRCBIN_FRAME_STATE_HEADER4,
    USBRCBIN_FRAME_STATE_PAYLOAD,
    USBRCBIN_FRAME_STATE_ENDOFFRAME1,
    USBRCBIN_FRAME_STATE_ENDOFFRAME2,
    USBRCBIN_FRAME_STATE_ENDOFFRAME3,
//    USBRCBIN_FRAME_STATE_ENDOFFRAME4,
}teUSBBIN_Frame_State;

#define USBRCBIN_FRAME_HEADER1          'A'
#define USBRCBIN_FRAME_HEADER2          'L'
#define USBRCBIN_FRAME_HEADER3          'T'
#define USBRCBIN_FRAME_HEADER4          'O'
#define USBRCBIN_FRAME_ENDOFFRAME1      '\r'
#define USBRCBIN_FRAME_ENDOFFRAME2      '\n'
#define USBRCBIN_FRAME_ENDOFFRAME3      '\r'
#define USBRCBIN_FRAME_ENDOFFRAME4      '\n'


int xUSBRCBinary_receiveFrameData(char *pStr, unsigned int length, unsigned int timeout)
{
    int i32retValue = 0;
    uint32_t ui32dataReceived = 0, i32CounterRead = 0, ui32CountDown;

    teUSBBIN_Frame_State state;

    unsigned char ui8Preamble;
    bool isError, isFinish;

    ASSERT(pStr != NULL);

    if (pStr == NULL) {
        return (0);
    }

    state = USBRCBIN_FRAME_STATE_HEADER1;

    isError = false;
    isFinish = false;
    ui32CountDown = timeout;

    if (length >= 16) // sizof(header) + sizeof(type) + sizeof(CRC) + sizeof(endOfFrame)
    {
        do {
            USBCDCD_waitForConnect(USBCDCD_RemoteControl, WAIT_FOREVER);
            switch(state)
            {
            case USBRCBIN_FRAME_STATE_HEADER1:
                ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, &ui8Preamble, 1, timeout);
                if (ui32dataReceived) {
                    if (ui8Preamble == USBRCBIN_FRAME_HEADER1) {
                        pStr[0] = ui8Preamble;
                        i32CounterRead = 1;
                        ui32CountDown = timeout;
                        state = USBRCBIN_FRAME_STATE_HEADER2;
                    }
                }else{
                    Task_sleep(1);
                    if (ui32CountDown) ui32CountDown--;
                    if (!ui32CountDown) {
                        i32CounterRead = 0;
                        isFinish = true;
                        isError = true;
                    }
                }
                break;
            case USBRCBIN_FRAME_STATE_HEADER2:
                ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, &ui8Preamble, 1, timeout);
                if (ui32dataReceived) {
                    switch(ui8Preamble)
                    {
                    case USBRCBIN_FRAME_HEADER2:
                        pStr[1] = ui8Preamble;
                        i32CounterRead = 2;
                        state = USBRCBIN_FRAME_STATE_HEADER3;
                        break;
                    case USBRCBIN_FRAME_HEADER1:
                        pStr[0] = ui8Preamble;
                        i32CounterRead = 1;
                        state = USBRCBIN_FRAME_STATE_HEADER2;
                        break;
                    default:
                        i32CounterRead = 0;
                        state = USBRCBIN_FRAME_STATE_HEADER1;
                        break;
                    }
                }else{
                    Task_sleep(1);
                    if (ui32CountDown) ui32CountDown--;
                    if (!ui32CountDown) {
                        isFinish = true;
                        isError = true;
                    }
                }
                break;
            case USBRCBIN_FRAME_STATE_HEADER3:
                ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, &ui8Preamble, 1, timeout);
                if (ui32dataReceived) {
                    switch(ui8Preamble)
                    {
                    case USBRCBIN_FRAME_HEADER3:
                        pStr[2] = ui8Preamble;
                        i32CounterRead = 3;
                        state = USBRCBIN_FRAME_STATE_HEADER4;
                        break;
                    case USBRCBIN_FRAME_HEADER1:
                        pStr[0] = ui8Preamble;
                        i32CounterRead = 1;
                        state = USBRCBIN_FRAME_STATE_HEADER2;
                        break;
                    default:
                        i32CounterRead = 0;
                        state = USBRCBIN_FRAME_STATE_HEADER1;
                        break;
                    }
                }else{
                    Task_sleep(1);
                    if (ui32CountDown) ui32CountDown--;
                    if (!ui32CountDown) {
                        isFinish = true;
                        isError = true;
                    }
                }
                break;
            case USBRCBIN_FRAME_STATE_HEADER4:
                ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, &ui8Preamble, 1, timeout);
                if (ui32dataReceived) {
                    switch(ui8Preamble)
                    {
                    case USBRCBIN_FRAME_HEADER4:
                        pStr[3] = ui8Preamble;
                        i32CounterRead = 4;
                        state = USBRCBIN_FRAME_STATE_PAYLOAD;
                        break;
                    case USBRCBIN_FRAME_HEADER1:
                        pStr[0] = ui8Preamble;
                        i32CounterRead = 1;
                        state = USBRCBIN_FRAME_STATE_HEADER2;
                        break;
                    default:
                        i32CounterRead = 0;
                        state = USBRCBIN_FRAME_STATE_HEADER1;
                        break;
                    }
                }else{
                    Task_sleep(1);
                    if (ui32CountDown) ui32CountDown--;
                    if (!ui32CountDown){
                        isFinish = true;
                        isError = true;
                    }
                }
                break;
            case USBRCBIN_FRAME_STATE_PAYLOAD:
                if (length - i32CounterRead >= sizeof(int)) {
                    ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, (unsigned char *)&pStr[i32CounterRead], 1, timeout);
                    if (ui32dataReceived) {
                        if ((pStr[i32CounterRead] == USBRCBIN_FRAME_ENDOFFRAME1))
                        {
                            state = USBRCBIN_FRAME_STATE_ENDOFFRAME1;
                        }
                    }else{
                        Task_sleep(1);
                        if (ui32CountDown) ui32CountDown--;
                        if (!ui32CountDown) {
                            isFinish = true;
                            isError = true;
                        }
                    }
                }else {
                    isFinish = true;
                    isError = true;
                }
                i32CounterRead += ui32dataReceived;
                break;
            case USBRCBIN_FRAME_STATE_ENDOFFRAME1:
                if (length - i32CounterRead >= sizeof(int)) {
                    ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, (unsigned char *)&pStr[i32CounterRead], 1, timeout);
                    if (ui32dataReceived) {
                        switch(pStr[i32CounterRead]){
                        case USBRCBIN_FRAME_ENDOFFRAME1:
                            state = USBRCBIN_FRAME_STATE_ENDOFFRAME1;
                            break;
                        case USBRCBIN_FRAME_ENDOFFRAME2:
                            state = USBRCBIN_FRAME_STATE_ENDOFFRAME2;
                            break;
                        default:
                            state = USBRCBIN_FRAME_STATE_PAYLOAD;
                            break;
                        }
                    }else{
                        Task_sleep(1);
                        if (ui32CountDown) ui32CountDown--;
                        if (!ui32CountDown) {
                            isFinish = true;
                            isError = true;
                        }
                    }
                }else {
                    isFinish = true;
                    isError = true;
                }
                i32CounterRead += ui32dataReceived;
                break;
            case USBRCBIN_FRAME_STATE_ENDOFFRAME2:
                if (length - i32CounterRead >= sizeof(int)) {
                    ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, (unsigned char *)&pStr[i32CounterRead], 1, timeout);
                    if (ui32dataReceived) {
                        switch(pStr[i32CounterRead]){
                        case USBRCBIN_FRAME_ENDOFFRAME3:
                            state = USBRCBIN_FRAME_STATE_ENDOFFRAME3;
                            break;
                        default:
                            state = USBRCBIN_FRAME_STATE_PAYLOAD;
                            break;
                        }
                    }else{
                        Task_sleep(1);
                        if (ui32CountDown) ui32CountDown--;
                        if (!ui32CountDown) {
                            isFinish = true;
                            isError = true;
                        }
                    }
                }else {
                    isFinish = true;
                    isError = true;
                }
                i32CounterRead += ui32dataReceived;
                break;
            case USBRCBIN_FRAME_STATE_ENDOFFRAME3:
                if (length - i32CounterRead >= sizeof(int)) {
                    ui32dataReceived = USBCDCD_receiveData(USBCDCD_RemoteControl, (unsigned char *)&pStr[i32CounterRead], 1, timeout);
                    if (ui32dataReceived) {
                        switch(pStr[i32CounterRead]){
                        case USBRCBIN_FRAME_ENDOFFRAME4:
//                            state = USBRCBIN_FRAME_STATE_ENDOFFRAME3;
                            isFinish = true;
                            break;
                        default:
                            state = USBRCBIN_FRAME_STATE_PAYLOAD;
                            break;
                        }
                    }else{
                        Task_sleep(1);
                        if (ui32CountDown) ui32CountDown--;
                        if (!ui32CountDown) {
                            isFinish = true;
                            isError = true;
                        }
                    }
                }else {
                    isFinish = true;
                    isError = true;
                }
                i32CounterRead += ui32dataReceived;
                break;
//            case USBRCBIN_FRAME_STATE_ENDOFFRAME4:
//                break;
            default:
                i32CounterRead = 0;
                state = USBRCBIN_FRAME_STATE_HEADER1;
                break;
            }

        }while(timeout && !isFinish);
        if (i32CounterRead) i32retValue = i32CounterRead;
        if (isError) i32retValue |= 0x01;
    }

    return i32retValue;
}



