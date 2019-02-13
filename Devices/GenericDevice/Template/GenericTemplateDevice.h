/*
 * GenericTemplateDevice.h
 *
 *  Created on: Sep 19, 2018
 *      Author: epenate
 */

#ifndef DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_H_
#define DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_H_


#define DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_TASK_STACK_SIZE        2048
#define DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_TASK_PRIORITY          2

#define GENERIC_DEVICE_TEMPLATE_CLOCK_TIMEOUT       100
#define GENERIC_DEVICE_TEMPLATE_CLOCK_PERIOD        100


#ifdef  __DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_GLOBAL
    #define __DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_EXT
#else
    #define __DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_GENERICDEVICE_EXT
void vGenericTemplateDevice_Params_init(Device_Params *params, uint32_t address);


#ifdef __cplusplus
}
#endif






#endif /* DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_H_ */