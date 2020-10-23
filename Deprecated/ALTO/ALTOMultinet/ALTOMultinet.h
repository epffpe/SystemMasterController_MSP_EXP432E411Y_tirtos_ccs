/*
 * ALTOMultinet.h
 *
 *  Created on: Jan 12, 2018
 *      Author: epenate
 */

#ifndef ALTOMULTINET_H_
#define ALTOMULTINET_H_


#define ALTOMULTINET_TASK_STACK_SIZE        2048
#define ALTOMULTINET_TASK_PRIORITY          6

#define ALTOMULTINET_NUM_RELAYS_DEVICES     32
#define ALTOMULTINET_NUM_TEMP_DEVICES       32

#define ALTOMULTINET_NUM_ORT_MSGS           200

typedef struct {
    uint32_t ui32SerialDEPin;
    uint32_t ui32SerialREPin;
    uint32_t ui32SerialPortIndex;
}ALTOMultinet_Params;


typedef struct {
    RelayControllerWorker_Params tRelayControllerWorkerParams;
    Queue_Handle hMsgQ;
    bool bIsEnabled;
}ALTOMultinetRelays_Table;


typedef struct ALTOMultinetRemoteRoutingMsgObj {
    Queue_Elem elem; /* first field for Queue */
    tALTOORTInput tORTInput;
} ALTOMultinetRemoteRoutingMsgObj, *ALTOMultinetRemoteRoutingMsg;




typedef struct {
    Queue_Handle hFreeQueue;
    ALTOMultinetRelays_Table *atALTOMultinetRelayTable;
    ALTOMultinetRelays_Table *atALTOMultinetTempTable;
    GateMutexPri_Handle gateMutex;
}ALTOMultinetControllerWorkerQ_Params;


typedef enum {
    ALTO_ALTONet_Amp = 0x01,
    ALTO_Multinet_RelayBox = 0x2,
    ALTO_Multinet_TemperatureBox = 0x3,
}ALTOMultinetUnitType;




#ifdef __cplusplus
extern "C"  {
#endif


#ifdef  __ALTOMULTINET_GLOBAL
    #define __ALTOMULTINET_EXT
#else
    #define __ALTOMULTINET_EXT  extern
#endif

__ALTOMULTINET_EXT void vALTOMultinet_Params_init(ALTOMultinet_Params *params);
__ALTOMULTINET_EXT Task_Handle xALTOMultinet_init(ALTOMultinet_Params *params);


#ifdef __cplusplus
}
#endif



#endif /* ALTOMULTINET_H_ */
