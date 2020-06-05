/*
 * commands.c
 *
 *  Created on: Oct 18, 2016
 *      Author: epenate
 */
#define __TCPCONSOLECOMMANDS_GLOBAL
#include "includes.h"
#undef htonl
#undef htons
#undef ntohl
#undef ntohs


#include <stdio.h>
#include <stdlib.h>
/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <ti/net/slnetutils.h>

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

//
//const char g_cCMDContatInformation[] = {
//                                        "\r\n"
//                                        ANSI_COLOR_YELLOW " Eugenio Penate\r\n" ANSI_COLOR_RESET
//                                        " Electrical Engineer\r\n"
//                                        " 978.466.5992\r\n"
//                                        " epenate@altoaviation.com\r\n"
//                                        "\n"
//                                        " Located at ALTO Jupiter, FL\r\n"
//                                        " ALTO Aviation / 880 Jupiter Park Drive suite 3 / Jupiter, FL 33458\r\n"
//                                        "\n"
//                                        " Corporate office\r\n"
//                                        " ALTO Aviation / 86 Leominster Road / PO Box 399 / Sterling, MA 01564\r\n"
//                                        " 800.814.0123 Toll Free / 978.466.5992 P / 978.466.5996 F\r\n"
//                                        "\r\n"
//
//
//};
//
//
//const char g_cCMDAboutInformation[] = {
//                                        "\r\n"
//                                        ANSI_COLOR_YELLOW " System\r\n" ANSI_COLOR_RESET
//                                        " Model number: %s\r\n"
//                                        " Serial number: %s\r\n"
//                                        " # of channels: %d\r\n"
//                                        "\r\n"
//                                        ANSI_COLOR_YELLOW " Firmware\r\n" ANSI_COLOR_RESET
//                                        " System version: %d.%d.%d.%d\r\n"
//                                        "\r\n"
//};
//


const char g_cCMDreprogramInformation[] = {
                                           "\r\n"
                                           ANSI_COLOR_MAGENTA
                                           "**************************************\r\n"
                                           "*****Entering in programming mode*****\r\n"
                                           "**************************************\r\n"
                                           ANSI_COLOR_RESET
                                           ANSI_COLOR_CYAN " Open LM Flash Programmer to continue\r\n" ANSI_COLOR_RESET
                                           "\r\n"
};




const char g_cCMDstatusInformation[] = {
                                        "\r"
                                        ANSI_COLOR_CYAN
                                        "sw0:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_CYAN
                                        "is0:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_CYAN
                                        "ind0:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_CYAN
                                        "R0:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_CYAN
                                        "S0:"
                                        ANSI_COLOR_RESET
                                        " %d "


                                        ANSI_COLOR_YELLOW
                                        "sw1:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_YELLOW
                                        "is1:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_YELLOW
                                        "ind1:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_YELLOW
                                        "R1:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_YELLOW
                                        "S1:"
                                        ANSI_COLOR_RESET
                                        " %d "


                                        ANSI_COLOR_MAGENTA
                                        "sw2:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_MAGENTA
                                        "is2:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_MAGENTA
                                        "ind2:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_MAGENTA
                                        "R2:"
                                        ANSI_COLOR_RESET
                                        " %d "
                                        ANSI_COLOR_MAGENTA
                                        "S2:"
                                        ANSI_COLOR_RESET
                                        " %d "
};

const char g_cCMDstatusChannelInformation[] = {
                                               ANSI_COLOR_YELLOW
                                               " Channel %d:"
                                               ANSI_COLOR_RESET
                                               " %s, %s\r\n"
};

const char g_cCMDstatusChannelMasterInformation[] = {
                                                     ANSI_COLOR_GREEN
                                                     "Master"
                                                     ANSI_COLOR_RESET
};

const char g_cCMDstatusChannelSlaveInformation[] = {
                                                    ANSI_COLOR_CYAN
                                                    "Slave"
                                                    ANSI_COLOR_RESET
};

const char g_cCMDstatusChannelToggleInformation[] = {
                                                     ANSI_COLOR_MAGENTA
                                                     "Toggle"
                                                     ANSI_COLOR_RESET
};

const char g_cCMDstatusChannelMomentaryInformation[] = {
                                                        ANSI_COLOR_RED
                                                        "Momentary"
                                                        ANSI_COLOR_RESET
};



const char g_cCMDreadVoltagesInformation[] = {
                                              "\r"
                                              ANSI_COLOR_CYAN
                                              " Vin:%s"
                                              " %2.1f V"
                                              ANSI_COLOR_RESET
                                              ANSI_COLOR_CYAN
                                              " VUSB:%s"
                                              " %2.1f V"
                                              ANSI_COLOR_RESET
};



