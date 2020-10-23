/*
 * USBConsoleCommands.h
 *
 *  Created on: Jun 5, 2020
 *      Author: epffpe
 */

#ifndef DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_H_
#define DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_H_



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

#ifdef  __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_GLOBAL
    #define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_EXT
#else
    #define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_EXT    extern
#endif

//*****************************************************************************
//
// Declaration for the callback functions that will implement the command line
// functionality.  These functions get called by the command line interpreter
// when the corresponding command is typed into the command line.
//
//*****************************************************************************

__DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_EXT
Void vUSBConsoleWorkerFxn(UArg arg0, UArg arg1);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_H_ */
