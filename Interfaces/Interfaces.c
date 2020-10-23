/*
 * Interfaces.c
 *
 *  Created on: Feb 23, 2018
 *      Author: epenate
 */

#define __INTERFACES_GLOBAL
#include "includes.h"


/* Externs */
//extern const IF_Config IF_config[];


/* Used to check status and initialization */
static int IF_count = -1;

/* Default IF parameters structure */
//const IF_Params IF_defaultParams = {
////    IF_MODE_BLOCKING,  /* transferMode */
////    NULL,               /* transferCallbackFxn */
////    IF_100kHz,         /* bitRate */
//    (uintptr_t) NULL    /* custom */
//};





void vIF_close(IF_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}

int xIF_control(IF_Handle handle, unsigned int cmd, void* arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}

void vIF_init(void)
{
    Display_printf(g_SMCDisplay, 0, 0, "Initializing Interfaces\n");
    if (IF_count == -1) {
        /* Call each driver's init function */
        for (IF_count = 0; IF_config[IF_count].fxnTablePtr != NULL; IF_count++) {
            IF_config[IF_count].fxnTablePtr->initFxn((IF_Handle)&(IF_config[IF_count]));
        }
    }
}

IF_Handle hIF_open(unsigned int index, IF_Params* params)
{
    IF_Handle handle;

    if (index >= IF_count) {
        return (NULL);
    }

    /* If params are NULL use defaults. */
    if (params == NULL) {
//        params = (IF_Params *) &IF_defaultParams;
        return (NULL);
    }

    /* Get handle for this driver instance */
    handle = (IF_Handle)&(IF_config[index]);

    return (handle->fxnTablePtr->openFxn(handle, params));
}

void vIF_Params_init(IF_Params* params, teIF_ParamsType paramType)
{
    switch(paramType) {
    case IF_Params_Type_Uart:
        UART_Params_init(&params->uartParams);
        break;
    default:
        break;
    }
}

bool bIF_transfer(IF_Handle handle, IF_Transaction* transaction)
{
    return (handle->fxnTablePtr->transferFxn(handle, transaction));
}

