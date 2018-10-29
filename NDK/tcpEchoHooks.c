/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *    ======== tcpEchoHooks.c ========
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <pthread.h>

#include <ti/ndk/inc/netmain.h>

#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnetif.h>

#include <ti/display/Display.h>

#define TCPPORT 1000

#define TCPHANDLERSTACK 2048
#define IFPRI  4   /* Ethernet interface priority */

/* Prototypes */
extern void *tcpHandler(void *arg0);

extern Display_Handle g_SMCDisplay;

/*
 *  ======== netIPAddrHook ========
 *  user defined network IP address hook
 */
void netIPAddrHook(uint32_t IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;
    static uint16_t arg0 = TCPPORT;
    static bool createTask = true;
    int32_t status = 0;

    status = SlNetSock_init(0);
    if (status != 0) {
        Display_printf(g_SMCDisplay, 0, 0, "SlNetSock_init fail (%d)\n",
            status);
    }

    status = SlNetIf_init(0);
    if (status != 0) {
        Display_printf(g_SMCDisplay, 0, 0, "SlNetIf_init fail (%d)\n",
            status);
    }

    status = SlNetIf_add(SLNETIF_ID_2, "eth0",
            (const SlNetIf_Config_t *)&SlNetIfConfigNDK, IFPRI);
    if (status != 0) {
        Display_printf(g_SMCDisplay, 0, 0, "SlNetIf_add fail (%d)\n",
            status);
    }

    if (fAdd && createTask) {
        /*
         *  Create the Task that farms out incoming TCP connections.
         *  arg0 will be the port that this task listens to.
         */

        /* Set priority and stack size attributes */
        pthread_attr_init(&attrs);
        priParam.sched_priority = 1;

        detachState = PTHREAD_CREATE_DETACHED;
        retc = pthread_attr_setdetachstate(&attrs, detachState);
        if (retc != 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                    "netIPAddrHook: pthread_attr_setdetachstate() failed\n");
            while (1);
        }

        pthread_attr_setschedparam(&attrs, &priParam);

        retc |= pthread_attr_setstacksize(&attrs, TCPHANDLERSTACK);
        if (retc != 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                    "netIPAddrHook: pthread_attr_setstacksize() failed\n");
            while (1);
        }

        retc = pthread_create(&thread, &attrs, tcpHandler, (void *)&arg0);
        if (retc != 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                    "netIPAddrHook: pthread_create() failed\n");
            while (1);
        }

        createTask = false;
    }
}


/*
 *  ======== netOpenHook ========
 *  NDK network open hook
 */
void netOpenHook()
{
}
