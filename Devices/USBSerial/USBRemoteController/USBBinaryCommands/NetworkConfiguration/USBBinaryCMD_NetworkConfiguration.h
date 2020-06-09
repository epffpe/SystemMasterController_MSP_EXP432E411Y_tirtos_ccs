/*
 * USBBinaryCMD_NetworkConfiguration.h
 *
 *  Created on: Jun 9, 2020
 *      Author: epffpe
 */

#ifndef DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_H_
#define DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_H_



//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


/* Max IPNet Domain name Length - Change requires NETTOOLS rebuild */
#define USBBINCMD_CFG_DOMAIN_MAX  64

typedef struct {
    uint32_t    isIPAuto;               /* staticIP = 0, DHCP=1 */
    uint32_t    IPAddr;                 /* IP Address */
    uint32_t    IPMask;                 /* Subnet Mask */
    uint32_t    IPGateAddr;             /* Gateway IP Address */
    char        Domain[USBBINCMD_CFG_DOMAIN_MAX]; /* IPNet Domain Name */
}tUSBBinaryCMD_ipconfig_payload;


typedef struct {
    uint32_t    cmdType;
    tUSBBinaryCMD_ipconfig_payload payload;
    uint32_t    crc;
    uint32_t    eof;
}tUSBBinaryCMD_setIpConfig_payload;

typedef struct {
    uint32_t    header;
    uint32_t    cmdType;
    tUSBBinaryCMD_ipconfig_payload payload;
    uint32_t    crc;
    uint32_t    eof;
}tUSBBinaryCMD_getIpConfigResponseFrame;





typedef struct {
    uint32_t    user0;
    uint32_t    user1;
}tUSBBinaryCMD_macAddress_payload;


typedef struct {
    uint32_t    cmdType;
    tUSBBinaryCMD_macAddress_payload payload;
    uint32_t    crc;
    uint32_t    eof;
}tUSBBinaryCMD_setMACAddress_payload;

typedef struct {
    uint32_t    header;
    uint32_t    cmdType;
    tUSBBinaryCMD_macAddress_payload payload;
    uint32_t    crc;
    uint32_t    eof;
}tUSBBinaryCMD_getMACAddressResponseFrame;



#ifdef  __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_GLOBAL
    #define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_EXT
#else
    #define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif





#endif /* DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_H_ */
