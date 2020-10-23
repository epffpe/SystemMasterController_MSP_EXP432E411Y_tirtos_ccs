/*
 * ALTOInterface.h
 *
 *  Created on: Dec 11, 2017
 *      Author: epenate
 */

#ifndef ALTOINTERFACE_H_
#define ALTOINTERFACE_H_




#define ALTO_FRAME_DATA_PAYLOAD_SIZE            26

#ifdef __cplusplus
extern "C"  {
#endif

#define ALTO_FULL_FRAME_MSG_SIZE        70
#define ALTO_FRAME_MSG_SIZE             64


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32;1m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"



/* ALTO Frame Message */
typedef struct ALTOFrameMsgObj {
    Queue_Elem elem; /* first field for Queue */
    char buff[ALTO_FRAME_MSG_SIZE];
    char msgSize;
} ALTOFrameMsgObj, *ALTOFrameMsg;


typedef struct {
    uint8_t classID;
    uint8_t operationID;
    uint8_t functionID;
    union {
        uint8_t sequence;
        uint8_t ui8MultinetAddress;
        struct {
            uint8_t ui4address      : 5;
            uint8_t uin4unitType     : 3;
        };
    };
    uint8_t length;
    uint8_t data[ALTO_FRAME_DATA_PAYLOAD_SIZE];
    uint8_t bcc;
}ALTO_Frame_t;

typedef enum {
    ALTO_Class_Control_Channel = 0x00,
    ALTO_Class_Diagnostic = 0x01,
    ALTO_Class_Software_Transfer = 0x02,
    ALTO_Class_Aircraft_Information = 0x03,
    ALTO_Class_Analog = 0x04,
    ALTO_Class_Headphone = 0x07,
    ALTO_Class_Amplifier = 0x08,
    ALTO_Class_PA_Chime = 0x09,
    ALTO_Class_Clu = 0x0B,
}ALTOClassesIDName;


typedef enum {
    ALTO_Operation_Set = 0x00,
    ALTO_Operation_Get = 0x01,
    ALTO_Operation_Unsolicided = 0x05,
    ALTO_Operation_Response,
    ALTO_Operation_ACKNAK,
    ALTO_Operation_Status,
    ALTO_Operation_MethodStart = 0x80,
    ALTO_Operation_MethodAction = 0x82,
    ALTO_Operation_MethodStop = 0x84,
    ALTO_Operation_MethodResult = 0x86,

    // Operations for Volume Function
    ALTO_Operation_Volume_Inc = 0x03,
    ALTO_Operation_Volume_Dec = 0x04,
}ALTOOperationsIDName;


typedef enum {
    // Functions for ControlChannel class 0x00
    ALTO_Function_Heartbeat = 0x20,
    ALTO_Function_HeartbeatTimeoutOverride = 0x21,
    ALTO_Function_DataExchange = 0x24,
    ALTO_Function_PowerOnInit = 0x30,

    // Functions for Diagnostic Class 0x01
    ALTO_Function_PrepareForRestart = 0x09,
    ALTO_Function_DeviceDetailedStatus = 0x10,
    ALTO_Function_ConfigDataParameter = 0x40,
    ALTO_Function_DiagRestart = 0x63,
    ALTO_Function_ManufacturingInformation = 0x70,
    ALTO_Function_TuningDatabaseInfo = 0x74,
    ALTO_Function_TuningDBRecordInfo = 0x75,
    ALTO_Function_ActiveConfigDatabase = 0x78,

    // Functions for SoftwareTransfer Class 0x02
    ALTO_Function_DownloadStart = 0x10,
    ALTO_Function_DownloadSegment = 0x11,
    ALTO_Function_DownloadEnd = 0x12,
    ALTO_Function_DownloadAbort = 0x13,
    ALTO_Function_TransferData = 0x30,
    ALTO_Function_TransferStatus = 0x31,

    // Functions for AircraftInformation Class 0x03
    ALTO_Function_AircraftInfo = 0x11,
    ALTO_Function_WOWOverride = 0x0B,

    // Functions for Analog Class 0x04
    ALTO_Function_AnalogSelect_AB_1 = 0x01,
    ALTO_Function_AnalogSelect_AB_2 = 0x02,
    ALTO_Function_AnalogSelect_Diag = 0x03,

    // Functions for Amplifier Class 0x08
    ALTO_Function_DiagInputSelect = 0x03,
    ALTO_Function_AudioFormat = 0x08,
    ALTO_Function_InputSelect = 0x09,
    ALTO_Function_Volume = 0x11,
    ALTO_Function_Bass = 0x12,
    ALTO_Function_Treble = 0x13,
    ALTO_Function_Mute = 0x14,
    ALTO_Function_Compressor = 0x20,
    ALTO_Function_Loudness = 0x21,
    ALTO_Function_Spatial = 0x22,
    ALTO_Function_SurroundEnable = 0x30,

    // Functions for PAChime Class 0x09
    ALTO_Function_SetOutputChannels = 0x10,
    ALTO_Function_ChimeAudioSequence = 0x11,
    ALTO_Function_PAEvent = 0x12,

    // Functions for CLU Class 0x0B
    ALTO_Function_CluInpInfo = 0x10,
    ALTO_Function_CluInpOverride = 0x12,
    ALTO_Function_CluMasterOff = 0x33,
    ALTO_Function_CluSerialMemory = 0x40,
    ALTO_Function_CluProgramState = 0x41,
    ALTO_Function_CluClearProgramException = 0x42,
    ALTO_Function_CluClearSerialMemory = 0x43,
    ALTO_Function_CluBitMap = 0x44,
    ALTO_Function_CluPinVinGate = 0x50,
    ALTO_Function_CluLocalRemoteVin = 0x51,

}ALTOFunctionsIDName;



typedef enum {
    ALTO_AckNak_GoodStatus = 0x00,
    ALTO_AckNak_Busy_WaitingToRespond = 0x40,
    ALTO_AckNak_DiagSessionNotActive = 0xCD,
    ALTO_AckNak_HardwareFault = 0xD4,
    ALTO_AckNak_InvalidCRC = 0xFC,
    ALTO_AckNak_FunctionNotExecuted = 0xFD,
    ALTO_AckNak_InvalidArgument = 0xFE,
    ALTO_AckNak_InvalidBcc = 0xFF,
}ALTOAckNakIDName;


typedef struct {
    uint8_t softwareVersionMajor;
    uint8_t softwareVersionMinor;
    uint8_t hardwareVersion;
    uint8_t reserved1;
    uint8_t unused1;
    uint8_t reserved2;
    uint8_t unused2[3];
    uint8_t multiNetAddress;
    uint8_t unitSN3;
    uint8_t unitSN2;
    uint8_t unitSN1;
    uint8_t unitSN0;
    uint8_t unused3;
    uint8_t unitType;
    uint8_t unitSubType;
    uint8_t unitIDNumber;
    uint8_t cfgIDNumber;
    uint8_t MPRev;
    uint8_t mainSWPN2;
    uint8_t mainSWPN1;
    uint8_t mainSWPN0;
    uint8_t unused4[3];
}tALTOManufacturingInfo;

typedef struct {
    uint16_t inputState                 : 1;
    uint16_t destinationVirtualInput    : 5;
    uint16_t destinationAddress         : 5;
    uint16_t destinationType            : 3;
    uint16_t flushOverflow              : 1;
    uint16_t ignored                    : 1;
}tALTOORTRouting;

typedef struct {
    uint8_t value           : 1;
    uint8_t inputAddress    : 5;
    uint8_t reserved        : 2;
}tALTOORTInput;


#ifdef  __ALTOINTERFACE_GLOBAL
    #define __ALTOINTERFACE_EXT
#else
    #define __ALTOINTERFACE_EXT  extern
#endif

__ALTOINTERFACE_EXT void vALTOFrame_Params_init( ALTO_Frame_t *tframe );

__ALTOINTERFACE_EXT int8_t cALTOFrame_BCC_calulate( ALTO_Frame_t *tframe );
__ALTOINTERFACE_EXT void vALTOFrame_BCC_fill( ALTO_Frame_t *tframe );
__ALTOINTERFACE_EXT int8_t cALTOFrame_BCC_check( ALTO_Frame_t *tframe );

__ALTOINTERFACE_EXT void vALTOFrame_create_ASCII(char *pi8Buffer70, ALTO_Frame_t *tframe);
__ALTOINTERFACE_EXT int xALTOFrame_convert_ASCII_to_binary(char *pi8Buffer64, ALTO_Frame_t *tframe);
__ALTOINTERFACE_EXT void vALTOFrame_ORTConvertDataFromBigEndianToLittleEndian(uint8_t *pui8Data, uint8_t ui8Length);

#ifdef __cplusplus
}
#endif

#endif /* ALTOINTERFACE_H_ */
