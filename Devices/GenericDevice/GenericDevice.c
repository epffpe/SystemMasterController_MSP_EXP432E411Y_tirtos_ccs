/*
 * GenericDevice.c
 *
 *  Created on: Aug 8, 2018
 *      Author: epenate
 */


#define __DEVICES_GENERICDEVICE_GLOBAL
#include "includes.h"



void vGeneticDevice_close(DeviceList_Handler handle);
DeviceList_Handler hGeneticDevice_open(DeviceList_Handler handle, void *params);

const GenericDevice_hookParams g_GenericDevice_defaultHookTable =
{
 .andMask = Event_Id_NONE,
 .orMask = DEVICE_ALL_EVENTS,
 .timeout = BIOS_WAIT_FOREVER,
 .setupFxn = NULL,
 .setupArg0 = NULL,
 .eventFxns = {[0 ... 31] = NULL},
 .eventFxnArg0 = {NULL},
};

const Device_FxnTable g_GeneticDevice_fxnTable =
{
 .closeFxn = vGeneticDevice_close,
 .openFxn = hGeneticDevice_open,
 .sendMsgFxn = NULL,
};





void vGeneticDevice_Hooks_init(GenericDevice_hookParams *hooks)
{
    ASSERT(hooks != NULL);

    if (hooks) {
        *hooks = g_GenericDevice_defaultHookTable;
    }
}


uint32_t xGeneticDevice_registerHookFunctionForEventId(GenericDevice_hookParams *hooks, uint32_t eventId, Device_HookFxn hookFxn, UArg arg0)
{
    uint32_t retVal = 0;
    int index;
    uint32_t ui32EventId;

    ASSERT(hooks != NULL);

    if (hooks) {
        for (index = 0; index < 32; index++) {
            ui32EventId = (1 << index);
            if (eventId & ui32EventId) {
                hooks->eventFxns[index] = hookFxn;
                hooks->eventFxnArg0[index] = arg0;
                retVal |= ui32EventId;
            }

        }
    }

    return retVal;
}

void vGeneticDevice_registerHookFunctionForSetUp(GenericDevice_hookParams *hooks, Device_HookFxn hookFxn, UArg arg0)
{
    ASSERT(hooks != NULL);

    if (hooks) {
        hooks->setupFxn = hookFxn;
        hooks->setupArg0 = arg0;
    }
}

void vGeneticDevice_Params_init(Device_Params *params, uint32_t address, GenericDevice_hookParams *hooks)
{
    ASSERT(params != NULL);

    if (params) {
        params->deviceID = address;
        params->deviceType = DEVICE_TYPE_GENERIC;
        params->arg0 = (void *)hooks;
        params->arg1 = NULL;
        params->period = GENERIC_DEVICE_CLOCK_PERIOD;
        params->timeout = GENERIC_DEVICE_CLOCK_TIMEOUT;
        params->priority = GENERICDEVICE_TASK_PRIORITY;
        params->stackSize = GENERICDEVICE_TASK_STACK_SIZE;
        params->fxnTablePtr = (Device_FxnTable *)&g_GeneticDevice_fxnTable;
    }
}



Void vGeneticDeviceFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    DeviceList_Handler devHandle;
    Queue_Handle msgQHandle;
    Event_Handle eventHandle;
    Clock_Handle clockHandle;
    GenericDevice_hookParams hooks;
    device_msg_t *pMsg;
    int index;
    uint32_t eventId;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        Task_exit();
        return;
    }
    if (arg1 == NULL) {
        Task_exit();
        return;
    }

    GenericDevice_args args = *(GenericDevice_args *)arg0;
    hooks = *(GenericDevice_hookParams *)args.hooks;

    Memory_free(NULL, (GenericDevice_hookParams *)args.hooks, sizeof(GenericDevice_hookParams));
    args.hooks = &hooks;
    Memory_free(NULL, (GenericDevice_args *)arg0, sizeof(GenericDevice_args));

    devHandle = (DeviceList_Handler)arg1;
    eventHandle = devHandle->eventHandle;
    msgQHandle = devHandle->msgQHandle;
    clockHandle = devHandle->clockHandle;

