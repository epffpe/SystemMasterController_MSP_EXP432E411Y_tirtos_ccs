/*
 * ComputerBridge.h
 *
 *  Created on: Jan 12, 2018
 *      Author: epenate
 */

#ifndef COMPUTERBRIDGE_H_
#define COMPUTERBRIDGE_H_


#define COMPUTERBRIDGE_TASK_STACK_SIZE      1600
#define COMPUTERBRIDGE_TASK_PRIORITY        7
#define COMPUTERBRIDGE_TASK_RX_STACK_SIZE   1600
#define COMPUTERBRIDGE_RX_TASK_PRIORITY     9

#define COMPUTERBRIDGE_MAILBOX_NUM_BUFS     128


typedef struct {
    uint32_t ui32PCSerialDEPin;
    uint32_t ui32PCSerialREPin;
    uint32_t ui32PCPortIndex;
    uint32_t ui32DevicePortIndex;
}ComputerBridge_Params;


#ifdef __cplusplus
extern "C"  {
#endif


#ifdef  __COMPUTERBRIDGE_GLOBAL
    #define __COMPUTERBRIDGE_EXT
#else
    #define __COMPUTERBRIDGE_EXT  extern
#endif

__COMPUTERBRIDGE_EXT void vComputerBridge_Params_init(ComputerBridge_Params *params);
__COMPUTERBRIDGE_EXT Task_Handle xComputerBridge_init(ComputerBridge_Params *params);


#ifdef __cplusplus
}
#endif


#endif /* COMPUTERBRIDGE_H_ */
