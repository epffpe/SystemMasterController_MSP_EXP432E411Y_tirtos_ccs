/*
 * ALTOMultinetDevice.h
 *
 *  Created on: Mar 30, 2018
 *      Author: epenate
 */

#ifndef DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_H_
#define DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_H_

#define ALTOMULTINETDEVICE_TASK_STACK_SIZE        2048
#define ALTOMULTINETDEVICE_TASK_PRIORITY          6
#define ALTOMULTINETDEVICE_DISCOVERY_TASK_PRIORITY 5

#define ALTOMULTINETDEVICE_CLOCK_TIMEOUT       10
#define ALTOMULTINETDEVICE_CLOCK_PERIOD        10000

#define ALTOMULTINETDEVICE_NUM_RELAYS_DEVICES     32
#define ALTOMULTINETDEVICE_NUM_TEMP_DEVICES       32



typedef enum
{
    SERVICE_ALTO_MULTINET_DETAILED_STATUS_UUID = 0x0110,
    SERVICE_ALTO_MULTINET_LOCAL_REMOTE_UUID = 0x0B51,
} ALTOMultinet_service_UUID_t;

//Characteristic defines
typedef enum
{
    CHARACTERISTIC_ALTO_MULTINET_DETAILED_STATUS_GET_ID = 0x01,
    CHARACTERISTIC_ALTO_MULTINET_STATUS_RELAY_GET_ID,
    CHARACTERISTIC_ALTO_MULTINET_DETAILED_POT_GET_ID,
} ALTOMultinet_service_detailedStatus_characteristics_t;



typedef enum
{
    CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_ID = 0x01,
    CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_TRIGGER_ID,
    CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_GET_GATE_ID,
    CHARACTERISTIC_ALTO_MULTINET_LOCALREMOTE_SET_GATE_ID,
} TCP_CMD_ALTOMultinet_localRemote_characteristics_t;


typedef struct {
    Queue_Handle hControllersListQueue;
    uint32_t ui32InterfaceIndex;
    uint8_t  ui8VersionMajor;
    uint8_t  ui8VersionMinor;
    union {
        uint8_t ui8MultinetAddress;
        struct {
            uint8_t ui4address      : 5;
            uint8_t ui4unitType     : 3;
        };
    };
}ALTOMultinetDeviceControllerWorker_Params;





typedef struct {
    ALTOMultinetDeviceControllerWorker_Params controllerWorkerParams;
    Queue_Handle hMsgQ;
    bool bIsEnabled;
}ALTOMultinetDeviceController_Elem;


typedef struct ALTOMultinetDeviceRemoteRoutingMsgObj {
    Queue_Elem elem; /* first field for Queue */
    tALTOORTInput tORTInput;
} ALTOMultinetDeviceRemoteRoutingMsgObj, *ALTOMultinetDeviceRemoteRoutingMsg;




typedef struct {
    Queue_Handle hFreeQueue;
    ALTOMultinetDeviceController_Elem *atALTOMultinetRelayTable;
    ALTOMultinetDeviceController_Elem *atALTOMultinetTempTable;
    GateMutexPri_Handle gateMutex;
}ALTOMultinetDeviceControllerWorkerQ_Params;



typedef enum {
    ALTO_ALTONet_UnitType_Amp = 0x01,
    ALTO_Multinet_UnitType_RelayBox = 0x2,
    ALTO_Multinet_UnitType_TemperatureBox = 0x3,
}ALTOMultinetDeviceUnitType;





typedef struct {
    Queue_Elem _elem;
    Task_Handle taskHandle;
    uint32_t fatherDeviceID;
    ALTOMultinetDeviceController_Elem worker;
}ALTOMultinetDeviceControllerWorkerList_t;


typedef struct __attribute__ ((__packed__)) {
    uint8_t SwVersionMajor;
    uint8_t SwVersionMinor;
    uint8_t HwVersion;
    uint8_t reserved1;
    uint8_t unused1;
    uint8_t reserved2;
    uint8_t unused2[3];
    uint8_t multinetAddress;
    uint8_t unitSerialNumber[4];
    uint8_t unused3;
    uint8_t unitType;
    uint8_t unitSubType;
    uint8_t unitIDNumber;
    uint8_t configIDNumber;
    uint8_t mpRev;
    uint8_t mainSWPN[3];
}ALTOMultinetDeviceManufacturingInformation;


