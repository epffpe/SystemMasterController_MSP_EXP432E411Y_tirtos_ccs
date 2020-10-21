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
 *    ======== httpSrvBasic.c ========
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <ti/net/http/httpserver.h>
#include <ti/net/http/urlhandler.h>
#include <ti/net/slnetif.h>
#include <ti/net/slnetsock.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/display/Display.h>

#include "HTTPServer/urlsimple.h"
#include "HTTPServer/urlsimpleput.h"
#include "HTTPServer/memzip/urlmemzip.h"
#include "certs/certificate.h"
#include "DISPLAY/SMCDisplay.h"

#include "includes.h"



extern Display_Handle display;
extern void startSNTP(void);
extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

#define SECURE_PORT             443
#define NUM_URLHANDLERS         3
#define SERVER_BACKLOG_COUNT    3

/*
 * ADD_ROOT_CA should be set to 1 if you want to verify client certificates with
 * a root certificate. A root certificate is not provided with this example. You
 * will also need to add a certificate chain to your client that your chosen
 * root certificate trusts. Our python-based client included with this example
 * does not provide a certificate for verification.
 */
#define ADD_ROOT_CA             0

/* Secure object names */
#define ROOT_CA_CERT_FILE     "DummyCA"
#define PRIVATE_KEY_FILE      "DummyKey"
#define TRUSTED_CERT_FILE     "DummyTrustedCert"

/*
 * Structure that is passed to HTTPServer_create. It defines the callbacks
 * used by the server as it parses requests.
 */
URLHandler_Setup handlerTable[] =
{
    {
        NULL,
        NULL,
        NULL,
        URLSimple_process,
        NULL,
        NULL
    },
    {
        NULL,
        NULL,
        NULL,
        URLSimplePut_process,
        NULL,
        NULL
    },
#if 0
    {
        NULL,
        URLStop_create,
        URLStop_delete,
        URLStop_process,
        NULL,
        NULL
    },

    {
        NULL,
        URLSender_create,
        URLSender_delete,
        URLSender_process,
        NULL,
        URLSender_send
    },
#endif
    {
        NULL,
        NULL,
        NULL,
        URLMemzip_process,
        NULL,
        NULL
    }
};

/*
 * ======== serverFxn ========
 *
 * Thread started by netIPAddrHook in httpSrvBasicHooks.c once the NDK has
 * acquired an IP address. This thread initializes and starts a simple HTTP
 * server.
 */