const char g_cCMDreadTemperatureInformation[] = {
                                                 "\r"
                                                 ANSI_COLOR_CYAN
                                                 " uC temp:%s"
                                                 " %d C,"
                                                 " %d F"
                                                 ANSI_COLOR_RESET
};



const char g_cCMDrebootInformation[] = {
                                           "\r\n"
                                           ANSI_COLOR_MAGENTA
                                           " Rebooting the system\r\n"
                                           ANSI_COLOR_RESET
};


const char g_cCMDrebootUSBInformation[] = {
                                           "\r\n"
                                           ANSI_COLOR_MAGENTA
                                           " Rebooting the system\r\n\r\n"
                                           ANSI_COLOR_RESET
                                           ANSI_COLOR_CYAN
                                           " Close this terminal and open a new one!"
                                           ANSI_COLOR_RESET
};



//*****************************************************************************
//
// Command: help
//
// Print the help strings for all commands.
//
//*****************************************************************************
//int CMD_help(UART_Handle uart, int argc, char **argv)
//{
//    int32_t i32Index;
//    int n;
//    char buff[128];
//    tCmdLineEntry *psCmdEntry;
//    //
//    // Keep the compiler happy.
//    //
//    (void)argc;
//    (void)argv;
//    (void)uart;
//
//    //
//    // Check the argument count and return errors for too many or too few.
//    //
//    if(argc == 1) {
//    	//
//    	// Start at the beginning of the command table
//    	//
//    	i32Index = 0;
//
//    	//
//    	// Get to the start of a clean line on the serial output.
//    	//
//    	n = sprintf(buff, "\n\rAvailable Commands\n\r------------------\n\n");
//    	UART_write(uart, buff, n);
//    	//
//    	// Display strings until we run out of them.
//    	//
//    	while(g_psCmdTable[i32Index].pcCmd)
//    	{
//    		n = sprintf(buff, "\r\x1b[32;1m%17s\x1b[0m %s\n", g_psCmdTable[i32Index].pcCmd,
//    				g_psCmdTable[i32Index].pcHelp);
//    		UART_write(uart, buff, n);
//    		i32Index++;
//    	}
//    }
//    if(argc == 2) {
//    	// Start at the beginning of the command table, to look for a matching
//    	// command.
//    	//
//    	psCmdEntry = &g_psCmdTable[0];
//
//    	//
//    	// Search through the command table until a null command string is
//    	// found, which marks the end of the table.
//    	//
//    	while (psCmdEntry->pcCmd) {
//    		//
//    		// If this command entry command string matches argv[0], then call
//    		// the function for this command, passing the command line
//    		// arguments.
//    		// ustrncmp(argv[1], psCmdEntry->pcCmd, 5)
//    		if (!ustrncmp(argv[1], psCmdEntry->pcCmd, 10)) {
//    			n = sprintf(buff, "\n\rCommand: \x1b[32;1m%s\x1b[0m\n\r------------------\n\r  %s\n", argv[1], psCmdEntry->pcHelp + 3);
//    			UART_write(uart, buff, n);
//    		}
//
//    		//
//    		// Not found, so advance to the next entry.
//    		//
//    		psCmdEntry++;
//    	}
//    }
//
//    if(argc > 2)
//    {
//    	return(CMDLINE_TOO_MANY_ARGS);
//    }
//    //
//    // Leave a blank line after the help strings.
//    //
//    n = sprintf(buff, "\n\r");
//    UART_write(uart, buff, n);
//
//    return (0);
//}
//
//int CMD_USBhelp(int argc, char **argv)
//{
//    int32_t i32Index;
//    int n;
//    char buff[128];
//    tUSBCmdLineEntry *psCmdEntry;
//    //
//    // Keep the compiler happy.
//    //
//    (void)argc;
//    (void)argv;
//
//    //
//    // Check the argument count and return errors for too many or too few.
//    //
//    if(argc == 1){
//        //
//        // Start at the beginning of the command table
//        //
//        i32Index = 0;
//
//        //
//        // Get to the start of a clean line on the serial output.
//        //
//        n = sprintf(buff, "\n\rAvailable Commands\n\r------------------\n\n");
//        USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//        //
//        // Display strings until we run out of them.
//        //
//        while(g_psUSBCmdTable[i32Index].pcCmd)
//        {
//            n = sprintf(buff, "\r\x1b[32;1m%17s\x1b[0m %s\n", g_psUSBCmdTable[i32Index].pcCmd,
//                        g_psUSBCmdTable[i32Index].pcHelp);
//            USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//            i32Index++;
//        }
//    }
//    if(argc == 2){
//        // Start at the beginning of the command table, to look for a matching
//        // command.
//        //
//        psCmdEntry = &g_psUSBCmdTable[0];
//
//        //
//        // Search through the command table until a null command string is
//        // found, which marks the end of the table.
//        //
//        while (psCmdEntry->pcCmd) {
//            //
//            // If this command entry command string matches argv[0], then call
//            // the function for this command, passing the command line
//            // arguments.
//            // ustrncmp(argv[1], psCmdEntry->pcCmd, 5)
//            if (!ustrncmp(argv[1], psCmdEntry->pcCmd, 10)) {
//                n = sprintf(buff, "\n\rCommand: \x1b[32;1m%s\x1b[0m\n\r------------------\n\r  %s\n", argv[1], psCmdEntry->pcHelp + 3);
//                USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//            }
//
//            //
//            // Not found, so advance to the next entry.
//            //
//            psCmdEntry++;
//        }
//    }
//
//    if(argc > 2) {
//        return(CMDLINE_TOO_MANY_ARGS);
//    }
//    //
//    // Leave a blank line after the help strings.
//    //
//    n = sprintf(buff, "\n\r");
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//
//    return (0);
//}
//
////*****************************************************************************
////
//// Command: cls
////
//// Clear the terminal screen.
////
////*****************************************************************************
//int CMD_cls(UART_Handle uart, int argc, char **argv)
//{
//    int n;
//    char buff[32];
//    (void)uart;
//
//    n = sprintf(buff, "\033[2J\033[H");
//    UART_write(uart, buff, n);
////    g_bFirstUpdate = true;
////    g_ui8FirstLine = 1;
//    return (0);
//}
//
//int CMD_USBcls(int argc, char **argv)
//{
//    int n;
//    char buff[32];
//
//    n = sprintf(buff, "\033[2J\033[H");
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
////    g_bFirstUpdate = true;
////    g_ui8FirstLine = 1;
//    return (0);
//}
//
//
////*****************************************************************************
////
//// Command: contact
////
//// Display contact information.
////
////*****************************************************************************
//int CMD_contact(UART_Handle uart, int argc, char **argv)
//{
//    (void)uart;
//
//    UART_write(uart, g_cCMDContatInformation, sizeof(g_cCMDContatInformation));
//    return (0);
//}
//
//int CMD_USBcontact(int argc, char **argv)
//{
//    USBCDCD_sendData((const unsigned char *)g_cCMDContatInformation, sizeof(g_cCMDContatInformation), BIOS_WAIT_FOREVER);
//    return (0);
//}
//
//
//
//
////*****************************************************************************
////
//// Command: about
////
//// Display information about this unit.
////
////*****************************************************************************
//int CMD_about(UART_Handle uart, int argc, char **argv)
//{
//    int n;
//    volatile tEEPROM_Data *infoResponse;
//    char buff[256];
//
//    infoResponse = INFO_get();
//    n = sprintf(buff, g_cCMDAboutInformation,
//                infoResponse->modelNumber,
//                infoResponse->serialNumber,
//                infoResponse->numberOfChannels,
//                FIRMWARE_VERSION_MAJOR,
//                FIRMWARE_VERSION_MINOR,
//                FIRMWARE_VERSION_MONTH,
//                FIRMWARE_VERSION_BUILD);
//    UART_write(uart, buff, n);
//    return (0);
//}
//
//int CMD_USBabout(int argc, char **argv)
//{
//    int n;
//    volatile tEEPROM_Data *infoResponse;
//    char buff[256];
//
//    infoResponse = INFO_get();
//    n = sprintf(buff, g_cCMDAboutInformation,
//                infoResponse->modelNumber,
//                infoResponse->serialNumber,
//                infoResponse->numberOfChannels,
//                FIRMWARE_VERSION_MAJOR,
//                FIRMWARE_VERSION_MINOR,
//                FIRMWARE_VERSION_MONTH,
//                FIRMWARE_VERSION_BUILD);
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//    return (0);
//}
//
//
//
//
//
////*****************************************************************************
////
//// Command: reprogram
////
//// Display information about this unit.
////
////*****************************************************************************
//int CMD_reprogram(UART_Handle uart, int argc, char **argv)
//{
//    (void)uart;
//
//    UART_write(uart, g_cCMDreprogramInformation, sizeof(g_cCMDreprogramInformation));
//    Task_sleep(200);
//    DFU_jumpToBootLoader();
//    return (0);
//}
//
//int CMD_USBreprogram(int argc, char **argv)
//{
//    USBCDCD_sendData((const unsigned char *)g_cCMDreprogramInformation, sizeof(g_cCMDreprogramInformation), BIOS_WAIT_FOREVER);
//    Task_sleep(200);
//    DFU_jumpToBootLoader();
//    return (0);
//}
//
//
//////*****************************************************************************
//////
////// Command: start
//////
////// Clear the terminal screen.
//////
//////*****************************************************************************
////int
////CMD_start(UART_Handle uart, int argc, char **argv)
////{
////	int n;
////	char buff[32];
////	(void)uart;
////
////	n = sprintf(buff, "\r Button tester sequence started\n\r");
////	UART_write(uart, buff, n);
////	g_bStart = true;
////	return (0);
////}
////
//////*****************************************************************************
//////
////// Command: stop
//////
////// Clear the terminal screen.
//////
//////*****************************************************************************
////int
////CMD_stop(UART_Handle uart, int argc, char **argv)
////{
////	int n;
////	char buff[32];
////	(void)uart;
////
////	n = sprintf(buff, "\r Button tester sequence stopped\n\r");
////	UART_write(uart, buff, n);
////	g_bStart = false;
////	return (0);
////}
////
////*****************************************************************************
////
//// Command: status
////
//// Display status information.
////
////*****************************************************************************
//int CMD_status(UART_Handle uart, int argc, char **argv)
//{
//    int n;
//    char buff[256];
//    bool exit = false, dataAvailable;
//    (void)uart;
//
//    n = sprintf(buff, "\r Press \"\x1b[32;1mq\x1b[0m\" to exit\n\r");
//    UART_write(uart, buff, n);
//
//    n = sprintf(buff, g_cCMDstatusChannelInformation,
//                0,
//                (DIGet(DIP_MS0) == Board_DIP_MASTER) ? g_cCMDstatusChannelMasterInformation : g_cCMDstatusChannelSlaveInformation,
//                (DIGet(DIP_MT0) == Board_DIP_TOGGLE) ? g_cCMDstatusChannelToggleInformation : g_cCMDstatusChannelMomentaryInformation);
//    UART_write(uart, buff, n);
//    n = sprintf(buff, g_cCMDstatusChannelInformation,
//                1,
//                (DIGet(DIP_MS1) == Board_DIP_MASTER) ? g_cCMDstatusChannelMasterInformation : g_cCMDstatusChannelSlaveInformation,
//                (DIGet(DIP_MT1) == Board_DIP_TOGGLE) ? g_cCMDstatusChannelToggleInformation : g_cCMDstatusChannelMomentaryInformation);
//    UART_write(uart, buff, n);
//    n = sprintf(buff, g_cCMDstatusChannelInformation,
//                2,
//                (DIGet(DIP_MS2) == Board_DIP_MASTER) ? g_cCMDstatusChannelMasterInformation : g_cCMDstatusChannelSlaveInformation,
//                (DIGet(DIP_MT2) == Board_DIP_TOGGLE) ? g_cCMDstatusChannelToggleInformation : g_cCMDstatusChannelMomentaryInformation);
//    UART_write(uart, buff, n);
//
//    while(!exit){
//        UART_control(uart, UART_CMD_ISAVAILABLE, &dataAvailable);
//        if (dataAvailable){
//            UART_read(uart, &buff, 1);
//            if (buff[0] == 'q') {
//                n = sprintf(buff, "\n\r");
//                UART_write(uart, buff, n);
//                exit = true;
//                return (0);
//            }
//        }
//        n = sprintf(buff, g_cCMDstatusInformation,
//                    DITbl[SW0].DIIn ? 0 : 1,
//                    DIGet(SW0),
//                    DIGet(IND0),
//                    DOTbl[RELAY0].DOCtrl,
//                    DOTbl[SLAVE0].DOCtrl,
//                    DITbl[SW1].DIIn ? 0 : 1,
//                    DIGet(SW1),
//                    DIGet(IND1),
//                    DOTbl[RELAY1].DOCtrl,
//                    DOTbl[SLAVE1].DOCtrl,
//                    DITbl[SW2].DIIn ? 0 : 1,
//                    DIGet(SW2),
//                    DIGet(IND2),
//                    DOTbl[RELAY2].DOCtrl,
//                    DOTbl[SLAVE2].DOCtrl
//                    );
//        UART_write(uart, buff, n);
//        Task_sleep(100);
//    }
//
//    return (0);
//}
//
//int CMD_USBstatus(int argc, char **argv)
//{
//    int n, received;
//    char buff[256];
//    bool exit = false;
//
//    n = sprintf(buff, "\r Press \"\x1b[32;1mq\x1b[0m\" to exit\n\r");
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//
//    n = sprintf(buff, g_cCMDstatusChannelInformation,
//                0,
//                (DIGet(DIP_MS0) == Board_DIP_MASTER) ? g_cCMDstatusChannelMasterInformation : g_cCMDstatusChannelSlaveInformation,
//                (DIGet(DIP_MT0) == Board_DIP_TOGGLE) ? g_cCMDstatusChannelToggleInformation : g_cCMDstatusChannelMomentaryInformation);
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//    n = sprintf(buff, g_cCMDstatusChannelInformation,
//                1,
//                (DIGet(DIP_MS1) == Board_DIP_MASTER) ? g_cCMDstatusChannelMasterInformation : g_cCMDstatusChannelSlaveInformation,
//                (DIGet(DIP_MT1) == Board_DIP_TOGGLE) ? g_cCMDstatusChannelToggleInformation : g_cCMDstatusChannelMomentaryInformation);
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//    n = sprintf(buff, g_cCMDstatusChannelInformation,
//                2,
//                (DIGet(DIP_MS2) == Board_DIP_MASTER) ? g_cCMDstatusChannelMasterInformation : g_cCMDstatusChannelSlaveInformation,
//                (DIGet(DIP_MT2) == Board_DIP_TOGGLE) ? g_cCMDstatusChannelToggleInformation : g_cCMDstatusChannelMomentaryInformation);
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//
//    while(!exit){
////        UART_control(uart, UART_CMD_ISAVAILABLE, &dataAvailable);
//        received = USBCDCD_receiveData((unsigned char *)buff, 1, 1);
//        if (received){
////            UART_read(uart, &buff, 1);
//            if (buff[0] == 'q') {
//                n = sprintf(buff, "\n\r");
//                USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//                exit = true;
//                return (0);
//            }
//        }
//        n = sprintf(buff, g_cCMDstatusInformation,
//                    DITbl[SW0].DIIn ? 0 : 1,
//                    DIGet(SW0),
//                    DIGet(IND0),
//                    DOTbl[RELAY0].DOCtrl,
//                    DOTbl[SLAVE0].DOCtrl,
//                    DITbl[SW1].DIIn ? 0 : 1,
//                    DIGet(SW1),
//                    DIGet(IND1),
//                    DOTbl[RELAY1].DOCtrl,
//                    DOTbl[SLAVE1].DOCtrl,
//                    DITbl[SW2].DIIn ? 0 : 1,
//                    DIGet(SW2),
//                    DIGet(IND2),
//                    DOTbl[RELAY2].DOCtrl,
//                    DOTbl[SLAVE2].DOCtrl
//                    );
//        USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//        Task_sleep(100);
//    }
//
//    return (0);
//}
//
//
//
////*****************************************************************************
////
//// Command: voltages
////
//// Display input voltages.
////
////*****************************************************************************
//int CMD_voltages(UART_Handle uart, int argc, char **argv)
//{
//    int n;
//    char buff[128];
//    bool exit = false, dataAvailable;
//    const char *pcColor;
//    (void)uart;
//
//    n = sprintf(buff, "\r Press \"\x1b[32;1mq\x1b[0m\" to exit\n\r");
//    UART_write(uart, buff, n);
//
//    while(!exit){
//        UART_control(uart, UART_CMD_ISAVAILABLE, &dataAvailable);
//        if (dataAvailable){
//            UART_read(uart, &buff, 1);
//            if (buff[0] == 'q') {
//                n = sprintf(buff, "\n\r");
//                UART_write(uart, buff, n);
//                exit = true;
//                return (0);
//            }
//        }
//
//        if (g_fHIBV28 > 26.0) {
//            pcColor = ANSI_COLOR_GREEN;
//        }else{
//            pcColor = (g_fHIBV28 > 21.0) ? ANSI_COLOR_YELLOW : ANSI_COLOR_RED;
//        }
//        n = sprintf(buff, g_cCMDreadVoltagesInformation,
//                    pcColor,
//                    g_fHIBV28,
//                    (g_fHIBVUSb > 4.3) ? ANSI_COLOR_GREEN : ANSI_COLOR_YELLOW,
//                    g_fHIBVUSb);
//        UART_write(uart, buff, n);
//        Task_sleep(100);
//    }
//
//    return (0);
//}
//
//int CMD_USBvoltages(int argc, char **argv)
//{
//    int n, received;
//    char buff[128];
//    bool exit = false;
//    const char *pcColor;
//
//    n = sprintf(buff, "\r Press \"\x1b[32;1mq\x1b[0m\" to exit\n\r");
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//
//    while(!exit){
//        received = USBCDCD_receiveData((unsigned char *)buff, 1, 1);
//        if (received){
//            if (buff[0] == 'q') {
//                n = sprintf(buff, "\n\r");
//                USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//                exit = true;
//                return (0);
//            }
//        }
//
//        if (g_fHIBV28 > 26.0) {
//            pcColor = ANSI_COLOR_GREEN;
//        }else{
//            pcColor = (g_fHIBV28 > 21.0) ? ANSI_COLOR_YELLOW : ANSI_COLOR_RED;
//        }
//        n = sprintf(buff, g_cCMDreadVoltagesInformation,
//                    pcColor,
//                    g_fHIBV28,
//                    (g_fHIBVUSb > 4.3) ? ANSI_COLOR_GREEN : ANSI_COLOR_YELLOW,
//                    g_fHIBVUSb);
//        USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//        Task_sleep(100);
//    }
//
//    return (0);
//}
//
//
////*****************************************************************************
////
//// Command: temp
////
//// Display uController temperature.
////
////*****************************************************************************
//int CMD_temp(UART_Handle uart, int argc, char **argv)
//{
//    int n;
//    char buff[128];
//    bool exit = false, dataAvailable;
//    const char *pcColor;
//    (void)uart;
//
//    n = sprintf(buff, "\r Press \"\x1b[32;1mq\x1b[0m\" to exit\n\r");
//    UART_write(uart, buff, n);
//
//    while(!exit){
//        UART_control(uart, UART_CMD_ISAVAILABLE, &dataAvailable);
//        if (dataAvailable){
//            UART_read(uart, &buff, 1);
//            if (buff[0] == 'q') {
//                n = sprintf(buff, "\n\r");
//                UART_write(uart, buff, n);
//                exit = true;
//                return (0);
//            }
//        }
//
//        if (g_ui32HIBTempValueC < 35) {
//            pcColor = ANSI_COLOR_GREEN;
//        }else{
//            pcColor = (g_ui32HIBTempValueC < 60) ? ANSI_COLOR_YELLOW : ANSI_COLOR_RED;
//        }
//        n = sprintf(buff, g_cCMDreadTemperatureInformation,
//                    pcColor,
//                    g_ui32HIBTempValueC,
//                    g_ui32HIBTempValueF);
//        UART_write(uart, buff, n);
//        Task_sleep(100);
//    }
//
//    return (0);
//}
//
//int CMD_USBtemp(int argc, char **argv)
//{
//    int n, received;
//    char buff[128];
//    bool exit = false;
//    const char *pcColor;
//
//    n = sprintf(buff, "\r Press \"\x1b[32;1mq\x1b[0m\" to exit\n\r");
//    USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//
//    while(!exit){
//        received = USBCDCD_receiveData((unsigned char *)buff, 1, 1);
//        if (received){
//            if (buff[0] == 'q') {
//                n = sprintf(buff, "\n\r");
//                USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//                exit = true;
//                return (0);
//            }
//        }
//
//        if (g_ui32HIBTempValueC < 35) {
//            pcColor = ANSI_COLOR_GREEN;
//        }else{
//            pcColor = (g_ui32HIBTempValueC < 60) ? ANSI_COLOR_YELLOW : ANSI_COLOR_RED;
//        }
//        n = sprintf(buff, g_cCMDreadTemperatureInformation,
//                    pcColor,
//                    g_ui32HIBTempValueC,
//                    g_ui32HIBTempValueF);
//        USBCDCD_sendData((const unsigned char *)buff, n, BIOS_WAIT_FOREVER);
//        Task_sleep(100);
//    }
//
//    return (0);
//}
//
//
//
////*****************************************************************************
////
//// Command: reboot
////
//// Reset the system.
////
////*****************************************************************************
//int CMD_reboot(UART_Handle uart, int argc, char **argv)
//{
//    (void)uart;
//
//    UART_write(uart, g_cCMDrebootInformation, sizeof(g_cCMDrebootInformation));
//    Task_sleep(50);
//    ROM_SysCtlReset();
//    return (0);
//}
//
//int CMD_USBreboot(int argc, char **argv)
//{
//    USBCDCD_sendData((const unsigned char *)g_cCMDrebootUSBInformation, sizeof(g_cCMDrebootUSBInformation), BIOS_WAIT_FOREVER);
//    Task_sleep(100);
//    ROM_SysCtlReset();
//    return (0);
//}


