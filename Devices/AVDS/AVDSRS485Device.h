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
} AVDS485Device_service_Steve_Command_characteristics_t;



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
