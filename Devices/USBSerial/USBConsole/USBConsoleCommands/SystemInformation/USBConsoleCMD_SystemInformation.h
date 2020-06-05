/*
 * USBConsoleCMD_SystemInformation.h
 *
 *  Created on: Jun 5, 2020
 *      Author: epffpe
 */

#ifndef DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_H_
#define DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_H_


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

#ifdef  __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_GLOBAL
    #define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_EXT
#else
    #define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************

__DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_EXT
int CMD_USBcontact(int argc, char **argv);
__DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_EXT
int CMD_USBabout(int argc, char **argv);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_H_ */
