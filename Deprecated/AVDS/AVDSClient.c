/*
 * AVDSClient.c
 *
 *  Created on: Feb 6, 2018
 *      Author: epenate
 */
#define __AVDSCLIENT_GLOBAL
#include "includes.h"


#define UDPPACKETSIZE 256

void vAVDSTestTcp( tsAVDSInfoMsg *ptsAVDSInfo );
void vAVDSTestTcpCommand(tsAVDSInfoMsg *ptsAVDSInfo);
void vAVDSTestTcpCommandSetChannel( tsAVDSInfoMsg *ptsAVDSInfo );


/*
 *  ======== echoFxn ========
 *  Echoes UDP messages.
 *
 */
Void vAVDSechoFxn(UArg arg0, UArg arg1)
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
    tsAVDSInfoMsg *ptsAVDSInfo;

    // Allocate the file descriptor environment for this Task
    fdOpenSession( (void *)Task_self() );


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
                if (bytesRcvd > 0) {
//                    bytesSent = sendto(server, buffer, bytesRcvd, 0,
//                                       (struct sockaddr *)&clientAddr, addrlen);
//                    if (bytesSent < 0 || bytesSent != bytesRcvd) {
//                        System_printf("Error: sendto failed.\n");
////                        goto shutdown;
//                    }
                    ptsAVDSInfo = (tsAVDSInfoMsg *)buffer;
                    vAVDSTestTcpCommand(ptsAVDSInfo);
//                    vAVDSTestTcpCommandSetChannel(ptsAVDSInfo);
                }
            }
        }
    } while (status > 0);

    shutdown:
    if (server > 0) {
        close(server);
    }

    // Free the file descriptor environment for this Task
    fdCloseSession( (void *)Task_self() );
    Task_exit();
}


void vAVDSechoTcp( uint32_t IPAddr )
{
    int s;
    struct sockaddr_in sin1;
    int i;
    char *pBuf = 0;
    struct timeval timeout;
    Error_Block eb;


    // Allocate the file descriptor environment for this Task
    fdOpenSession( (void *)Task_self() );
    printf("\n== Start TCP Echo Client Test ==\n");
    // Create test socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( s == SOCKET_ERROR )
    {
        System_printf("failed socket create (%d)\n",fdError());
        goto leave;
    }
    // Prepare address for connect
    //    bzero( &sin1, sizeof(struct sockaddr_in) );
    memset(&sin1, 0, sizeof(struct sockaddr_in));
    sin1.sin_family = AF_INET;
    sin1.sin_addr.s_addr = IPAddr;
    sin1.sin_port = htons(7);


    // Configure our Tx and Rx timeout to be 5 seconds
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof( timeout ) );
    setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) );
    // Connect socket
    if( connect( s, (PSA) &sin1, sizeof(sin1) ) < 0 )
    {
        printf("failed connect (%d)\n",fdError());
        goto leave;
    }
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    // Allocate a working buffer
    if( !(pBuf = (char *)Memory_alloc(NULL, 4096, 0, &eb)) )
    {
        printf("failed temp buffer allocation\n");
        goto leave;
    }
    // Fill buffer with a test pattern
    for(i=0; i<4096; i++)
        *(pBuf+i) = (char)i;
    // Send the buffer
    if( send( s, pBuf, 4096, 0 ) < 0 )
    {
        printf("send failed (%d)\n",fdError());
        goto leave;
    }
    // Try and receive the test pattern back
    i = recv( s, pBuf, 4096, MSG_WAITALL );
    if( i < 0 )
    {
        printf("recv failed (%d)\n",fdError());
        goto leave;
    }
    // Verify reception size and pattern
    if( i != 4096 )
    {
        printf("received %d (not %d) bytes\n",i, 4096);
        goto leave;
    }
    for(i=0; i<4096; i++)
        if( *(pBuf+i) != (char)i )
        {
            printf("verify failed at byte %d\n",i);
            break;
        }
    // If here, the test passed
    if( i==4096 )
        printf("passed\n");
    leave:
    if( pBuf )
        //        free( pBuf );
        Memory_free(NULL, pBuf, 4096);
    if( s != SOCKET_ERROR ) fdClose( (void *)s );
    printf("== End TCP Echo Client Test ==\n\n");
    // Free the file descriptor environment for this Task
    fdCloseSession( (void *)Task_self() );
    Task_exit();
}


