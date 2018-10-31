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
 *    ======== udpEcho.c ========
 *    Contains BSD sockets code.
 */

#include <string.h>
#include <stdint.h>

#include <pthread.h>
/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <ti/net/slnetutils.h>

#include <ti/display/Display.h>

#define UDPPACKETSIZE 1472


extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

extern Display_Handle g_SMCDisplay;

/*
 *  ======== echoFxn ========
 *  Echoes UDP messages.
 *
 */
void *echoFxn(void *arg0)
{
    int                bytesRcvd;
    int                bytesSent;
    int                status;
    int                server;
    fd_set             readSet;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    socklen_t          addrlen;
    char               buffer[UDPPACKETSIZE];
    uint16_t           portNumber = *(uint16_t *)arg0;
    uint32_t IPTmp;
    char response[] = "System Master Controller ID: 0123456789";

    fdOpenSession(TaskSelf());

    Display_printf(g_SMCDisplay, 0, 0, "UDP Echo example started\n");

    server = socket(AF_INET, SOCK_DGRAM, 0);
    if (server == -1) {
        Display_printf(g_SMCDisplay, 0, 0, "Error: socket not created.\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(portNumber);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        Display_printf(g_SMCDisplay, 0, 0, "Error: bind failed.\n");
        goto shutdown;
    }

    do {
        /*
         *  readSet and addrlen are value-result arguments, which must be reset
         *  in between each select() and recvfrom() call
         */
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);
        addrlen = sizeof(clientAddr);

        /* Wait forever for the reply */
        status = select(server + 1, &readSet, NULL, NULL, NULL);
        if (status > 0) {
            if (FD_ISSET(server, &readSet)) {
                bytesRcvd = recvfrom(server, buffer, UDPPACKETSIZE, 0,
                        (struct sockaddr *)&clientAddr, &addrlen);
                buffer[bytesRcvd] = 0;
                if (bytesRcvd > 0) {
                    IPTmp = ntohl(clientAddr.sin_addr.s_addr);
                    Display_printf(g_SMCDisplay, 0, 0, "remoteIp:\t:%d.%d.%d.%d:%d\n", (uint8_t)(IPTmp>>24)&0xFF,
                                   (uint8_t)(IPTmp>>16)&0xFF,
                                   (uint8_t)(IPTmp>>8)&0xFF,
                                   (uint8_t)IPTmp&0xFF,
                                   clientAddr.sin_port
                    );
                    bytesSent = sendto(server, response, sizeof(response), 0,
                                                (struct sockaddr *)&clientAddr, addrlen);
                    if(bytesSent != sizeof(response)){
                        Display_printf(g_SMCDisplay, 0, 0, "Error: sendto failed.\n");

                    }
//                    bytesSent = sendto(server, buffer, bytesRcvd, 0,
//                            (struct sockaddr *)&clientAddr, addrlen);
//                    if (bytesSent < 0 || bytesSent != bytesRcvd) {
//                        Display_printf(g_SMCDisplay, 0, 0,
//                                "Error: sendto failed.\n");
//                        goto shutdown;
//                    }
                }
            }
        }
    } while (status > 0);

shutdown:
    if (server != -1) {
        close(server);
    }

    fdCloseSession(TaskSelf());

    return (NULL);
}
