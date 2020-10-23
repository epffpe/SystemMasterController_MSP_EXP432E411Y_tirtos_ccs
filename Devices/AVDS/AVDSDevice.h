/*
 * AVDSDevice.h
 *
 *  Created on: Mar 25, 2020
 *      Author: epf
 */

#ifndef DEVICES_AVDS_AVDSDEVICE_H_
#define DEVICES_AVDS_AVDSDEVICE_H_


#define DEVICES_AVDS_AVDSDEVICE_TASK_STACK_SIZE        2048
#define DEVICES_AVDS_AVDSDEVICE_TASK_PRIORITY          2

#define DEVICES_AVDS_AVDSDEVICE_CLOCK_TIMEOUT       1000
#define DEVICES_AVDS_AVDSDEVICE_CLOCK_PERIOD        100

#define AVDS_MAX_PACKET_SIZE            64

typedef enum
{
    SERVICE_AVDS_ALTO_EMULATOR_UUID = 0x0001,
    SERVICE_AVDS_COMMANDS_UUID = 0x0002,
} AVDSDevice_service_UUID_t;

//Characteristic defines
typedef enum
{
    CHARACTERISTIC_AVDS_ALTO_EMULATOR_DIRECT_COMMAND_ID = 0x01
} AVDSDevice_service_alto_emulator_characteristics_t;

typedef enum
{
    CHARACTERISTIC_AVDS_Commands_SetChannel_ID = 0x01,
    CHARACTERISTIC_AVDS_Commands_GetChannel_ID,
    CHARACTERISTIC_AVDS_Commands_SetZone_ID,
    CHARACTERISTIC_AVDS_Commands_SetOverride_ID,
    CHARACTERISTIC_AVDS_Commands_RestoreOverride_ID,
} AVDSDevice_service_commands_characteristics_t;


#ifdef  __DEVICES_AVDS_AVDSDEVICE_GLOBAL
    #define __DEVICES_AVDS_AVDSDEVICE_EXT
#else
    #define __DEVICES_AVDS_AVDSDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_AVDS_AVDSDEVICE_EXT
void vAVDSDevice_Params_init(Device_Params *params, uint32_t address);


#ifdef __cplusplus
}
#endif





#endif /* DEVICES_AVDS_AVDSDEVICE_H_ */
