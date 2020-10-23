/*
 * USBConsoleCMD_SystemConfig.c
 *
 *  Created on: Oct 23, 2020
 *      Author: epenate
 */


#define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMCONFIG_USBCONSOLECMD_SYSTEMCONFIG_GLOBAL
#include "includes.h"

volatile bool isLogDisplayOn = false;

int CMD_USBsetLog(int argc, char **argv);
int CMD_USBuartDebug(int argc, char **argv);
int CMD_USBserial5En(int argc, char **argv);
int CMD_USB5VOut(int argc, char **argv);
int CMD_USBusbPowerEn(int argc, char **argv);


tUSBCmdLineEntry g_psUSBSetCmdTable[] =
{
 {"log",         CMD_USBsetLog,   " : set log {on/off}" },
 {"uartdebug",   CMD_USBuartDebug,   " : set uartdebug {on/off}" },
 {"serial5en",   CMD_USBserial5En,   " : set serial5en {on/off}" },
 {"5vout",       CMD_USB5VOut,   " : set 5vout {on/off}" },
 {"usbpen",      CMD_USBusbPowerEn,   " : set usbpen {on/off}" },
 { 0, 0, 0 }
};


const char g_cCMDSetLogInformation[] = {
                                        "\r\n"
                                        " Command: " ANSI_COLOR_GREEN "set log\r\n" ANSI_COLOR_RESET
                                        " ---------------------\r\n"
                                        " This command is used to enable and disable the log\r\n"
                                        " on the Serial Port Interface 1\r\n"
                                        "\r\n"
                                        " - Use "ANSI_COLOR_GREEN"set log on" ANSI_COLOR_RESET " to enable\r\n"
                                        " - Use "ANSI_COLOR_GREEN"set log off"ANSI_COLOR_RESET" to disable\r\n"
                                        "\r\n"
};

const char g_cCMDSetUartDebugInformation[] = {
                                        "\r\n"
                                        " Command: " ANSI_COLOR_GREEN "set uartdebug\r\n" ANSI_COLOR_RESET
                                        " ---------------------\r\n"
                                        " This command is used to enable and disable the uart port\r\n"
                                        " of the internal connector P2. It will conflict with the RS232.\r\n"
                                        "\r\n"
                                        " - Use "ANSI_COLOR_GREEN"set uartdebug on" ANSI_COLOR_RESET " to enable\r\n"
                                        " - Use "ANSI_COLOR_GREEN"set uartdebug off"ANSI_COLOR_RESET" to disable\r\n"
                                        "\r\n"
};

const char g_cCMDSet5VOutInformation[] = {
                                        "\r\n"
                                        " Command: " ANSI_COLOR_GREEN "set 5vout\r\n" ANSI_COLOR_RESET
                                        " ---------------------\r\n"
                                        " This command is used to enable and disable the 5V Output Power\r\n"
                                        " of the connector J1.\r\n"
                                        "\r\n"
                                        " - Use "ANSI_COLOR_GREEN"set 5vout on" ANSI_COLOR_RESET " to enable\r\n"
                                        " - Use "ANSI_COLOR_GREEN"set 5vout off"ANSI_COLOR_RESET" to disable\r\n"
                                        "\r\n"
};

int CMD_USBset(int argc, char **argv)
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
    switch(argc) {
    case 1:
        //
        // Start at the beginning of the command table
        //
        i32Index = 0;

        //
        // Get to the start of a clean line on the serial output.
        //
        n = sprintf(buff, "\n\rAvailable Arguments for set\n\r---------------------------\n\n");
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        //
        // Display strings until we run out of them.
        //
        while(g_psUSBSetCmdTable[i32Index].pcCmd)
        {
            n = sprintf(buff, "\r\x1b[32;1m%17s\x1b[0m %s\n", g_psUSBSetCmdTable[i32Index].pcCmd,
                        g_psUSBSetCmdTable[i32Index].pcHelp);
            USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
            i32Index++;
        }
        break;
    case 2:
    case 3:
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        psCmdEntry = &g_psUSBSetCmdTable[0];

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
                return (psCmdEntry->pfnCmd(argc, argv));
            }

            //
            // Not found, so advance to the next entry.
            //
            psCmdEntry++;
        }

        break;
    default:
        return(CMDLINE_TOO_MANY_ARGS);

    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}