typedef struct __attribute__ ((__packed__)) {
    uint8_t overall;
    uint8_t fault;
    uint16_t v28v5;
    uint16_t v5;
    uint16_t v8;
    uint16_t v28[3];
    uint8_t relay;
    uint8_t indicator;
    uint32_t input;
    uint32_t pin;
}ALTOMultinetDeviceRelay_detailedStatus;

typedef struct __attribute__ ((__packed__)) {
    uint8_t overall;
    uint8_t fault;
    uint16_t v5;
    uint16_t v8;
    uint16_t v28;
    uint8_t relay;
    uint16_t indicator;
    uint8_t indicator16;
    uint16_t inputL;
    uint8_t inputH;
    uint32_t pin;
    uint8_t pot1Step;
    uint8_t pot2Step;
    uint8_t dac1Step;
    uint8_t dac2Step;
    uint8_t dac3Step;
}ALTOMultinetDeviceTemp_detailedStatus;



typedef struct __attribute__ ((__packed__)) {
    uint32_t pin;
    uint32_t lin;
    uint8_t relay;
    uint8_t status;
    uint32_t scratchFlags;
    uint32_t virtualRemoteInputs;
    uint32_t gate;
}ALTOMultinetDeviceRelay_CluBitMap;


typedef struct {
    union {
        uint8_t ui8MultinetAddress;
        struct {
            uint8_t ui4address      : 5;
            uint8_t ui4unitType     : 3;
        };
    };
}ALTOMultinetDevice_multinetAddress;


typedef struct {
    union {
        uint8_t ui8MultinetAddress;
        struct {
            uint8_t ui4address      : 5;
            uint8_t ui4unitType     : 3;
        };
    };
    uint8_t relayBitPosition;
}ALTOMultinetDeviceRelay_getStatusRelay;


typedef struct {
    ALTOMultinetDeviceRelay_getStatusRelay sender;
    uint8_t relayState;
}ALTOMultinetDeviceRelay_getStatusRelayResponse;

typedef struct {
    ALTOMultinetDevice_multinetAddress sender;
    uint8_t pot1Step;
    uint8_t pot2Step;
    uint8_t dac1Step;
    uint8_t dac2Step;
    uint8_t dac3Step;
}ALTOMultinetDeviceRelay_getDetailedStatusPotResponse;

typedef struct {
    union {
        uint8_t ui8MultinetAddress;
        struct {
            uint8_t ui4address      : 5;
            uint8_t ui4unitType     : 3;
        };
    };
    tALTOORTInput tORTInput;
}ALTOMultinetDeviceRelay_setSwitchOnOff;

typedef struct {
    ALTOMultinetDeviceRelay_setSwitchOnOff sender;
    int8_t acknak;
}ALTOMultinetDeviceRelay_setSwitchOnOffResponse;


typedef enum {
    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_PHYSICAL = 0,
    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SELECT_VIRTUAL = 1,
    SERVICE_ALTO_MULTINET_CLUPINVINGATE_SINGLE_UNIT = 0,
    SERVICE_ALTO_MULTINET_CLUPINVINGATE_WILDCARD = 3,
}ALTOMultinetDeviceRelay_CluPinVinGate_values;

typedef struct {
    uint8_t value           : 1;
    uint8_t inputAddress    : 5;
    uint8_t singleWildcard  : 2;
}ALTOMultinetDeviceRelay_CluPinVinGate_inputSpecifier;


#ifdef __cplusplus
extern "C"  {
#endif



#ifdef  __DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_GLOBAL
    #define __DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_EXT
#else
    #define __DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_EXT  extern
#endif


extern const Device_FxnTable g_ALTOMultinetDevice_fxnTable;

__DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_EXT
void vALTOMultinetDevice_Params_init(Device_Params *params, uint32_t address);

__DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_EXT
ALTOMultinetRemoteRoutingMsgObj *ptALTOMultinet_SendMsgToControllerWorker(Queue_Handle hControllersListQueue,
                                                                          uint8_t ui8MultinetAddress,
                                                                          tALTOORTInput *pMsg);


#ifdef __cplusplus
}
#endif



#endif /* DEVICES_ALTOMULTINETDEVICE_ALTOMULTINETDEVICE_H_ */