void vAVDSTestTcp( tsAVDSInfoMsg *ptsAVDSInfo )
{
    int s;
    struct sockaddr_in sin1;
    int i;
    char *pBuf = 0;
    struct timeval timeout;
    Error_Block eb;


    // Allocate the file descriptor environment for this Task
//    fdOpenSession( (void *)Task_self() );
    System_printf("\n== Start TCP Echo Client Test ==\n");
    // Create test socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( s == SOCKET_ERROR )
    {
        System_printf("failed socket create (%d)\n",fdError());
        goto leave;
    }
    // Prepare address for connect
    //    bzero( &sin1, sizeof(struct sockaddr_in) );
    memset(&sin1, 0, sizeof(struct sockaddr_in));
    sin1.sin_family = AF_INET;
    sin1.sin_addr.s_addr = ptsAVDSInfo->ipAddress;
    sin1.sin_port = ptsAVDSInfo->portNumber;


    // Configure our Tx and Rx timeout to be 5 seconds
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof( timeout ) );
    setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) );
    // Connect socket
    if( connect( s, (PSA) &sin1, sizeof(sin1) ) < 0 )
    {
        System_printf("failed connect (%d)\n",fdError());
        goto leave;
    }
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    // Allocate a working buffer
    if( !(pBuf = (char *)Memory_alloc(NULL, 4096, 0, &eb)) )
    {
        System_printf("failed temp buffer allocation\n");
        goto leave;
    }
    // Fill buffer with a test pattern
    for(i=0; i<4096; i++)
        *(pBuf+i) = (char)i;
    // Send the buffer
    if( send( s, pBuf, 4096, 0 ) < 0 )
    {
        System_printf("send failed (%d)\n",fdError());
        goto leave;
    }
    // Try and receive the test pattern back
    i = recv( s, pBuf, 4096, MSG_WAITALL );
    if( i < 0 )
    {
        System_printf("recv failed (%d)\n",fdError());
        goto leave;
    }
    // Verify reception size and pattern
    if( i != 4096 )
    {
        System_printf("received %d (not %d) bytes\n",i, 4096);
        goto leave;
    }
    for(i=0; i<4096; i++)
        if( *(pBuf+i) != (char)i )
        {
            System_printf("verify failed at byte %d\n",i);
            break;
        }
    // If here, the test passed
    if( i==4096 )
        System_printf("passed\n");
    leave:
    if( pBuf )
        //        free( pBuf );
        Memory_free(NULL, pBuf, 4096);
    if( s != SOCKET_ERROR ) fdClose( (void *)s );
    System_printf("== End TCP Echo Client Test ==\n\n");
    // Free the file descriptor environment for this Task
//    fdCloseSession( (void *)Task_self() );
    System_flush();
//    Task_exit();
}

void vAVDSTestTcpCommand( tsAVDSInfoMsg *ptsAVDSInfo )
{
    int s;
    struct sockaddr_in sin1;
    int i;
    char pBuf[64];
    struct timeval timeout;
    tsAVDSCommandGetChannel *ptsAVDSCommandGetChannel;
    tsAVDSCommandGetChannelResponse *ptsAVDSCommandGetChannelResponse;


    // Allocate the file descriptor environment for this Task
//    fdOpenSession( (void *)Task_self() );
    System_printf("\n== Start TCP Echo Client Test ==\n");
    // Create test socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( s == SOCKET_ERROR )
    {
        System_printf("failed socket create (%d)\n",fdError());
        goto leave;
    }
    // Prepare address for connect
    //    bzero( &sin1, sizeof(struct sockaddr_in) );
    memset(&sin1, 0, sizeof(struct sockaddr_in));
    sin1.sin_family = AF_INET;
    sin1.sin_addr.s_addr = ptsAVDSInfo->ipAddress;
    sin1.sin_port = ptsAVDSInfo->portNumber;


    // Configure our Tx and Rx timeout to be 5 seconds
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof( timeout ) );
    setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) );
    // Connect socket
    if( connect( s, (PSA) &sin1, sizeof(sin1) ) < 0 )
    {
        System_printf("failed connect (%d)\n",fdError());
        goto leave;
    }

    // Allocate a working buffer
    pBuf[0] = AVDS_CMD_Get_Channel;
    ptsAVDSCommandGetChannel = (tsAVDSCommandGetChannel *)&pBuf[1];
    ptsAVDSCommandGetChannel->outputChannel = htons(1);

    i = sizeof(tsAVDSCommandGetChannel);
    // Send the buffer
    if( send( s, pBuf, 1 + sizeof(tsAVDSCommandGetChannel), 0 ) < 0 )
    {
        System_printf("send failed (%d)\n",fdError());
        goto leave;
    }
    memset(pBuf, 0, sizeof(pBuf));
    i = sizeof(tsAVDSCommandGetChannelResponse);
    // Try and receive the test pattern back
    i = recv( s, pBuf, 64, 0 );
    if( i < 0 )
    {
        System_printf("recv failed (%d)\n",fdError());
        goto leave;
    }

    leave:
    if( s != SOCKET_ERROR ) fdClose( (void *)s );

    ptsAVDSCommandGetChannelResponse = (tsAVDSCommandGetChannelResponse *)&pBuf[1];
    ptsAVDSCommandGetChannelResponse->result = ntohl(ptsAVDSCommandGetChannelResponse->result);
    ptsAVDSCommandGetChannelResponse->audioInputChannel = ntohs(ptsAVDSCommandGetChannelResponse->audioInputChannel);
    ptsAVDSCommandGetChannelResponse->videoInputChannel = ntohs(ptsAVDSCommandGetChannelResponse->videoInputChannel);
    System_printf("Result == %d\n", ptsAVDSCommandGetChannelResponse->result);
    System_printf("Video == %d\n", ptsAVDSCommandGetChannelResponse->videoInputChannel);
    System_printf("Audio Channel == %d\n", ptsAVDSCommandGetChannelResponse->audioInputChannel);

    System_printf("== End TCP Echo Client Test ==\n\n");
    // Free the file descriptor environment for this Task