int CMD_USBsetLog(int argc, char **argv)
{
    int n;
    char buff[64];
    tEEPROM_debugCfgData *psDebugCfgData, sDebugCfgData;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;

    //
    // Check the argument count and return errors for too many or too few.
    //
    switch(argc) {
    case 1:
        return(CMDLINE_TOO_FEW_ARGS);
    case 2:
        USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDSetLogInformation, sizeof(g_cCMDSetLogInformation), BIOS_WAIT_FOREVER);

        break;
    case 3:
        psDebugCfgData = (tEEPROM_debugCfgData *)psEEPDebugConfg_get();
        sDebugCfgData = *psDebugCfgData;

        if (!strncmp(argv[2], "on", 3)) {
            if(!psDebugCfgData->sysLogDisplay) {
                if (!isLogDisplayOn) SMCDisplay_init();
                sDebugCfgData.sysLogDisplay = 1;
                vEEPDebugConfg_set(&sDebugCfgData);
                isLogDisplayOn = true;
                n = sprintf(buff, "\n\rInitializing Log on Serial Interface 1\n\r");
            }else {
                n = sprintf(buff, "\n\rLog on Serial Interface 1 is already initialized\n\r");
            }
        }else if (!strncmp(argv[2], "off", 4)) {
            if(psDebugCfgData->sysLogDisplay) {
                //                SMCDisplay_close();
                sDebugCfgData.sysLogDisplay = 0;
                vEEPDebugConfg_set(&sDebugCfgData);
                n = sprintf(buff, "\n\rClosing Log on Serial Interface 1\n\rPlease reboot the system\n\r");
            }else {
                n = sprintf(buff, "\n\rLog is already closed on Serial Interface 1\n\r");
            }
        }else {
            return CMDLINE_BAD_CMD;
        }

        //
        // Get to the start of a clean line on the serial output.
        //
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        break;
    default:
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}



int CMD_USBuartDebug(int argc, char **argv)
{
    int n;
    char buff[64];
    tEEPROM_debugCfgData *psDebugCfgData, sDebugCfgData;
    tUSBCmdLineEntry *psCmdEntry;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;

    //
    // Check the argument count and return errors for too many or too few.
    //
    switch(argc) {
    case 1:
        return(CMDLINE_TOO_FEW_ARGS);
    case 2:
        USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDSetUartDebugInformation, sizeof(g_cCMDSetUartDebugInformation), BIOS_WAIT_FOREVER);
        break;
    case 3:
        psDebugCfgData = (tEEPROM_debugCfgData *)psEEPDebugConfg_get();
        sDebugCfgData = *psDebugCfgData;

        if (!strncmp(argv[2], "on", 3)) {
            DOSet(DIO_UART_DEBUG, DO_ON);
            n = sprintf(buff, "\n\rSetting UART Debug ON\n\r");
        }else if (!strncmp(argv[2], "off", 4)) {
            DOSet(DIO_UART_DEBUG, DO_OFF);
            n = sprintf(buff, "\n\rSetting UART Debug OFF\n\r");
        }else {
            return CMDLINE_BAD_CMD;
        }

        //
        // Get to the start of a clean line on the serial output.
        //
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        break;
    default:
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}



