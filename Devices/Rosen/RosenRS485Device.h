/*
 * RosenRS485.h
 *
 *  Created on: Apr 6, 2020
 *      Author: epf
 */

#ifndef DEVICES_ROSEN_ROSENRS485DEVICE_H_
#define DEVICES_ROSEN_ROSENRS485DEVICE_H_


#define DEVICES_ROSEN_ROSENRS485DEVICE_TASK_STACK_SIZE        2048
#define DEVICES_ROSEN_ROSENRS485DEVICE_TASK_PRIORITY          2

#define DEVICES_ROSEN_ROSENRS485DEVICE_CLOCK_TIMEOUT       1000
#define DEVICES_ROSEN_ROSENRS485DEVICE_CLOCK_PERIOD        1000




#ifdef  __DEVICES_ROSEN_ROSENRS485DEVICE_GLOBAL
    #define __DEVICES_ROSEN_ROSENRS485DEVICE_EXT
#else
    #define __DEVICES_ROSEN_ROSENRS485DEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_ROSEN_ROSENRS485DEVICE_EXT
void vRosen485Device_Params_init(Device_Params *params, uint32_t address);


#ifdef __cplusplus
}
#endif






#endif /* DEVICES_ROSEN_ROSENRS485DEVICE_H_ */
