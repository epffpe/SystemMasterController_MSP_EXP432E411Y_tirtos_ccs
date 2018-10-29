/*
 * Rosen.c
 *
 *  Created on: Feb 13, 2018
 *      Author: epenate
 */

#define __ROSEN_GLOBAL
#include "includes.h"



Void vROSENBlueRayDVDFxnV00(UArg arg0, UArg arg1)
{
    tsROSENCommand cmd;

    GPIO_write(SMC_SERIAL2_DE, 1);
    GPIO_write(SMC_SERIAL2_RE, 1); //disable read

    hSerialChangeSpeed(Board_Serial2, 9600);

    xROSENBlueRayDVDCreateMsgFrame(&cmd,
                                   ROSENSingleBlueRayDVDCommand_Menu_Up,
                                   ROSSEN_BlueRayDVD_Network_Address_20);

//    xBSPSerial_sendData(Board_Serial2, (const char *)&cmd, sizeof(tsROSENCommand), 30);
    while(1) {
        Task_sleep(1000);
        xBSPSerial_sendData(Board_Serial2, (const char *)&cmd, sizeof(tsROSENCommand), 30);
    }
}

Task_Handle xROSEN_init(void *params)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.stackSize = ROSEN_TASK_STACK_SIZE;
    taskParams.priority = ROSEN_TASK_PRIORITY;
    taskParams.arg0 = (UArg)params;
    taskHandle = Task_create((Task_FuncPtr)vROSENBlueRayDVDFxnV00, &taskParams, &eb);

    return taskHandle;
}


uint32_t xROSENCreateMsgFrame(tsROSENCommand *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address)
{
    ASSERT(ptsCmdBuf != NULL);

    if (ptsCmdBuf == NULL) {
        return 0;
    }
    ptsCmdBuf->header = header;
    ptsCmdBuf->command = command;
    ptsCmdBuf->address = address;
    return 3;
}