//    if (hooks.setupFxn) hooks.setupFxn(hooks.setupArg0, arg1);
    if (hooks.setupFxn) hooks.setupFxn((UArg)&args, arg1);



    while (1) {
        events = Event_pend(eventHandle, hooks.andMask, hooks.orMask, hooks.timeout); //BIOS_WAIT_FOREVER

//        if (events & DEVICE_PERIODIC_EVT) {
//            events &= ~DEVICE_PERIODIC_EVT;
//
//        }

        if (events & DEVICE_APP_KILL_EVT) {
            events &= ~DEVICE_APP_KILL_EVT;


            Clock_stop(clockHandle);
            Clock_delete(&clockHandle);

            /*
             * Needs to flush the messages in the queue before remove it
             */
            while (!Queue_empty(msgQHandle)) {
                pMsg = Queue_get(msgQHandle);
                Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
            }
            Queue_delete(&msgQHandle);
            Event_delete(&eventHandle);
            if (hooks.eventFxns[DEVICE_APP_KILL_EVT_INDEX]) hooks.eventFxns[DEVICE_APP_KILL_EVT_INDEX](hooks.eventFxnArg0[DEVICE_APP_KILL_EVT_INDEX], arg1);
            Task_exit();
        }

        for (index = 0; index < 32; index++) {
            eventId = (1 << index);
            if (events & eventId) {
                events &= ~eventId;
                if (hooks.eventFxns[index]) hooks.eventFxns[index](hooks.eventFxnArg0[index], arg1);
            }

        }
    }
}

void vGeneticDevice_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}


void vGeneticDevice_close(DeviceList_Handler handle)
{
    unsigned int key;

//    static const int price_lookup[] = {
//                                       [APP_MSG_SERVICE_WRITE] = 6,
//                                       [APP_MSG_SERVICE_CFG] = 10,
//                                       [2 ... 10] = 55
//    };

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }

    /* Need exclusive access to prevent a race condition */
    key = Hwi_disable();
    if(handle->state.opened == true) {
        handle->state.opened = false;
        Hwi_restore(key);

        Event_post(handle->eventHandle, DEVICE_APP_KILL_EVT);
        return;
    }
    Hwi_restore(key);
}
DeviceList_Handler hGeneticDevice_open(DeviceList_Handler handle, void *params)
{
    unsigned int key;
    Error_Block eb;
    Device_Params *deviceParams;

    union {
        Task_Params        taskParams;
        Event_Params       eventParams;
        Clock_Params       clockParams;
    } paramsUnion;

    ASSERT(handle != NULL);
    ASSERT(params != NULL);


    if (handle == NULL) {
        return (NULL);
    }

    /* Need exclusive access to prevent a race condition */
    key = Hwi_disable();
    if(handle->state.opened == true) {
        Hwi_restore(key);
        Log_warning1("Device:(%p) already in use.", handle->deviceID);
        return handle;
    }
    handle->state.opened = true;
    Hwi_restore(key);

    Error_init(&eb);

    deviceParams = (Device_Params *)params;
//    handle->deviceID = deviceParams->deviceID;


    GenericDevice_args *pArgs = Memory_alloc(NULL, sizeof(GenericDevice_args), 0, &eb);

    if (pArgs) {

        pArgs->hooks = (GenericDevice_hookParams *)deviceParams->arg0;
        pArgs->arg1 = (UArg)deviceParams->arg1;
        //    Event_Params_init(&paramsUnion.eventParams);
        handle->eventHandle = Event_create(NULL, &eb);

        handle->msgQHandle = Queue_create(NULL, NULL);


        Clock_Params_init(&paramsUnion.clockParams);
        paramsUnion.clockParams.period = deviceParams->period;
        paramsUnion.clockParams.startFlag = TRUE;
        paramsUnion.clockParams.arg = (UArg)handle->eventHandle;
        handle->clockHandle = Clock_create(vGeneticDevice_clockHandler, deviceParams->timeout, &paramsUnion.clockParams, &eb);
        if (handle->clockHandle == NULL) {
            System_abort("Clock create failed");
        }

        Task_Params_init(&paramsUnion.taskParams);
        paramsUnion.taskParams.stackSize = deviceParams->stackSize;
        paramsUnion.taskParams.priority = deviceParams->priority;
        paramsUnion.taskParams.arg0 = (UArg)pArgs; //(UArg)deviceParams->arg0;
        paramsUnion.taskParams.arg1 = (UArg)handle;
        handle->taskHandle = Task_create((Task_FuncPtr)vGeneticDeviceFxn, &paramsUnion.taskParams, &eb);

        return handle;
    }
    return NULL;
}