void *serverFxn(void *arg)
{
    HTTPServer_Handle       srv;
    struct                  sockaddr_in addr;
    int                     status = 0;
    uint16_t                port = *(uint16_t *)arg;
    SlNetSockSecAttrib_t    *secAttribs;

    int retVal;

    fdOpenSession(TaskSelf());

    Display_printf(g_SMCDisplay, 0, 0, "*** Initializing HTTP Server ***\n");
    HTTPServer_init();

    /* Create the HTTPServer and pass in your array of handlers */
    Display_printf(g_SMCDisplay, 0, 0, "\nCreating the HTTPServer and pass in the array of handlers");
    if ((srv = HTTPServer_create(handlerTable, NUM_URLHANDLERS, NULL)) == NULL)
    {
        Display_printf(g_SMCDisplay, 0, 0,
                       "Failed to create HTTPServer> 0x%p\n", srv);
//        goto reboot;
    }

    /*
     * Create a connection point for the server.
     */
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(port);

    startSNTP();
    if (port == SECURE_PORT)
    {
        /* Load security parameters */
        #if ADD_ROOT_CA
            /*
             * this SlNetIf_loadSecObj() adds a Root CA to the server that it
             * would use to verify the certificates of incoming clients
             */
            status |= SlNetIf_loadSecObj(SLNETIF_SEC_OBJ_TYPE_CERTIFICATE,
                                        ROOT_CA_CERT_FILE,
                                        strlen(ROOT_CA_CERT_FILE), cliCAPem,
                                        cliCAPemLen, SLNETIF_ID_2);
        #endif
        status |= SlNetIf_loadSecObj(SLNETIF_SEC_OBJ_TYPE_CERTIFICATE,
                                     TRUSTED_CERT_FILE,
                                     strlen(TRUSTED_CERT_FILE), srvCertPem,
                                     srvCertPemLen, SLNETIF_ID_2);
        status |= SlNetIf_loadSecObj(SLNETIF_SEC_OBJ_TYPE_RSA_PRIVATE_KEY,
                                     PRIVATE_KEY_FILE, strlen(PRIVATE_KEY_FILE),
                                     srvKeyPem, srvKeyPemLen, SLNETIF_ID_2);
        if (status < 0)
        {
            Display_printf(g_SMCDisplay, 0, 0, "Failed to load security attributes "
                           "- status (%d)\n", status);
            exit(1);
        }

        secAttribs = SlNetSock_secAttribCreate();

        #if ADD_ROOT_CA
            /*
             *  Set the ROOT CA for the example client - this enables/enforces
             *  client authentication
             */
            status |= SlNetSock_secAttribSet(secAttribs,
                                            SLNETSOCK_SEC_ATTRIB_PEER_ROOT_CA,
                                            ROOT_CA_CERT_FILE,
                                            sizeof(ROOT_CA_CERT_FILE));
        #endif
        /* Set the server's personal identification certificate chain */
        status |= SlNetSock_secAttribSet(secAttribs,
                                         SLNETSOCK_SEC_ATTRIB_LOCAL_CERT,
                                         TRUSTED_CERT_FILE,
                                         sizeof(TRUSTED_CERT_FILE));
        /* Set the server's private key */
        status |= SlNetSock_secAttribSet(secAttribs,
                                         SLNETSOCK_SEC_ATTRIB_PRIVATE_KEY,
                                         PRIVATE_KEY_FILE,
                                         sizeof(PRIVATE_KEY_FILE));
        if (status < 0)
        {
            Display_printf(g_SMCDisplay, 0, 0, "Failed to set security attributes "
                           "- status (%d)\n", status);
            exit(1);
        }

        HTTPServer_enableSecurity(srv, secAttribs, true);
    }


    Display_printf(g_SMCDisplay, 0, 0, "... Starting HTTP Server on port %d ...\n",
                   port);
    retVal = HTTPServer_serveSelect(srv, (struct sockaddr *)&addr, sizeof(addr),
                                    SERVER_BACKLOG_COUNT);
    Display_printf(g_SMCDisplay, 0, 0, "... HTTP Server Stopped on port %d ...", port);

    switch(retVal) {
    case 0:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server received stop command");
        break;
    case -1:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server shutdown unexpectedly");
        break;
    case -2:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal accept() call failed");
        break;
    case -3:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal network socket creation failure");
        break;
    case -4:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal bind() call failed");
        break;
    case -5:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal listen() call failed");
        break;
    case -6:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal memory allocation or object creation failure");
        break;
    case -7:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal mq creation failure");
        break;
    case -8:
        Display_printf(g_SMCDisplay, 0, 0, "  -> HTTP Server Internal mutex-related failure");
        break;
    default:
        break;
    }

    HTTPServer_delete(&srv);
//    Task_sleep((unsigned int)1000);

//    reboot:
    fdCloseSession(TaskSelf());


    SFFS_Handle hSFFS;
    hSFFS = hSFFS_open(SFFS_Internal);
    xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);
//
//    hSFFS = hSFFS_open(SFFS_External);
//    xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);

    Task_sleep(50);

//      wait for flash memory mutex

//        WatchdogUnlock(WATCHDOG0_BASE);
//        WatchdogResetDisable(WATCHDOG0_BASE);
    //    USBDCDTerm(0);
    USBDevDisconnect(USB0_BASE);

    Task_sleep(50);

    SysCtlReset();

    return (NULL);
}
