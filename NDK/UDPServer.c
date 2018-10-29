/*
 * UDPServer.c
 *
 *  Created on: Apr 16, 2017
 *      Author: epf
 */


#define UDPSERVER_GLOBALS
#include "includes.h"

#define UDPPACKETSIZE 256

/*
 *  ======== echoFxn ========
 *  Echoes UDP messages.
 *
 */
Void udpHandlerFxn(UArg arg0, UArg arg1)
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
    char response[] = "System Master Controller ID: 0123456789";

    uint32_t IPTmp;

    // Open the file session
    fdOpenSession((void *)Task_self());

    server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server == -1) {
        System_printf("Error: socket not created.\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(arg0);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        System_printf("Error: bind failed.\n");
        goto shutdown;
    }
    System_printf("UDP server\n");
    System_flush();
    do {
        /*
         *  readSet and addrlen are value-result arguments, which must be reset
         *  in between each select() and recvfrom() call
         */
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);
        addrlen = sizeof(clientAddr);
        System_printf("Waiting for UDP broadcast message\n");
        System_flush();
        /* Wait forever for the reply */
        status = select(server + 1, &readSet, NULL, NULL, NULL);

        System_printf("Status: 0x%x\n", status);
        System_flush();
        if (status > 0) {
            if (FD_ISSET(server, &readSet)) {
                bytesRcvd = recvfrom(server, buffer, UDPPACKETSIZE, 0,
                        (struct sockaddr *)&clientAddr, &addrlen);
                buffer[bytesRcvd] = 0;
                System_printf("UDP -> %s\n", buffer);
//                System_flush();
                if (bytesRcvd > 0) {
//                    clientAddr.sin_port = htons(1234);
                    IPTmp = ntohl(clientAddr.sin_addr.s_addr);
                    System_printf("remoteIp:\t:%d.%d.%d.%d:%d\n", (uint8_t)(IPTmp>>24)&0xFF,
                                  (uint8_t)(IPTmp>>16)&0xFF,
                                  (uint8_t)(IPTmp>>8)&0xFF,
                                  (uint8_t)IPTmp&0xFF,
                                  clientAddr.sin_port
                                  );

                    bytesSent = sendto(server, response, sizeof(response), 0,
                            (struct sockaddr *)&clientAddr, addrlen);
//                    bytesSent = sendto(server, buffer, bytesRcvd, 0,
//                            (struct sockaddr *)&clientAddr, addrlen);
//                    if (bytesSent < 0 || bytesSent != bytesRcvd) {
//                        System_printf("Error: sendto failed.\n");
//                        goto shutdown;
//                    }
                    if(bytesSent != sizeof(response)){
                        System_printf("Error: sendto failed.\n");

                    }
                    System_flush();
                }
            }
        }
    } while (status > 0);

shutdown:
    System_flush();
    if (server > 0) {
        close(server);
    }
    // Close the file session
    fdCloseSession((void *)Task_self());
}


