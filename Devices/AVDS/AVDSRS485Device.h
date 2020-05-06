/*
 * AVDSRS485Device.h
 *
 *  Created on: Apr 22, 2020
 *      Author: epffpe
 */

#ifndef DEVICES_AVDS_AVDSRS485DEVICE_H_
#define DEVICES_AVDS_AVDSRS485DEVICE_H_



#define DEVICES_AVDS_AVDSRS485DEVICE_TASK_STACK_SIZE        2048
#define DEVICES_AVDS_AVDSRS485DEVICE_TASK_PRIORITY          2

#define DEVICES_AVDS_AVDSRS485DEVICE_CLOCK_TIMEOUT        1000
#define DEVICES_AVDS_AVDSRS485DEVICE_CLOCK_PERIOD         1000


typedef enum
{
    SERVICE_AVDSRS485DEVICE_ALTO_EMULATOR_UUID = 0x0201,
    SERVICE_AVDSRS485DEVICE_STEVE_COMMANDS_UUID,
} AVDS485Device_service_UUID_t;


typedef enum
{
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_SET_ID = 0x01,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_CHANNEL_GET_ID,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_VOLUME_SET_ID,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_VOLUME_GET_ID,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_MUTE_SET_ID,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_MUTE_GET_ID,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_BASS_SET_ID,
    CHARACTERISTIC_SERVICE_AVDSRS485DEVICE_STEVE_COMMAND_SERIAL_BASS_GET_ID,
} AVDS485Device_service_Steve_Command_characteristics_t;















typedef enum {
    AVDS485DEVICE_COMMAND_INITIALIZE_COMMUNICATIONS = 0x22,
    AVDS485DEVICE_COMMAND_PING = 0x23,
    AVDS485DEVICE_COMMAND_SET_CHANNEL = 0x10,
    AVDS485DEVICE_COMMAND_GET_CHANNEL = 0x11,
    AVDS485DEVICE_COMMAND_SET_ZONE = 0x12,
    AVDS485DEVICE_COMMAND_SET_OVERRIDE = 0x13,
    AVDS485DEVICE_COMMAND_RESTORE_OVERRIDE = 0x14,
    AVDS485DEVICE_COMMAND_SET_ZONE_OVERRIDE = 0x15,
    AVDS485DEVICE_COMMAND_RESTORE_ZONE_OVERRIDE = 0x15,
    AVDS485DEVICE_COMMAND_SET_CONTROL_PROPERTY = 0x19,
    AVDS485DEVICE_COMMAND_GET_CONTROL_PROPERTY = 0x1A,
}AVDS485Device_Commands_type;


typedef enum {
    AVDS485DEVICE_COMMAND_INITIALIZE_COMMUNICATIONS_SIZE = 1,
    AVDS485DEVICE_COMMAND_PING_SIZE = 1,
    AVDS485DEVICE_COMMAND_SET_CHANNEL_SIZE = 7,
    AVDS485DEVICE_COMMAND_GET_CHANNEL_SIZE = 3,
    AVDS485DEVICE_COMMAND_SET_CONTROL_PROPERTY_SIZE = 8,
    AVDS485DEVICE_COMMAND_GET_CONTROL_PROPERTY_SIZE = 4,
}AVDS485Device_Commands_size;



typedef enum {
    AVDS485DEVICE_PROPERTY_VOLUME = 0x04,
    AVDS485DEVICE_PROPERTY_MUTE = 0x05,
    AVDS485DEVICE_PROPERTY_TREBLE = 0x06,
    AVDS485DEVICE_PROPERTY_BASS = 0x07,
    AVDS485DEVICE_PROPERTY_COMPRESSION = 0x17,
}AVDS485Device_property;



typedef struct  {
    uint8_t                         outputChannel;
    uint8_t                         audioInputChannel;
    uint8_t                         videoInputChannel;
}AVDS485Device_serviceSteveCommand_charSetChannel_data;

typedef struct  {
    uint32_t                        result;
}AVDS485Device_serviceSteveCommand_charSetChannelResp_data;

typedef struct  {
    uint8_t                         outputChannel;
}AVDS485Device_serviceSteveCommand_charGetChannel_data;

typedef struct  {
    uint32_t                        result;
    uint8_t                         audioInputChannel;
    uint8_t                         videoInputChannel;
}AVDS485Device_serviceSteveCommand_charGetChannelResp_data;


typedef struct __attribute__ ((__packed__))  {
    uint8_t                         outputChannel;
    uint32_t                        value;
}AVDS485Device_serviceSteveCommand_charSetProperty_data;

typedef struct  {
    uint32_t                        result;
}AVDS485Device_serviceSteveCommand_charSetPropertyResp_data;




typedef struct {
    uint8_t                         outputChannel;
    uint8_t                         property;
}AVDS485Device_serviceSteveCommand_charGetProperty_data;

typedef struct  {
    uint32_t                        result;
    uint32_t                        value;
}AVDS485Device_serviceSteveCommand_charGetPropertyResp_data;



/*
 * Preamble 1 (0xAA)
 * Preamble 2 (0x55)
 */
typedef struct {
    uint8_t     preamble1;
    uint8_t     preamble2;
    uint16_t    packetLength;
}AVDS485Device_Command_Wrapper;

typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint16_t                        crc;
}AVDS485Device_Command_Initialize_Communication;

typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint32_t                        result;
    uint16_t                        crc;
}AVDS485Device_Command_Initialize_Communication_Response;


typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint16_t                        outputChannel;
    uint16_t                        audioInputChannel;
    uint16_t                        videoInputChannel;
    uint16_t                        crc;
}AVDS485Device_Command_set_channel;

typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint32_t                        result;
    uint16_t                        crc;
}AVDS485Device_Command_set_channel_Response;



typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint16_t                        outputChannel;
    uint16_t                        crc;
}AVDS485Device_Command_get_channel;

typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint32_t                        result;
    uint16_t                        audioInputChannel;
    uint16_t                        videoInputChannel;
    uint16_t                        crc;
}AVDS485Device_Command_get_channel_Response;



typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint16_t                        outputChannel;
    uint8_t                         property;
    uint32_t                        value;
    uint16_t                        crc;
}AVDS485Device_Command_setControlProperty;

typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint32_t                        result;
    uint16_t                        crc;
}AVDS485Device_Command_setControlProperty_Response;



typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint16_t                        outputChannel;
    uint8_t                         property;
    uint16_t                        crc;
}AVDS485Device_Command_getControlProperty;

typedef struct __attribute__ ((__packed__)) {
    AVDS485Device_Command_Wrapper   wrapper;
    uint8_t                         command;
    uint32_t                        result;
    uint32_t                        value;
    uint16_t                        crc;
}AVDS485Device_Command_getControlProperty_Response;


#ifdef  __DEVICES_AVDS_AVDSRS485DEVICE_GLOBAL
    #define __DEVICES_AVDS_AVDSRS485DEVICE_EXT
#else
    #define __DEVICES_AVDS_AVDSRS485DEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_AVDS_AVDSRS485DEVICE_EXT
void vAVDS485Device_Params_init(Device_Params *params, uint32_t address);


#ifdef __cplusplus
}
#endif




#endif /* DEVICES_AVDS_AVDSRS485DEVICE_H_ */