int CMD_USBserial5En(int argc, char **argv)
{
    int n;
    char buff[64];
    tEEPROM_debugCfgData *psDebugCfgData, sDebugCfgData;
    tUSBCmdLineEntry *psCmdEntry;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;

    //
    // Check the argument count and return errors for too many or too few.
    //
    switch(argc) {
    case 1:
        return(CMDLINE_TOO_FEW_ARGS);
    case 2:
        USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDSetLogInformation, sizeof(g_cCMDSetLogInformation), BIOS_WAIT_FOREVER);
        break;
    case 3:
        psDebugCfgData = (tEEPROM_debugCfgData *)psEEPDebugConfg_get();
        sDebugCfgData = *psDebugCfgData;

        if (!strncmp(argv[2], "on", 3)) {
            DOSet(DIO_SERIAL5_EN_, DO_ON);
            n = sprintf(buff, "\n\rSetting Serial 5 Enable ON\n\r");
        }else if (!strncmp(argv[2], "off", 4)) {
            DOSet(DIO_SERIAL5_EN_, DO_OFF);
            n = sprintf(buff, "\n\rSetting Serial 5 Enable OFF\n\r");
        }else {
            return CMDLINE_BAD_CMD;
        }

        //
        // Get to the start of a clean line on the serial output.
        //
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        break;
    default:
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}



int CMD_USB5VOut(int argc, char **argv)
{
    int n;
    char buff[64];
    tEEPROM_debugCfgData *psDebugCfgData, sDebugCfgData;
    tUSBCmdLineEntry *psCmdEntry;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;

    //
    // Check the argument count and return errors for too many or too few.
    //
    switch(argc) {
    case 1:
        return(CMDLINE_TOO_FEW_ARGS);
    case 2:
        USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDSet5VOutInformation, sizeof(g_cCMDSet5VOutInformation), BIOS_WAIT_FOREVER);
        break;
    case 3:
        psDebugCfgData = (tEEPROM_debugCfgData *)psEEPDebugConfg_get();
        sDebugCfgData = *psDebugCfgData;

        if (!strncmp(argv[2], "on", 3)) {
            DOSet(DIO_5V_OUT_EN, DO_ON);
            n = sprintf(buff, "\n\rSetting 5V Output Power ON\n\r");
        }else if (!strncmp(argv[2], "off", 4)) {
            DOSet(DIO_5V_OUT_EN, DO_OFF);
            n = sprintf(buff, "\n\rSetting 5V Output Power OFF\n\r");
        }else {
            return CMDLINE_BAD_CMD;
        }

        //
        // Get to the start of a clean line on the serial output.
        //
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        break;
    default:
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}



int CMD_USBusbPowerEn(int argc, char **argv)
{
    int n;
    char buff[64];
    tEEPROM_debugCfgData *psDebugCfgData, sDebugCfgData;
    tUSBCmdLineEntry *psCmdEntry;
    //
    // Keep the compiler happy.
    //
    (void)argc;
    (void)argv;

    //
    // Check the argument count and return errors for too many or too few.
    //
    switch(argc) {
    case 1:
        return(CMDLINE_TOO_FEW_ARGS);
    case 2:
        USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDSetLogInformation, sizeof(g_cCMDSetLogInformation), BIOS_WAIT_FOREVER);
        break;
    case 3:
        psDebugCfgData = (tEEPROM_debugCfgData *)psEEPDebugConfg_get();
        sDebugCfgData = *psDebugCfgData;

        if (!strncmp(argv[2], "on", 3)) {
            DOSet(DIO_USB0EPEN, DO_ON);
            n = sprintf(buff, "\n\rSetting USB Power ON\n\r");
        }else if (!strncmp(argv[2], "off", 4)) {
            DOSet(DIO_USB0EPEN, DO_OFF);
            n = sprintf(buff, "\n\rSetting USB Power OFF\n\r");
        }else {
            return CMDLINE_BAD_CMD;
        }

        //
        // Get to the start of a clean line on the serial output.
        //
        USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);
        break;
    default:
        return(CMDLINE_TOO_MANY_ARGS);
    }
    //
    // Leave a blank line after the help strings.
    //
    n = sprintf(buff, "\n\r");
    USBCDCD_sendData(USBCDCD_Console, buff, n, BIOS_WAIT_FOREVER);

    return (0);
}


