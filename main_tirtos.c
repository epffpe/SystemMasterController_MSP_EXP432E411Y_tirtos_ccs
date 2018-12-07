/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,

 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_tirtos.c ========
 */
//#include <stdint.h>
//
///* POSIX Header files */
//#include <pthread.h>
//
///* RTOS header files */
//#include <ti/sysbios/BIOS.h>
//
///* Example/Board Header files */
//#include "Board.h"
#include "includes.h"

extern void *mainThread(void *arg0);
extern void *SMC_initThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE    2048

/*
 * The following (weak) function definition is needed in applications
 * that do *not* use the NDK TCP/IP stack:
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak void NDK_hookInit(int32_t id) {}
#elif defined(__GNUC__) && !defined(__ti__)
void __attribute__((weak)) NDK_hookInit(int32_t id) {}
#else
#pragma WEAK (NDK_hookInit)
void NDK_hookInit(int32_t id) {}
#endif


extern void ti_ndk_config_Global_startupFxn();


void programEMACAddress()
{
    uint32_t ulUser0, ulUser1;

    ulUser0 = 0x00B61A00;
    ulUser1 = 0x0055BF03;
    FlashUserSet(ulUser0, ulUser1);
    FlashUserSave();
}

//Void inittask(UArg arg0, UArg arg1)
//{
//    SMC_initThread(NULL);
//}
//
//void loadInitialization()
//{
//    Task_Params taskParams;
//    Error_Block eb;
//    /* Make sure Error_Block is initialized */
//    Error_init(&eb);
//
//    Task_Params_init(&taskParams);
//    taskParams.stackSize = THREADSTACKSIZE;
//    taskParams.priority = 9;
//    Task_create((Task_FuncPtr)inittask, &taskParams, &eb);
//
//}

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Call driver init functions */
    Board_initGeneral();

//    programEMACAddress();

    INFO_init();


    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 9;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* failed to set attributes */
        while (1) {}
    }

//    loadInitialization();
//    retc = pthread_create(&thread, &attrs, mainThread, NULL);
    retc = pthread_create(&thread, &attrs, SMC_initThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1) {}
    }

    ti_ndk_config_Global_startupFxn();

    System_printf("Starting the System Master Controller\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n"
                  "-- Compiled: "__DATE__" "__TIME__" --\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    BIOS_start();

    return (0);
}
