/*
 * TCPRemoteController.h
 *
 *  Created on: May 2, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPREMOTECONTROLLER_H_
#define DEVICES_REMOTECONTROLLER_TCPREMOTECONTROLLER_H_


#define TCPRCDEVICE_TASK_STACK_SIZE         2048
#define TCPRCDEVICE_TASK_PRIORITY           5
#define TCPRCDEVICE_ID                      1


#define TCPRCDEVICE_CLOCK_TIMEOUT       10
#define TCPRCDEVICE_CLOCK_PERIOD        10

#define TCPDEVICE_PACKETSIZE            256
#define TCPDEVICE_NUMTCPWORKERS         3

#define TCP_SERVER_HANDLER_STACK        1024
#define TCP_SERVER_TASK_PRIORITY        2
#define TCP_SERVER_PORT                 1000


#define TCP_WORKER_HANDLER_STACK        2048
#define TCP_WORKER_TASK_PRIORITY        2


#define UDP_SERVER_HANDLER_STACK        1024
#define UDP_SERVER_TASK_PRIORITY        2
#define UDP_SERVER_PORT                 1001


typedef enum
{
    SERVICE_TCP_REMOTECONTROL_ASCII_VOLUMEN_RETURN_UUID = 0x0001,
} TCPRCASCII_service_UUID_t;


#ifdef __cplusplus
extern "C"  {
#endif


#ifdef  __DEVICES_TCPRCDEVICE_ALTOMULTINETDEVICE_GLOBAL
    #define __DEVICES_TCPRCDEVICE_ALTOMULTINETDEVICE_EXT
#else
    #define __DEVICES_TCPRCDEVICE_ALTOMULTINETDEVICE_EXT  extern
#endif


__DEVICES_TCPRCDEVICE_ALTOMULTINETDEVICE_EXT void vTCPRCDevice_Params_init(Device_Params *params, uint32_t address);
//extern int fdOpenSession(void *);
//extern void fdCloseSession();
//extern void *TaskSelf();


#ifdef __cplusplus
}
#endif


#endif /* DEVICES_REMOTECONTROLLER_TCPREMOTECONTROLLER_H_ */