//    fdCloseSession( (void *)Task_self() );
    System_flush();
//    Task_exit();
}



void vAVDSTestTcpCommandSetChannel( tsAVDSInfoMsg *ptsAVDSInfo )
{
    int s;
    struct sockaddr_in sin1;
    int i;
    char pBuf[64];
    struct timeval timeout;
    tsAVDSCommandSetChannel *ptsAVDSCommandSetChannel;
    tsAVDSCommandSetChannelResponse *ptsAVDSCommandSetChannelResponse;


    // Allocate the file descriptor environment for this Task
//    fdOpenSession( (void *)Task_self() );
    System_printf("\n== Start TCP Echo Client Test ==\n");
    // Create test socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( s == SOCKET_ERROR )
    {
        System_printf("failed socket create (%d)\n",fdError());
        goto leave;
    }
    // Prepare address for connect
    //    bzero( &sin1, sizeof(struct sockaddr_in) );
    memset(&sin1, 0, sizeof(struct sockaddr_in));
    sin1.sin_family = AF_INET;
    sin1.sin_addr.s_addr = ptsAVDSInfo->ipAddress;
    sin1.sin_port = ptsAVDSInfo->portNumber;


    // Configure our Tx and Rx timeout to be 5 seconds
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof( timeout ) );
    setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) );
    // Connect socket
    if( connect( s, (PSA) &sin1, sizeof(sin1) ) < 0 )
    {
        System_printf("failed connect (%d)\n",fdError());
        goto leave;
    }

    // Allocate a working buffer
    pBuf[0] = AVDS_CMD_Set_Channel;
    ptsAVDSCommandSetChannel = (tsAVDSCommandSetChannel *)&pBuf[1];
    ptsAVDSCommandSetChannel->outputChannel = htons(3);
    ptsAVDSCommandSetChannel->audioInputChannel = htons(2);
    ptsAVDSCommandSetChannel->videoInputChannel = htons(3);

    i = sizeof(tsAVDSCommandSetChannel);
    // Send the buffer
    if( send( s, pBuf, 1 + sizeof(tsAVDSCommandSetChannel), 0 ) < 0 )
    {
        System_printf("send failed (%d)\n",fdError());
        goto leave;
    }
    memset(pBuf, 0, sizeof(pBuf));
    i = sizeof(tsAVDSCommandSetChannelResponse);
    // Try and receive the test pattern back
    i = recv( s, pBuf, 64, 0 );
    if( i < 0 )
    {
        System_printf("recv failed (%d)\n",fdError());
        goto leave;
    }

    leave:
    if( s != SOCKET_ERROR ) fdClose( (void *)s );

    ptsAVDSCommandSetChannelResponse = (tsAVDSCommandSetChannelResponse *)&pBuf[1];
    ptsAVDSCommandSetChannelResponse->result = ntohl(ptsAVDSCommandSetChannelResponse->result);
    System_printf("Result == %d\n", ptsAVDSCommandSetChannelResponse->result);

    System_printf("== End TCP Echo Client Test ==\n\n");
    // Free the file descriptor environment for this Task
//    fdCloseSession( (void *)Task_self() );
    System_flush();
//    Task_exit();
}



Task_Handle xAVDS_init(void *params)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.stackSize = AVDS_TASK_STACK_SIZE;
    taskParams.priority = AVDS_TASK_PRIORITY;
    taskParams.arg0 = (UArg)params;
    taskHandle = Task_create((Task_FuncPtr)vAVDSechoFxn, &taskParams, &eb);

    return taskHandle;
}
