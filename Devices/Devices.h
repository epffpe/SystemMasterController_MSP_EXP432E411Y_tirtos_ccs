/*
 * Devices.h
 *
 *  Created on: Mar 19, 2018
 *      Author: epenate
 */

#ifndef DEVICES_DEVICES_H_
#define DEVICES_DEVICES_H_



#define DEVICES_MAX_NUM_LIST            32
#define DEVICES_LIST_BLOCK_SIZE         40


#ifdef __cplusplus
extern "C"  {
#endif



// Internal Events for RTOS application
//#define PRZ_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
//#define PRZ_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define DEVICE_STATE_CHANGE_EVT                  Event_Id_00
#define DEVICE_CHAR_CHANGE_EVT                   Event_Id_01
#define DEVICE_PERIODIC_EVT                      Event_Id_02
#define DEVICE_APP_MSG_EVT                       Event_Id_03
#define DEVICE_APP_KILL_EVT                      Event_Id_09
#define DEVICE_APP_KILL_EVT_INDEX                9
#define DEVICE_CONN_EVT_END_EVT                  Event_Id_30

#define DEVICE_ALL_EVENTS                       (DEVICE_STATE_CHANGE_EVT        | \
                                                DEVICE_CHAR_CHANGE_EVT          | \
                                                DEVICE_PERIODIC_EVT             | \
                                                DEVICE_APP_MSG_EVT              | \
                                                DEVICE_APP_KILL_EVT             | \
                                                DEVICE_CONN_EVT_END_EVT)


/*********************************************************************
 * TYPEDEFS
 */


typedef enum
{
  APP_MSG_ERROR_TRANSFER_FAILED = 1,
  APP_MSG_ERROR_CRC,
} device_msg_error_t;


// Types of messages that can be sent to the user application task from other
// tasks or interrupts. Note: Messages from BLE Stack are sent differently.
typedef enum
{
  APP_MSG_SERVICE_WRITE = 0,   /* A characteristic value has been written     */
  APP_MSG_SERVICE_CFG,         /* A characteristic configuration has changed  */
  APP_MSG_UPDATE_CHARVAL,      /* Request from ourselves to update a value    */
  APP_MSG_GAP_STATE_CHANGE,    /* The GAP / connection state has changed      */
  APP_MSG_SEND_PASSCODE,       /* A pass-code/PIN is requested during pairing */
  APP_MSG_ERROR_HANDLER,       /* An error has ocurred on previous message */
  APP_MSG_RAW,
} device_msg_types_t;

// Struct for messages sent to the application task
typedef struct
{
  Queue_Elem       _elem;
  device_msg_types_t  type;
  IHeap_Handle     heap;
  uint16_t         pduLen;
  uint8_t          pdu[];
} device_msg_t;

// Struct for messages about characteristic data
typedef struct
{
    int32_t     connHandle;
    uint32_t    retDeviceID;
    uint16_t    svcUUID; // UUID of the service
    uint16_t    retSvcUUID;
    uint16_t    dataLen; //
    uint16_t    paramID; // Index of the characteristic
    uint16_t    retParamID;
    uint8_t     data[];  // Flexible array member, extended to malloc - sizeof(.)
} char_data_t;

// Struct for message about sending/requesting passcode from peer.
typedef struct
{
  uint16_t  connHandle;
  uint8_t   uiInputs;
  uint8_t   uiOutputs;
  uint32_t  numComparison;
} passcode_req_t;

// Struct for message about button state
typedef struct
{
  uint8_t   pinId;
  uint8_t   state;
} button_state_t;



typedef struct DevicesList_t *DeviceList_Handler;

typedef void        (*Device_CloseFxn)    (DeviceList_Handler handle);
typedef void        (*Device_HookFxn)     (UArg arg0, UArg arg1);

typedef DeviceList_Handler  (*Device_OpenFxn)     (DeviceList_Handler handle,
                                                    void *params);

typedef void (*Device_enqueueDataMsg) ( DeviceList_Handler devHandle,
                                         device_msg_types_t appMsgType,
                                         int32_t connHandle,
                                         uint16_t serviceUUID, uint8_t paramID,
                                         uint32_t retDeviceID,
                                         uint16_t retServiceUUID, uint8_t retParamID,
                                         uint8_t *pValue, uint16_t len );

