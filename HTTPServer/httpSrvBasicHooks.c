/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *    ======== httpSrvBasicHooks.c ========
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnetif.h>
#include <ti/net/slnetutils.h>

#include <ti/display/Display.h>
#include <ti/drivers/emac/EMACMSP432E4.h>

#define HTTPSTACKSIZE   8192
#define IF_PRIORITY     1
#define SERVER_PORT     80
#define SEC_SERVER_PORT 443

/* Prototypes */
extern Display_Handle display;
extern void *serverFxn(void *arg0);

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
    uint32_t            hostByteAddr;
    static bool         createTask = true;
    int32_t             status = 0;
    static uint16_t     http_port = SERVER_PORT;
    static uint16_t     https_port = SEC_SERVER_PORT;

    if (fAdd) {
        Display_printf(display, 0, 0, "Network Added: ");
    }
    else {
        Display_printf(display, 0, 0, "Network Removed: ");
    }

    /* print the IP address that was added/removed */
    hostByteAddr = NDK_ntohl(IPAddr);
    Display_printf(display, 0, 0, "If-%d:%d.%d.%d.%d\n", IfIdx,
            (uint8_t)(hostByteAddr>>24)&0xFF, (uint8_t)(hostByteAddr>>16)&0xFF,
            (uint8_t)(hostByteAddr>>8)&0xFF, (uint8_t)hostByteAddr&0xFF);

    /* Initialize the NS library */

    status = SlNetSock_init(0);
    if (status != 0)
    {
        Display_printf(display, 0, 0, "SlNetSock_init fail (%d)\n",
            status);
    }

    status = SlNetIf_init(0);
    if (status != 0)
    {
        Display_printf(display, 0, 0, "SlNetIf_init fail (%d)\n",
            status);
    }

    status = SlNetUtil_init(0);
    if (status != 0)
    {
        Display_printf(display, 0, 0, "SlNetUtil_init fail (%d)\n",
            status);
    }

    /* Add an NDK interface with security enabled */
    status = SlNetIf_add(SLNETIF_ID_2, EMACMSP432E4_ETHERNET_NAME,
            (const SlNetIf_Config_t *)&SlNetIfConfigNDKSec, IF_PRIORITY);
    if (status != 0)
    {
        Display_printf(display, 0, 0, "SlNetIf_add fail (%d)\n",
            status);
    }

    if (fAdd && createTask)
    {
        /* Set priority and stack size attributes */
        pthread_attr_init(&attrs);
        priParam.sched_priority = 1;

        detachState = PTHREAD_CREATE_DETACHED;
        retc = pthread_attr_setdetachstate(&attrs, detachState);
        if (retc != 0)
        {
            Display_printf(display, 0, 0,
                    "httpSrvBasicHooks main: pthread_attr_setdetachstate() "
                    "failed\n");
            while (1);
        }

        pthread_attr_setschedparam(&attrs, &priParam);

        retc |= pthread_attr_setstacksize(&attrs, HTTPSTACKSIZE);
        if (retc != 0)
        {
            Display_printf(display, 0, 0,
                    "httpSrvBasicHooks main: pthread_attr_setstacksize() "
                    "failed\n");
            while (1);
        }

        /* Begin the thread that will start the INSECURE server */
        retc = pthread_create(&thread, &attrs, serverFxn, (void *)&http_port);
        if (retc != 0)
        {
            Display_printf(display, 0, 0,
                    "httpSrvBasicHooks main: http pthread_create() failed\n");
            while (1);
        }

        /* Begin the thread that will start the SECURE server */
        retc = pthread_create(&thread, &attrs, serverFxn, (void *)&https_port);
        if (retc != 0)
        {
            Display_printf(display, 0, 0,
                    "httpSrvBasicHooks main: https pthread_create() failed\n");
            while (1);
        }

        createTask = false;
    }
}

/*
 *  ======== serviceReportHook ========
 *  NDK service report hook
 */
void serviceReportHook(uint32_t item, uint32_t status, uint32_t report, void *h)
{
    static char *taskName[] = {"Telnet", "HTTP", "NAT", "DHCPS", "DHCPC", "DNS"};
    static char *reportStr[] = {"", "Running", "Updated", "Complete", "Fault"};
    static char *statusStr[] =
        {"Disabled", "Waiting", "IPTerm", "Failed","Enabled"};

    Display_printf(display, 0, 0, "Service Status: %-9s: %-9s: %-9s: %03d\n",
            taskName[item - 1], statusStr[status], reportStr[report / 256],
            report & 0xFF);
}

/*
 *  ======== netOpenHook ========
 *  NDK network open hook
 */
void netOpenHook()
{
}
