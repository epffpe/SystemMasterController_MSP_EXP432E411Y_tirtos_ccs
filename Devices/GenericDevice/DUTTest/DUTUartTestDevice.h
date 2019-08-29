/*
 * DUTTestDevice.h
 *
 *  Created on: Aug 23, 2019
 *      Author: epenate
 */

#ifndef DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_H_
#define DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_H_

#define DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_TASK_STACK_SIZE        2048
#define DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_TASK_PRIORITY          6

#define DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_CLOCK_TIMEOUT       100
#define DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_CLOCK_PERIOD        100


#ifdef  __DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_GLOBAL
    #define __DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_EXT
#else
    #define __DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_EXT
void vDUTUartTestDevice_Params_init(Device_Params *params, uint32_t address, uint32_t ifIndex);


#ifdef __cplusplus
}
#endif






#endif /* DEVICES_GENERICDEVICE_DUTTEST_DUTUARTTESTDEVICE_H_ */