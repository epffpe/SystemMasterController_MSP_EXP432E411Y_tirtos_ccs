/*
 * GenericTemplateDevice.c
 *
 *  Created on: Sep 19, 2018
 *      Author: epenate
 */


#define __DEVICES_GENERICDEVICE_TEMPLATE_GENERICTEMPLATEDEVICE_GLOBAL
#include "includes.h"



void vGenericTemplateDevice_setupHook(UArg arg0, UArg arg1)
{
    GenericDevice_hookParams *pHooks;
//    DeviceList_Handler devHandle;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    if (arg0 == NULL) {
        return;
    }
    if (arg1 == NULL) {
        return;
    }
//    devHandle = (DeviceList_Handler)arg1;

    /*
     * arg0 is a pointer to GenericDevice_hookParams struct so it can be used to share data with the other hooks functions
     */
    GenericDevice_args *pArgs = (GenericDevice_args *)arg0;
    pHooks = (GenericDevice_hookParams *)pArgs->hooks;

    pHooks->setupArg0 = pHooks->setupArg0;
}

void vGenericTemplateDevice_periodicEventHook(UArg arg0, UArg arg1)
{
//    DeviceList_Handler devHandle;
//
//    devHandle = (DeviceList_Handler)arg1;
}



void vGenericTemplateDevice_appMsgEventHook(UArg arg0, UArg arg1)
{
//    DeviceList_Handler devHandle;
//
//    devHandle = (DeviceList_Handler)arg1;
}


void vGenericTemplateDevice_Params_init(Device_Params *params, uint32_t address)
{
    uint32_t ui32EventId;
    Error_Block eb;
    UArg arg0 = NULL;

    ASSERT(params != NULL);

    if (params == NULL) {
        return;
    }

    Error_init(&eb);

    GenericDevice_hookParams *pHooks = Memory_alloc(NULL, sizeof(GenericDevice_hookParams), 0, &eb);
    if (pHooks != NULL)
    {
        vGeneticDevice_Hooks_init(pHooks);

        vGeneticDevice_registerHookFunctionForSetUp(pHooks, vGenericTemplateDevice_setupHook, arg0);

        /*
         * Needs to change vGenericTemplateDevice_periodicEventHook
         */
        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_PERIODIC_EVT, vGenericTemplateDevice_periodicEventHook, arg0);
        /*
         * Needs to change vGenericTemplateDevice_appMsgEventHook
         */
        ui32EventId = xGeneticDevice_registerHookFunctionForEventId(pHooks, DEVICE_APP_MSG_EVT, vGenericTemplateDevice_appMsgEventHook, arg0);

        vGeneticDevice_Params_init(params, address, pHooks);
    }
    (void)ui32EventId;
}



