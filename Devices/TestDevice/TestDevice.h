/*
 * TestDevice.h
 *
 *  Created on: Mar 26, 2018
 *      Author: epenate
 */

#ifndef DEVICES_TESTDEVICE_TESTDEVICE_H_
#define DEVICES_TESTDEVICE_TESTDEVICE_H_


#ifdef __cplusplus
extern "C"  {
#endif

#define TEST_DEVICE_CLOCK_TIMEOUT       10
#define TEST_DEVICE_CLOCK_PERIOD        30


#ifdef  __DEVICES_TESTDEVICE_TESTDEVICE_GLOBAL
    #define __DEVICES_TESTDEVICE_TESTDEVICE_EXT
#else
    #define __DEVICES_TESTDEVICE_TESTDEVICE_EXT  extern
#endif


extern const Device_FxnTable g_TestDevice_fxnTable;

__DEVICES_TESTDEVICE_TESTDEVICE_EXT void vTestDevice_Params_init(Device_Params *params, uint32_t address);

#ifdef __cplusplus
}
#endif





#endif /* DEVICES_TESTDEVICE_TESTDEVICE_H_ */
