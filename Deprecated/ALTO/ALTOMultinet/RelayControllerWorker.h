/*
 * RelayControllerWorker.h
 *
 *  Created on: Jan 12, 2018
 *      Author: epenate
 */

#ifndef RELAYCONTROLLERWORKER_H_
#define RELAYCONTROLLERWORKER_H_


#define RELAYCONTROLLERWORKER_TASK_STACK_SIZE      2048
#define RELAYCONTROLLERWORKER_TASK_PRIORITY        6


typedef struct {
    void *atALTOMultinetRelaysTable;
    uint32_t ui32SerialPortIndex;
    uint8_t  ui8VersionMajor;
    uint8_t  ui8VersionMinor;
    union {
        uint8_t ui8MultinetAddress;
        struct {
            uint8_t ui4address      : 5;
            uint8_t ui4unitType     : 3;
        };
    };
}RelayControllerWorker_Params;


#ifdef __cplusplus
extern "C"  {
#endif


#ifdef  __RELAYCONTROLLERWORKER_GLOBAL
    #define __RELAYCONTROLLERWORKER_EXT
#else
    #define __RELAYCONTROLLERWORKER_EXT  extern
#endif

__RELAYCONTROLLERWORKER_EXT void vRelayControllerWorker_Params_init(RelayControllerWorker_Params *params);
__RELAYCONTROLLERWORKER_EXT Task_Handle xRelayControllerWorker_init(RelayControllerWorker_Params *params,
                                                                    void *ptControllerWorkerQParams);



#ifdef __cplusplus
}
#endif




#endif /* RELAYCONTROLLERWORKER_H_ */
