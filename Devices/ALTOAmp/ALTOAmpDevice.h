/*
 * ALTOAmpDevice.h
 *
 *  Created on: Apr 27, 2018
 *      Author: epenate
 */

#ifndef DEVICES_ALTOAMP_ALTOAMPDEVICE_H_
#define DEVICES_ALTOAMP_ALTOAMPDEVICE_H_

#define ALTOAMPDEVICE_TASK_STACK_SIZE        2048
#define ALTOAMPDEVICE_TASK_PRIORITY          5

#define ALTOAMPDEVICE_CLOCK_TIMEOUT       10
#define ALTOAMPDEVICE_CLOCK_PERIOD        1000


typedef enum
{
    ALTO_ACKNAK_GOOD_STATUS = 0x00,
    ALTO_ACKNAK_BUSY_WAITING_TO_RESPOND = 0x40,
    ALTO_ACKNAK_INVALID_BCC = 0xFF,
    ALTO_ACKNAK_INVALID_ARGUMENT = 0xFE,
    ALTO_ACKNAK_INVALID_FUNCTION_NOT_EXECUTED = 0xFD,
    ALTO_ACKNAK_INVALID_CRC = 0xFC,
    ALTO_ACKNAK_PA_NOT_RESPONDING = 0xFB,
    ALTO_ACKNAK_TDB_FORMAT_MISMATCH = 0xDF,
    ALTO_ACKNAK_TRANSFER_INVALID_ADRESS = 0xDE,
    ALTO_ACKNAK_TRANSFER_MISSING_DATA = 0xDD,
    ALTO_ACKNAK_ERASE_BURN_FAILED = 0xDC,
    ALTO_ACKNAK_NOT_WEIGHT_ON_WHEELS = 0xDB,
    ALTO_ACKNAK_TRANSFER_NOT_ACTIVE = 0xDA,
    ALTO_ACKNAK_TRANSFER_INVALID_RECORD = 0xD9,
    ALTO_ACKNAK_TRANSFER_EXTRA_DATA = 0xD8,
    ALTO_ACKNAK_SEGMENT_NOT_ACTIVE = 0xD7,
    ALTO_ACKNAK_FLOW_CONTROL_VIOLATION = 0xD6,
    ALTO_ACKNAK_SEGMENT_ALREADY_ACTIVE = 0xD5,
    ALTO_ACKNAK_HARDWARE_FAULT = 0xD4,
    ALTO_ACKNAK_PA_NOT_INDICATED_IN_MAIN_CONFIG = 0xCF,
    ALTO_ACKNAK_NO_PA_NET_CONNECTION = 0xCD,
    ALTO_ACKNAK_DIAG_SESSION_NOT_ACTIVE = 0xCD,
    ALTO_ACKNAK_FLASH_READ_FAILURE = 0xCC,
    ALTO_ACKNAK_RECORD_FAILED_VALIDATION = 0xCB,
    ALTO_ACKNAK_DEFAULT_DB_LOADED = 0xCA,
    ALTO_ACKNAK_REQUESTED_DB_LOADED = 0xBF,
} ALTOAmp_ACKNAK_t;


typedef enum
{
    SERVICE_ALTO_AMP_INPUTSELECT_UUID = 0x0809,
    SERVICE_ALTO_AMP_VOLUME_UUID = 0x0811,
    SERVICE_ALTO_AMP_BASS_UUID = 0x0812,
    SERVICE_ALTO_AMP_TREBLE_UUID = 0x0813,
    SERVICE_ALTO_AMP_MUTE_UUID = 0x0814,
    SERVICE_ALTO_AMP_COMPRESSOR_UUID = 0x0820,
    SERVICE_ALTO_AMP_LOUDNESS_UUID = 0x0821,
    SERVICE_ALTO_AMP_HP_INPUT_SELECT_UUID = 0x0709,
    SERVICE_ALTO_AMP_HP_VOLUME_UUID = 0x0711,
    SERVICE_ALTO_AMP_HP_MUTE_UUID = 0x0714,
    SERVICE_ALTO_AMP_DIRECT_COMMAND_UUID = 0xFFFF,
} ALTOAmp_service_UUID_t;


//Characteristic defines
typedef enum
{
    CHARACTERISTIC_ALTO_AMP_GET_ID = 0x01,
    CHARACTERISTIC_ALTO_AMP_SET_ID,
    CHARACTERISTIC_ALTO_AMP_INC_ID,
    CHARACTERISTIC_ALTO_AMP_DEC_ID,
} ALTOAmp_service_alto_volume_characteristics_t;

typedef enum
{
    CHARACTERISTIC_ALTO_AMP_DIRECT_COMMAND1_ID = 0x01,
    CHARACTERISTIC_ALTO_AMP_DIRECT_COMMAND2_ID,
} ALTOAmp_service_direct_command_characteristics_t;

typedef struct {
    uint8_t zone;
    uint8_t zone1Volume;
    uint8_t zone2Volume;
}ALTOAmp_zoneVolumeData_t;

typedef struct {
    uint8_t zone1Volume;
    uint8_t zone2Volume;
}ALTOAmp_volumeData_t;

typedef struct {
    uint8_t hpNum;
    uint8_t value;
}ALTOAmp_headphoneData_t;

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
}ALTOAmp_directCommandData_t;

#ifdef __cplusplus
extern "C"  {
#endif


#ifdef  __DEVICES_ALTOAMPDEVICE_ALTOMULTINETDEVICE_GLOBAL
    #define __DEVICES_ALTOAMPDEVICE_ALTOMULTINETDEVICE_EXT
#else
    #define __DEVICES_ALTOAMPDEVICE_ALTOMULTINETDEVICE_EXT  extern
#endif


extern const Device_FxnTable g_ALTOAmpDevice_fxnTable;

__DEVICES_ALTOAMPDEVICE_ALTOMULTINETDEVICE_EXT void vALTOAmpDevice_Params_init(Device_Params *params, uint32_t address);



#ifdef __cplusplus
}
#endif



#endif /* DEVICES_ALTOAMP_ALTOAMPDEVICE_H_ */
