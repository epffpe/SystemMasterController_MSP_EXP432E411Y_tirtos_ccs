/*
 * commands.h
 *
 *  Created on: Oct 18, 2016
 *      Author: epenate
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32;1m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A



//*****************************************************************************
//
// Defines for the command line argument parser provided as a standard part of
// TivaWare.  hibernate example application uses the command line parser to
// extend functionality to the serial port.
//
//*****************************************************************************

#define CMDLINE_MAX_ARGS 5




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

#ifdef	__TCPCONSOLECOMMANDS_GLOBAL
	#define	__TCPCONSOLECOMMANDS_EXT
#else
	#define __TCPCONSOLECOMMANDS_EXT	extern
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




#endif /* COMMANDS_H_ */
