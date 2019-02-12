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

typedef struct {
    uint32_t    index;
    uint32_t    pinConfig;
    union {
        struct {
            uint32_t    DIVal;                    /* State of discrete input channel (or # of transitions) */
            uint32_t    DIDebounceDly;
            uint32_t    DIRptStartDly;
            uint32_t    DIRptDly;
            uint8_t     DIIn;                     /* Current state of sensor input */
            uint8_t     DIBypassEn;               /* Bypass enable switch (Bypass when TRUE) */
            uint8_t     DIModeSel;                /* Discrete input channel mode selector */
            uint8_t     DIDebounceEn;
        };
        struct {
            uint32_t    DOA;
            uint32_t    DOB;
            uint32_t    DOBCtr;
            uint32_t    DOSyncCtrMax;
            uint8_t     DOOut;
            uint8_t     DOCtrl;
            uint8_t     DOBypass;
            uint8_t     DOBypassEn;
            uint8_t     DOModeSel;
            uint8_t     DOBlinkEnSel;
            uint8_t     DOInv;
        };
    };
}TCPBin_CMD_DIOConfig_payload_t;

typedef struct {
    uint32_t    index;
    uint32_t    pinConfig;
}TCPBin_CMD_DIOConfigDirection_payload_t;

typedef struct {
    uint32_t    index;
    uint32_t    pinConfig;

    struct {
        uint32_t    DIVal;                    /* State of discrete input channel (or # of transitions) */
        uint32_t    DIDebounceDly;
        uint32_t    DIRptStartDly;
        uint32_t    DIRptDly;
        uint8_t     DIIn;                     /* Current state of sensor input */
        uint8_t     DIBypassEn;               /* Bypass enable switch (Bypass when TRUE) */
        uint8_t     DIModeSel;                /* Discrete input channel mode selector */
        uint8_t     DIDebounceEn;
    };
}TCPBin_CMD_DIConfig_payload_t;

typedef struct {
    uint32_t    index;
    uint32_t    pinConfig;

    struct {
        uint32_t    DOA;
        uint32_t    DOB;
        uint32_t    DOBCtr;
        uint32_t    DOSyncCtrMax;
        uint8_t     DOOut;
        uint8_t     DOCtrl;
        uint8_t     DOBypass;
        uint8_t     DOBypassEn;
        uint8_t     DOModeSel;
        uint8_t     DOBlinkEnSel;
        uint8_t     DOInv;
    };
}TCPBin_CMD_DOConfig_payload_t;


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
const uint8_t g_TCPRCBin_DiscreteIO_DIMapTable[];
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
const uint8_t g_TCPRCBin_DiscreteIO_DOMapTable[];


__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_get5VOutputPowerStatus(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_turnOn5VOutputPower(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_turnOff5VOutputPower(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_set5VOutputPower(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_getDIOConfiguration(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_setDIODirection(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_setDIConfiguration(int clientfd, char *payload, int32_t size);
__TCPBINARYCOMMANDS_DISCRETEIO_EXT
void vTCPRCBin_DiscreteIO_setDOConfiguration(int clientfd, char *payload, int32_t size);



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_DISCRETEIO_TCPBINARYCMD_DISCRETEIO_H_ */
