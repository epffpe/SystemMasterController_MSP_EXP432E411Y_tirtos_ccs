/*
 * GenericDevice.h
 *
 *  Created on: Aug 8, 2018
 *      Author: epenate
 */

#ifndef DEVICES_GENERICDEVICE_GENERICDEVICE_H_
#define DEVICES_GENERICDEVICE_GENERICDEVICE_H_

#define GENERICDEVICE_TASK_STACK_SIZE        2048
#define GENERICDEVICE_TASK_PRIORITY          2

#define GENERIC_DEVICE_CLOCK_TIMEOUT       100
#define GENERIC_DEVICE_CLOCK_PERIOD        100


typedef struct {
    UInt            andMask;
    UInt            orMask;
    UInt            timeout;
    Device_HookFxn  setupFxn;
    UArg            setupArg0;
    Device_HookFxn  eventFxns[32];
    UArg            eventFxnArg0[32];
}GenericDevice_hookParams;


typedef struct {
    GenericDevice_hookParams   *hooks;
    UArg                        arg1;
}GenericDevice_args;


#ifdef  __DEVICES_GENERICDEVICE_GLOBAL
    #define __DEVICES_GENERICDEVICE_EXT
#else
    #define __DEVICES_GENERICDEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif


__DEVICES_GENERICDEVICE_EXT
void vGeneticDevice_Hooks_init(GenericDevice_hookParams *hooks);
__DEVICES_GENERICDEVICE_EXT
void vGeneticDevice_Params_init(Device_Params *params, uint32_t address, GenericDevice_hookParams *hooks);
__DEVICES_GENERICDEVICE_EXT
uint32_t xGeneticDevice_registerHookFunctionForEventId(GenericDevice_hookParams *hooks, uint32_t eventId, Device_HookFxn hookFxn, UArg arg0);
__DEVICES_GENERICDEVICE_EXT
void vGeneticDevice_registerHookFunctionForSetUp(GenericDevice_hookParams *hooks, Device_HookFxn hookFxn, UArg arg0);

#ifdef __cplusplus
}
#endif


#endif /* DEVICES_GENERICDEVICE_GENERICDEVICE_H_ */
