/*
 * USBConsoleCommands.c
 *
 *  Created on: Jun 5, 2020
 *      Author: epffpe
 */

#define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_USBCONSOLECOMMANDS_GLOBAL
#include "includes.h"

int CMD_USBhelp(int argc, char **argv);
int CMD_USBcls(int argc, char **argv);
int CMD_USBreboot(int argc, char **argv);
int CMD_USBenterProgrammingMode(int argc, char **argv);

const unsigned char g_pucUSBCONSOLEWelcome[] = "\r\n>>>>>>> Welcome to the System Master Controller <<<<<<<\r\n";

const unsigned char g_pcUSBCONSOLEprompt[] = "\r" ANSI_COLOR_GREEN "epenate@ALTOAviation: > " ANSI_COLOR_RESET;


const char g_cCMDrebootUSBInformation[] = {
                                           "\r\n"
                                           ANSI_COLOR_MAGENTA
                                           " Rebooting the system\r\n\r\n"
                                           ANSI_COLOR_RESET
                                           ANSI_COLOR_CYAN
                                           " Close this terminal and open a new one!"
                                           ANSI_COLOR_RESET
};


const char g_cCMDdfuUSBInformation[] = {
                                           "\r\n"
                                           ANSI_COLOR_MAGENTA
                                           " Entering in Device Firmware Upgrade (DFU) mode\r\n\r\n"
                                           ANSI_COLOR_RESET
                                           ANSI_COLOR_CYAN
                                           " Close this terminal and open a new one!"
                                           ANSI_COLOR_RESET
};




tUSBCmdLineEntry g_psUSBCmdTable[] =
{
    {"help",   CMD_USBhelp,   " : Display list of commands" },
    {"?",      CMD_USBhelp,   " : Display list of commands" },
    {"cls",    CMD_USBcls,    " : Clear the terminal screen"},
    {"contact",CMD_USBcontact," : Display contact information"},
    {"about"  ,CMD_USBabout,  " : Display information about this unit"},
    {"reboot",   CMD_USBreboot,   " : Reset the system"},
    {"dfu", CMD_USBenterProgrammingMode," : Enter in programming mode"},
    {"set",     CMD_USBset,     " : Set parameters on the system"},
//    {"status",   CMD_USBstatus,   " : Display the internal state"},
//    {"voltage",   CMD_USBvoltages,   " : Display input voltages"},
//    {"temp",   CMD_USBtemp,   " : Display uController temperature"},
    { 0, 0, 0 }
};



/*----------------------------------------------------------------------------
  Line Editor
 *----------------------------------------------------------------------------*/
void USBCONSOLE_getline (char *line, int n)
{
    int  cnt = 0;
    char c;
    char space;
    unsigned int nbytes;

    nbytes = nbytes;

    do {
//        nbytes = UART_read(uart, &c, 1);
        nbytes = USBCDCD_receiveData(USBCDCD_Console, (unsigned char *)&c, 1, BIOS_WAIT_FOREVER);
        if (c == CR) c = LF;                    /* read character                 */
        if (c == BACKSPACE  ||  c == DEL) {     /* process backspace              */
            if (cnt != 0)  {
                cnt--;                              /* decrement count                */
                line--;                             /* and line pointer               */
                space = BACKSPACE;
//                UART_write(uart, &space, 1);                /* echo backspace                 */
                USBCDCD_sendData(USBCDCD_Console, &space, 1, BIOS_WAIT_FOREVER);
                space = ' ';
//                UART_write(uart, &space, 1);
                USBCDCD_sendData(USBCDCD_Console, &space, 1, BIOS_WAIT_FOREVER);
                space = BACKSPACE;
//                UART_write(uart, &space, 1);
                USBCDCD_sendData(USBCDCD_Console, &space, 1, BIOS_WAIT_FOREVER);
            }
        }
        else if (c != CNTLQ && c != CNTLS) {    /* ignore Control S/Q             */
            *line = c;
//            UART_write(uart, &c, 1);
            USBCDCD_sendData(USBCDCD_Console, &c, 1, BIOS_WAIT_FOREVER);
//          putchar (*line = c);                  /* echo and store character       */
            line++;                               /* increment line pointer         */
            cnt++;                                /* and count                      */
        }
    } while (cnt < n - 1  &&  c != LF);       /* check limit and line feed      */
    *(line - 1) = 0;                          /* mark end of string             */
}