//
////*****************************************************************************
////
//// Command: stop
////
//// Clear the terminal screen.
////
////*****************************************************************************
//int
//CMD_eeprom(UART_Handle uart, int argc, char **argv)
//{
//	int n;
//	char buff[128];
//	uint32_t pui32Data[4];
//
//    //
//    // Keep the compiler happy.
//    //
//    (void)argc;
//    (void)argv;
//    (void)uart;
//
//    //
//    // Check the argument count and return errors for too many or too few.
//    //
//    if(argc == 1)
//    {
//    	return(CMDLINE_TOO_FEW_ARGS);
//    }
//    if(argc > 2)
//    {
//    	return(CMDLINE_TOO_MANY_ARGS);
//    }
//
//    if(ustrncmp(argv[1], "read", 4) == 0)
//    {
//    	EEPROMRead(pui32Data, 0x000, sizeof(pui32Data));
//    	n = sprintf(buff, "\r Counter: \x1b[36m%d\x1b[0m, buttons: \x1b[32m%d\x1b[0m, errorLow: \x1b[31;1m%d\x1b[0m, errorHigh: \x1b[31;1m%d\x1b[0m ", pui32Data[0], pui32Data[1], pui32Data[2], pui32Data[3]);
//    	UART_write(uart, buff, n);
//    }else if(ustrncmp(argv[1], "reset", 5) == 0){
//    	pui32Data[0] = 0;
//    	pui32Data[1] = 0;
//    	pui32Data[2] = 0;
//    	pui32Data[3] = 0;
//    	g_ui32counter = 0;
//		g_ui32button = 0;
//		g_ui32errorLow = 0;
//		g_ui32errorHigh = 0;
//    	EEPROMProgram(pui32Data, 0x000, sizeof(pui32Data));
//    	EEPROMRead(pui32Data, 0x000, sizeof(pui32Data));
//    	n = sprintf(buff, "\r Counter: \x1b[36m%d\x1b[0m, buttons: \x1b[32m%d\x1b[0m, errorLow: \x1b[31;1m%d\x1b[0m, errorHigh: \x1b[31;1m%d\x1b[0m ", pui32Data[0], pui32Data[1], pui32Data[2], pui32Data[3]);
//    	UART_write(uart, buff, n);
//    }else{
//    	n = sprintf(buff, " Command not recognized. Please enter eeprom read or eeprom reset");
//    	UART_write(uart, buff, n);
//    }
//
//    n = sprintf(buff, "\n\r");
//    UART_write(uart, buff, n);
//	return (0);
//}