typedef struct Device_FxnTable {
    /*! Function to close the specified Device */
    Device_CloseFxn        closeFxn;

    /*! Function to implementation specific control function */
//    IF_ControlFxn      controlFxn;


    /*! Function to open the specified peripheral */
    Device_OpenFxn         openFxn;

    Device_enqueueDataMsg sendMsgFxn;
    /*! Function to initiate a IF data transfer */
//    IF_SendMsgFxn     SendMsgFxn;
} Device_FxnTable;

typedef enum
{
    DEVICE_TYPE_TEST = 1,
    DEVICE_TYPE_ALTO_AMP,
    DEVICE_TYPE_ALTO_MULTINET,
    DEVICE_TYPE_TCP_REMOTE_CONTROLLER_BINARY,
    DEVICE_TYPE_TCP_REMOTE_CONTROLLER_ASCII,
    DEVICE_TYPE_GENERIC,
    DEVICE_TYPE_TF_UART_TEST,
    DEVICE_TYPE_DUT_UART_TEST,
    DEVICE_TYPE_AVDS,
    DEVICE_TYPE_ROSEN,
    DEVICE_TYPE_ROSEN485,

    DEVICE_TYPE_COUNT
} device_types_t;

typedef struct Device_Params {
    uint32_t        deviceID;
    uint32_t        deviceType;
    void            *arg0;
    void            *arg1;
    uint32_t        period;
    uint32_t        timeout;
    uint32_t        priority;
    uint32_t        stackSize;
    Device_FxnTable *fxnTablePtr;
} Device_Params;

typedef struct DevicesList_t{
    Queue_Elem elem;
    uint32_t deviceID;
    device_types_t deviceType;
    Device_FxnTable *fxnTablePtr;
    Task_Handle  taskHandle;
    Queue_Handle msgQHandle;
    Event_Handle eventHandle;
    Clock_Handle clockHandle;
    struct {
        bool            opened:1;         /* Has the obj been opened */
    } state;
}DevicesList_t;


#ifdef  __DEVICES_GLOBAL
    #define __DEVICES_EXT
#else
    #define __DEVICES_EXT  extern
#endif

__DEVICES_EXT Queue_Handle g_hDevicesListQ;
extern const char *g_Device_nameByType[DEVICE_TYPE_COUNT];

__DEVICES_EXT
void vDevice_init();

__DEVICES_EXT
DeviceList_Handler xDevice_add(Device_Params *params, Error_Block *eb);

__DEVICES_EXT
void vDevice_remove(DeviceList_Handler handle);

__DEVICES_EXT
void vDevice_enqueueCharDataMsg( DeviceList_Handler devHandle,
                                 device_msg_types_t appMsgType,
                                 int32_t connHandle,
                                 uint16_t serviceUUID, uint8_t paramID,
                                 uint32_t retDeviceID,
                                 uint16_t retServiceUUID, uint8_t retParamID,
                                 uint8_t *pValue, uint16_t len );

__DEVICES_EXT
void vDevice_enqueueRawAppMsg(DeviceList_Handler devHandle,
                              device_msg_types_t appMsgType,
                              uint8_t *pData,
                              uint16_t len);
__DEVICES_EXT
void vDevice_enqueueRawAppMsgAndEvent(DeviceList_Handler devHandle,
                                      device_msg_types_t appMsgType,
                                      uint8_t *pData,
                                      uint16_t len,
                                      UInt eventMask);

__DEVICES_EXT
void vDevice_sendCharDataMsg( uint32_t deviceID,
                              device_msg_types_t appMsgType,
                              int32_t connHandle,
                              uint16_t serviceUUID, uint8_t paramID,
                              uint32_t retDeviceID,
                              uint16_t retServiceUUID, uint8_t retParamID,
                              uint8_t *pValue, uint16_t len );


__DEVICES_EXT
uint32_t xDevice_sendCharDataMsg( uint32_t deviceID,
                                  device_msg_types_t appMsgType,
                                  int32_t connHandle,
                                  uint16_t serviceUUID, uint8_t paramID,
                                  uint32_t retDeviceID,
                                  uint16_t retServiceUUID, uint8_t retParamID,
                                  uint8_t *pValue, uint16_t len );

__DEVICES_EXT
uint32_t xDevice_sendErrorMsg( uint32_t deviceID,
                               int32_t connHandle,
                               uint16_t serviceUUID, uint8_t paramID,
                               uint32_t retDeviceID,
                               uint16_t retServiceUUID, uint8_t retParamID,
                               device_msg_error_t error );

#ifdef __cplusplus
}
#endif



#include "Devices/ALTOMultinetDevice/ALTOMultinetDevice.h"

#endif /* DEVICES_DEVICES_H_ */
