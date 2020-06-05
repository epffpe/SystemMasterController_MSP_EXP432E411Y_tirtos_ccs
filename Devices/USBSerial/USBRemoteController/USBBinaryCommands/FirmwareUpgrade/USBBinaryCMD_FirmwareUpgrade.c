/*
 * USBBinaryCMD_FirmwareUpgrade.c
 *
 *  Created on: Jun 4, 2020
 *      Author: epffpe
 */


#define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_GLOBAL
#include "includes.h"





void vUSBBinaryCMD_enterProgrammingMode(char *payload, int32_t size)
{
//    USBBIN_CMD_t *pTCPBinCmd = (USBBIN_CMD_t *) payload;
    int n;
    char response[sizeof(tUSBBinaryCMD_enterProgrammingModeResponse)];
    if (xUSBRCBinaryCMD_checksum((int *)payload, 2) == 0) {
        n = xUSBRCBinaryCMD_frameCreate(response, USBBIN_CMD_firmwareUpgradeResponse, NULL, 0);
        USBCDCD_sendData(USBCDCD_RemoteControl, response, n, WAIT_FOREVER);

        Task_sleep((unsigned int)50);

//      wait for flash memory mutex

//        WatchdogUnlock(WATCHDOG0_BASE);
//        WatchdogResetDisable(WATCHDOG0_BASE);
        //    USBDCDTerm(0);
        USBDevDisconnect(USB0_BASE);


        JumpToBootLoader();
    }

}
