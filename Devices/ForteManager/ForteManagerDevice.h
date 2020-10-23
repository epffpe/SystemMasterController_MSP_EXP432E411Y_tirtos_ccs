/*
 * ForteManagerDevice.h
 *
 *  Created on: Sep 17, 2018
 *      Author: epenate
 */

#ifndef DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_H_
#define DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_H_


#define DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_TASK_STACK_SIZE        2048
#define DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_TASK_PRIORITY          7

#define FORTEMANAGER_DEVICE_CLOCK_TIMEOUT       100
#define FORTEMANAGER_DEVICE_CLOCK_PERIOD        100


typedef struct {
    uint32_t ui32PCSerialDEPin;
    uint32_t ui32PCSerialREPin;
    uint32_t ui32PCPortIndex;
    uint32_t ui32DevicePortIndex;
}ForteManager_Params;



#ifdef  __DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_GLOBAL
    #define __DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_EXT
#else
    #define __DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_GENERICDEVICE_EXT
void vForteManagerDevice_Params_init(Device_Params *params, uint32_t address, uint32_t ifIndex);


#ifdef __cplusplus
}
#endif




#endif /* DEVICES_FORTEMANAGER_FORTEMANAGERDEVICE_H_ */
