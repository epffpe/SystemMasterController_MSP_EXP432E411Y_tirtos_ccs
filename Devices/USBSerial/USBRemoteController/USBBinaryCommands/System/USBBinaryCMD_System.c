/*
 * USBBinaryCMD_System.c
 *
 *  Created on: Oct 19, 2020
 *      Author: epenate
 */


#define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_SYSTEM_USBBINARYCMD_SYSTEM_GLOBAL
#include "includes.h"



void vUSBBinaryCMD_Reboot(char *payload, int32_t size)
{
//    USBBIN_CMD_t *pTCPBinCmd = (USBBIN_CMD_t *) payload;
    int n;
    char response[sizeof(tUSBBinaryCMD_enterProgrammingModeResponse)];
    if (xUSBRCBinaryCMD_checksum((int *)payload, 2) == 0) {
        n = xUSBRCBinaryCMD_frameCreate(response, USBBIN_CMD_RebootResponse, NULL, 0);
        USBCDCD_sendData(USBCDCD_RemoteControl, response, n, WAIT_FOREVER);

        SFFS_Handle hSFFS;
        hSFFS = hSFFS_open(SFFS_Internal);
        xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);

//        hSFFS = hSFFS_open(SFFS_External);
//        xSFFS_lockMemoryForReboot(hSFFS, BIOS_WAIT_FOREVER);
//        vSFFS_close(hSFFS);

        Task_sleep((unsigned int)50);

//      wait for flash memory mutex

//        WatchdogUnlock(WATCHDOG0_BASE);
//        WatchdogResetDisable(WATCHDOG0_BASE);
        //    USBDCDTerm(0);
        USBDevDisconnect(USB0_BASE);

        Task_sleep((unsigned int)1);

        SysCtlReset();
    }

}

