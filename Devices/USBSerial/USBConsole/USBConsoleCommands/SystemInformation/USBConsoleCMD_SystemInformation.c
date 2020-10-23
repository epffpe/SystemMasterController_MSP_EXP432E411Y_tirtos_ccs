/*
 * USBConsoleCMD_SystemInformation.c
 *
 *  Created on: Jun 5, 2020
 *      Author: epffpe
 */

#define __DEVICES_USBSERIAL_USBCONSOLE_USBCONSOLECOMMANDS_SYSTEMINFORMATION_USBCONSOLECMD_SYSTEMINFORMATION_GLOBAL
#include "includes.h"
#include <ti/devices/msp432e4/driverlib/inc/hw_sysctl.h>


const char g_cCMDContatInformation[] = {
                                        "\r\n"
                                        ANSI_COLOR_YELLOW " Eugenio Penate\r\n" ANSI_COLOR_RESET
                                        " Electrical Engineer\r\n"
                                        " 978.466.5992\r\n"
                                        " epenate@altoaviation.com\r\n"
                                        "\n"
                                        " Located at ALTO Jupiter, FL\r\n"
                                        " ALTO Aviation / 880 Jupiter Park Drive suite 3 / Jupiter, FL 33458\r\n"
                                        "\n"
                                        " Corporate office\r\n"
                                        " ALTO Aviation / 86 Leominster Road / PO Box 399 / Sterling, MA 01564\r\n"
                                        " 800.814.0123 Toll Free / 978.466.5992 P / 978.466.5996 F\r\n"
                                        "\r\n"


};


const char g_cCMDAboutInformation[] = {
                                        "\r\n"
                                        ANSI_COLOR_YELLOW " System\r\n" ANSI_COLOR_RESET
                                        " Model number: %d\r\n"
                                        " Serial number: %06d\r\n"
                                        "\r\n"
                                        ANSI_COLOR_YELLOW " Firmware\r\n" ANSI_COLOR_RESET
                                        " System version: %d.%d.%d.%d\r\n"
                                        "\r\n"
                                        ANSI_COLOR_YELLOW " Chip ID\r\n" ANSI_COLOR_RESET
                                        " UniqueId: 0x%08X%08X%08X%08X\r\n"
                                        "\r\n"
};

int CMD_USBcontact(int argc, char **argv)
{
    USBCDCD_sendData(USBCDCD_Console, (char *)g_cCMDContatInformation, sizeof(g_cCMDContatInformation), BIOS_WAIT_FOREVER);
    return (0);
}


int CMD_USBabout(int argc, char **argv)
{
    int n;
    uint32_t uniqueID0, uniqueID1, uniqueID2, uniqueID3;
    volatile tEEPROM_Data *infoResponse;
    char buff[256];

    uniqueID0 = HWREG(SYSCTL_UNIQUEID0);
    uniqueID1 = HWREG(SYSCTL_UNIQUEID1);
    uniqueID2 = HWREG(SYSCTL_UNIQUEID2);
    uniqueID3 = HWREG(SYSCTL_UNIQUEID3);

    infoResponse = INFO_get();
    n = sprintf(buff, g_cCMDAboutInformation,
                infoResponse->boardHardwareVersion,
                infoResponse->unitSerialNumber,
                FIRMWARE_VERSION_MAJOR,
                FIRMWARE_VERSION_MINOR,
                FIRMWARE_VERSION_MONTH,
                FIRMWARE_VERSION_BUILD,
                uniqueID0,
                uniqueID1,
                uniqueID2,
                uniqueID3);
    USBCDCD_sendData(USBCDCD_Console, (char *)buff, n, BIOS_WAIT_FOREVER);
    return (0);
}