int CMD_TCPhelp(int sockfd, int argc, char **argv)
{
    int32_t i32Index;
    int n, byteSend;
    char buff[128];
    tTCPCmdLineEntry *psCmdEntry;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;


    //
    // Check the argument count and return errors for too many or too few.
    //
    if(argc == 1){
        //
        // Start at the beginning of the command table
        //
        i32Index = 0;

        //
        // Get to the start of a clean line on the serial output.
        //
        n = sprintf(buff, "\n\rAvailable Commands\n\r------------------\n\n");
        //
        // Display strings until we run out of them.
        //
        while(g_psTCPCmdTable[i32Index].pcCmd)
        {
            n = sprintf(buff, "\r\x1b[32;1m%17s\x1b[0m %s\n", g_psTCPCmdTable[i32Index].pcCmd,
                        g_psTCPCmdTable[i32Index].pcHelp);
            byteSend = send(sockfd, buff, n, 0);
            byteSend = byteSend;
            i32Index++;
        }
    }
    if(argc == 2){
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        psCmdEntry = &g_psTCPCmdTable[0];

        //
        // Search through the command table until a null command string is
        // found, which marks the end of the table.
        //
        while (psCmdEntry->pcCmd) {
            //
            // If this command entry command string matches argv[0], then call
            // the function for this command, passing the command line
            // arguments.
            // ustrncmp(argv[1], psCmdEntry->pcCmd, 5)
            if (!strcmp(argv[1], psCmdEntry->pcCmd)) {
//            if (!ustrncmp(argv[1], psCmdEntry->pcCmd, 10)) {
                n = sprintf(buff, "\n\rCommand: \x1b[32;1m%s\x1b[0m\n\r------------------\n\r  %s\n", argv[1], psCmdEntry->pcHelp + 3);
                byteSend = send(sockfd, buff, n, 0);
            }

            //
            // Not found, so advance to the next entry.
            //
            psCmdEntry++;
        }
    }

    if(argc > 2) {
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    byteSend = send(sockfd, buff, n, 0);

    return (0);
}




int CMD_TCP_volume(int sockfd, int argc, char **argv)
{
//    int32_t i32Index;
    int n;
    char buff[128];
//    tTCPCmdLineEntry *psCmdEntry;

    ALTOAmp_zoneVolumeData_t volumeData;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;


    //
    // Check the argument count and return errors for too many or too few.
    //
    if(argc == 5){
        if (!strcmp(argv[1], "set")) {
            volumeData.zone = atoi(argv[2]);
            volumeData.zone1Volume = atoi(argv[3]);
            volumeData.zone2Volume = atoi(argv[4]);
            vDevice_sendCharDataMsg( 33,
                                     APP_MSG_SERVICE_WRITE,
                                     sockfd,
                                     SERVICE_ALTO_AMP_VOLUME_UUID, CHARACTERISTIC_ALTO_AMP_SET_ID,
                                     TCPRCDEVICE_ID,
                                     SERVICE_TCP_REMOTECONTROL_ASCII_VOLUMEN_RETURN_UUID, 1,
                                     (uint8_t *)&volumeData, sizeof(ALTOAmp_zoneVolumeData_t) );
        }else{
            return(CMDLINE_INVALID_ARG);
        }
    }
    if(argc == 2){
        if (!strcmp(argv[1], "get")) {
            vDevice_sendCharDataMsg( 33,
                                     APP_MSG_SERVICE_WRITE,
                                     sockfd,
                                     SERVICE_ALTO_AMP_VOLUME_UUID, CHARACTERISTIC_ALTO_AMP_GET_ID,
                                     TCPRCDEVICE_ID,
                                     SERVICE_TCP_REMOTECONTROL_ASCII_VOLUMEN_RETURN_UUID, 1,
                                     (uint8_t *)0, 0 );
        }else{
            return(CMDLINE_INVALID_ARG);
        }
    }

    if(argc > 5) {
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    send(sockfd, buff, n, 0);

    return (0);
}

//*****************************************************************************
//
// Table of valid command strings, callback functions and help messages.  This
// is used by the cmdline module.
//
//*****************************************************************************
//tCmdLineEntry g_psCmdTable[] =
//{
//    {"help",   CMD_help,   " : Display list of commands" },
//    {"?",      CMD_help,   " : Display list of commands" },
//    {"contact",CMD_contact," : Display contact information"},
//    {"about"  ,CMD_about,  " : Display information about this unit"},
////    {"date",   CMD_date,   " : Set Date \"DD/MM/YYYY\"."},
////    {"time12", CMD_time12, " : Set Time 12HR style \"HH:MM:XX\" "
////                           "XX = AM or PM"},
////    {"time24", CMD_time24, " : Set Time 24HR style \"HH:MM\"."},
//    {"cls",    CMD_cls,    " : Clear the terminal screen"},
////	{"start",  CMD_start,  " : Start test"},
////	{"stop",   CMD_stop,   " : Stop test"},
////	{"eeprom",   CMD_eeprom, " : eeprom read, eeprom reset"},
//    {"reprogram", CMD_reprogram,    " : Enter in programming mode"},
//    {"status",   CMD_status,   " : Display the internal state"},
//    {"voltage",   CMD_voltages,   " : Display input voltages"},
//    {"temp",   CMD_temp,   " : Display uController temperature"},
//    {"reboot",   CMD_reboot,   " : Reset the system"},
//    { 0, 0, 0 }
//};
//
//tUSBCmdLineEntry g_psUSBCmdTable[] =
//{
//    {"help",   CMD_USBhelp,   " : Display list of commands" },
//    {"?",      CMD_USBhelp,   " : Display list of commands" },
//    {"contact",CMD_USBcontact," : Display contact information"},
//    {"about"  ,CMD_USBabout,  " : Display information about this unit"},
//    {"cls",    CMD_USBcls,    " : Clear the terminal screen"},
//    {"reprogram", CMD_USBreprogram," : Enter in programming mode"},
//    {"status",   CMD_USBstatus,   " : Display the internal state"},
//    {"voltage",   CMD_USBvoltages,   " : Display input voltages"},
//    {"temp",   CMD_USBtemp,   " : Display uController temperature"},
//    {"reboot",   CMD_USBreboot,   " : Reset the system"},
//    { 0, 0, 0 }
//};


tTCPCmdLineEntry g_psTCPCmdTable[] =
{
    {"help",   CMD_TCPhelp,   " : Display list of commands" },
    {"?",      CMD_TCPhelp,   " : Display list of commands" },
    {"volume",      CMD_TCP_volume,   " : volume set [zone] [vol1] [vol2], volume get" },
//    {"contact",CMD_USBcontact," : Display contact information"},
//    {"about"  ,CMD_USBabout,  " : Display information about this unit"},
    { 0, 0, 0 }
};

