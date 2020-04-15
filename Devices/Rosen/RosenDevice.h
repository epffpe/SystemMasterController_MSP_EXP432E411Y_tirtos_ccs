/*
 * RosenDevice.h
 *
 *  Created on: Apr 3, 2020
 *      Author: epf
 */

#ifndef DEVICES_ROSEN_ROSENDEVICE_H_
#define DEVICES_ROSEN_ROSENDEVICE_H_


#define DEVICES_ROSEN_ROSENDEVICE_TASK_STACK_SIZE        2048
#define DEVICES_ROSEN_ROSENDEVICE_TASK_PRIORITY          2

#define DEVICES_ROSEN_ROSENDEVICE_CLOCK_TIMEOUT       1000
#define DEVICES_ROSEN_ROSENDEVICE_CLOCK_PERIOD        1000

#define ROSEN_MAX_PACKET_SIZE            128


#define ROSEN_UDP_PORT                  7399

typedef enum
{
    SERVICE_ROSEN_UDP_ALTO_EMULATOR_UUID = 0x0101,
    SERVICE_ROSEN_UDP_COMMANDS_UUID,
    SERVICE_ROSEN_UDP_STEVE_COMMANDS_UUID,
} RosenUDPDevice_service_UUID_t;



//Characteristic defines
typedef enum
{
    CHARACTERISTIC_SERVICE_ROSEN_UDP_ALTO_EMULATOR_DIRECT_COMMAND_ID = 0x01
} RosenDevice_service_alto_emulator_characteristics_t;

typedef enum
{
    CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_SET_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSEN_UDP_STEVE_COMMAND_SM_SOURCE_GET_ID
} RosenUDPDevice_service_steve_commands_characteristics_t;


typedef struct {
    uint32_t ui32IPAddress;
    uint8_t ui8Value;
}RosenUDPDevice_SteveCommandData_t;



#ifdef  __DEVICES_ROSEN_ROSENDEVICE_GLOBAL
    #define __DEVICES_ROSEN_ROSENDEVICE_EXT
#else
    #define __DEVICES_ROSEN_ROSENDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_ROSEN_ROSENDEVICE_EXT
void vRosenDevice_Params_init(Device_Params *params, uint32_t address);


#ifdef __cplusplus
}
#endif





#endif /* DEVICES_ROSEN_ROSENDEVICE_H_ */
