/*
 * USBConsoleCMD_SystemConfig.h
 *
 *  Created on: Oct 23, 2020
 *      Author: epenate
 */

#ifndef DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_H_
#define DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_H_



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

#ifdef  __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_GLOBAL
    #define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_EXT
#else
    #define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************
__DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_EXT
int CMD_USBset(int argc, char **argv);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif



#endif /* DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_H_ */
