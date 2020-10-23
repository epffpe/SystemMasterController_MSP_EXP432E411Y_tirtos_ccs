/*
 * TempControllerWorker.h
 *
 *  Created on: Jan 29, 2018
 *      Author: epenate
 */

#ifndef TEMPCONTROLLERWORKER_H_
#define TEMPCONTROLLERWORKER_H_


#define TEMPCONTROLLERWORKER_TASK_STACK_SIZE      2048
#define TEMPCONTROLLERWORKER_TASK_PRIORITY        6


#ifdef __cplusplus
extern "C"  {
#endif


#ifdef  __TEMPCONTROLLERWORKER_GLOBAL
    #define __TEMPCONTROLLERWORKER_EXT
#else
    #define __TEMPCONTROLLERWORKER_EXT  extern
#endif

__TEMPCONTROLLERWORKER_EXT void vTempControllerWorker_Params_init(RelayControllerWorker_Params *params);
__TEMPCONTROLLERWORKER_EXT Task_Handle xTempControllerWorker_init(RelayControllerWorker_Params *params,
                                                                    void *ptControllerWorkerQParams);


#ifdef __cplusplus
}
#endif


#endif /* TEMPCONTROLLERWORKER_H_ */