Void vUSBConsoleWorkerFxn(UArg arg0, UArg arg1)
{
    int n;
    char cmdbuf[64], buff[64];
    int32_t i32CmdStatus;


    USBCDCD_waitForConnect(USBCDCD_Console, BIOS_WAIT_FOREVER);
//    Task_sleep(1000);
    USBCDCD_sendData(USBCDCD_Console, (char *)g_pucUSBCONSOLEWelcome, sizeof(g_pucUSBCONSOLEWelcome), BIOS_WAIT_FOREVER);
    while(1){
        USBCDCD_waitForConnect(USBCDCD_Console, BIOS_WAIT_FOREVER);
        USBCDCD_sendData(USBCDCD_Console, (char *)g_pcUSBCONSOLEprompt, sizeof(g_pcUSBCONSOLEprompt), BIOS_WAIT_FOREVER);
        USBCONSOLE_getline (cmdbuf, 64);
        //
        // Pass the line from the user to the command processor.
        // It will be parsed and valid commands executed.
        //
        i32CmdStatus = CmdLineProcessUSB(cmdbuf);

        //
        // Handle the case of bad command.
        //
        if(i32CmdStatus == CMDLINE_BAD_CMD)
        {
            n = sprintf(buff, ANSI_COLOR_RED "Command not recognized!\n" ANSI_COLOR_RESET);
            USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        }

        //
        // Handle the case of too many arguments.
        //
        else if(i32CmdStatus == CMDLINE_TOO_MANY_ARGS)
        {
            n = sprintf(buff, ANSI_COLOR_RED "Too many arguments for command processor!\n" ANSI_COLOR_RESET);
            USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        }

        //
        // Handle the case of too few arguments.
        //
        else if(i32CmdStatus == CMDLINE_TOO_FEW_ARGS)
        {
            n = sprintf(buff, ANSI_COLOR_RED "Too few arguments for command processor!\n" ANSI_COLOR_RESET);
            USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        }
    }

}




int CMD_USBhelp(int argc, char **argv)
{
    int32_t i32Index;
    int n;
    char buff[128];
    tUSBCmdLineEntry *psCmdEntry;
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
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        //
        // Display strings until we run out of them.
        //
        while(g_psUSBCmdTable[i32Index].pcCmd)
        {
            n = sprintf(buff, "\r\x1b[32;1m%17s\x1b[0m %s\n", g_psUSBCmdTable[i32Index].pcCmd,
                        g_psUSBCmdTable[i32Index].pcHelp);
            USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
            i32Index++;
        }
    }
    if(argc == 2){
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        psCmdEntry = &g_psUSBCmdTable[0];

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
            if (!strncmp(argv[1], psCmdEntry->pcCmd, 10)) {
                n = sprintf(buff, "\n\rCommand: \x1b[32;1m%s\x1b[0m\n\r------------------\n\r  %s\n", argv[1], psCmdEntry->pcHelp + 3);
                USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
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
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}

//*****************************************************************************
//
// Command: cls
//
// Clear the terminal screen.
//
//*****************************************************************************

int CMD_USBcls(int argc, char **argv)
{
    int n;
    char buff[32];

    n = sprintf(buff, "\033[2J\033[H");
    USBCDCD_sendData(USBCDCD_Console, (char *)buff, n, BIOS_WAIT_FOREVER);
    return (0);
}


//*****************************************************************************
//
// Command: reboot
//
// Reset the system.
//
//*****************************************************************************

int CMD_USBreboot(int argc, char **argv)
{
    USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDrebootUSBInformation, sizeof(g_cCMDrebootUSBInformation), BIOS_WAIT_FOREVER);

    SFFS_Handle hSFFS;
    hSFFS = hSFFS_open(SFFS_Internal);
    xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);
//
//    hSFFS = hSFFS_open(SFFS_External);
//    xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);

    Task_sleep(50);

//      wait for flash memory mutex

//        WatchdogUnlock(WATCHDOG0_BASE);
//        WatchdogResetDisable(WATCHDOG0_BASE);
    //    USBDCDTerm(0);
    USBDevDisconnect(USB0_BASE);

    Task_sleep(50);

    SysCtlReset();

    return (0);
}

//*****************************************************************************
//
// Command: reboot
//
// Reset the system.
//
//*****************************************************************************

int CMD_USBenterProgrammingMode(int argc, char **argv)
{
    USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDdfuUSBInformation, sizeof(g_cCMDdfuUSBInformation), BIOS_WAIT_FOREVER);

    SFFS_Handle hSFFS;
    hSFFS = hSFFS_open(SFFS_Internal);
    xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);
//
//    hSFFS = hSFFS_open(SFFS_External);
//    xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);

    Task_sleep(50);

//      wait for flash memory mutex

    WatchdogUnlock(WATCHDOG0_BASE);
    WatchdogResetDisable(WATCHDOG0_BASE);
    //    USBDCDTerm(0);
    USBDevDisconnect(USB0_BASE);

    JumpToBootLoader();

    return (0);
}

