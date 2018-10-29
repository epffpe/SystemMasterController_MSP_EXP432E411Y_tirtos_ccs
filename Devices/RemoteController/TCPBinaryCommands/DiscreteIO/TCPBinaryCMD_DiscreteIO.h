/*
 * TCPBinaryCMD_DiscreteIO.h
 *
 *  Created on: Oct 12, 2018
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_DISCRETEIO_TCPBINARYCMD_DISCRETEIO_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_DISCRETEIO_TCPBINARYCMD_DISCRETEIO_H_

typedef struct {
    uint32_t v5Status;
}TCPBin_CMD_5VPowerStatus_payload_t;

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  __TCPBINARYCOMMANDS_DISCRETEIO_GLOBAL
    #define __TCPBINARYCOMMANDS_DISCRETEIO_EXT
#else
    #define __TCPBINARYCOMMANDS_DISCRETEIO_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_get5VOutputPowerStatus(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_turnOn5VOutputPower(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_turnOff5VOutputPower(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_set5VOutputPower(int clientfd, char *payload, int32_t size);



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_DISCRETEIO_TCPBINARYCMD_DISCRETEIO_H_ */
