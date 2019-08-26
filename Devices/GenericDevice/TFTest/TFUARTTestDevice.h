/*
 * TFUARTTestDevice.h
 *
 *  Created on: Aug 19, 2019
 *      Author: epenate
 */

#ifndef DEVICES_TFTEST_TFUARTTESTDEVICE_H_
#define DEVICES_TFTEST_TFUARTTESTDEVICE_H_


#define DEVICES_TFTEST_TFUARTTESTDEVICE_TASK_STACK_SIZE        2048
#define DEVICES_TFTEST_TFUARTTESTDEVICE_TASK_PRIORITY          6

#define DEVICES_TFTEST_TFUARTTESTDEVICE_CLOCK_TIMEOUT       100
#define DEVICES_TFTEST_TFUARTTESTDEVICE_CLOCK_PERIOD        100


#ifdef  __DEVICES_TFTEST_TFUARTTESTDEVICE_GLOBAL
    #define __DEVICES_TFTEST_TFUARTTESTDEVICE_EXT
#else
    #define __DEVICES_TFTEST_TFUARTTESTDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif

__DEVICES_TFTEST_TFUARTTESTDEVICE_EXT
uint32_t g_TFUartTestErrorCounter[IF_COUNT];


__DEVICES_TFTEST_TFUARTTESTDEVICE_EXT
void vTFUartTestDevice_Params_init(Device_Params *params, uint32_t address, uint32_t ifIndex);


#ifdef __cplusplus
}
#endif





#endif /* DEVICES_TFTEST_TFUARTTESTDEVICE_H_ */
