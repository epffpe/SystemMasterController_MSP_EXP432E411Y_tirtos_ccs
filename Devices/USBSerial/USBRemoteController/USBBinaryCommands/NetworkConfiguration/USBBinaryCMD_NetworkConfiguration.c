/*
 * USBBinaryCMD_NetworkConfiguration.c
 *
 *  Created on: Jun 9, 2020
 *      Author: epffpe
 */


#define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_NETWORKCONFIGURATION_USBBINARYCMD_NETWORKCONFIGURATION_GLOBAL
#include "includes.h"
#include <ti/ndk/inc/netmain.h>

void vUSBBinaryCMD_setMACAddress(char *payload, int32_t size)
{
    int n;
    tUSBBinaryCMD_setMACAddress_payload *pCmdPayload = (tUSBBinaryCMD_setMACAddress_payload *) payload;

    char response[sizeof(tUSBBinaryCMD_noPayload)];

    if (xUSBRCBinaryCMD_checksum((int *)payload, 4) == 0) {

        FlashUserSet(pCmdPayload->payload.user0, pCmdPayload->payload.user1);
        FlashUserSave();

        n = xUSBRCBinaryCMD_frameCreate(response, USBBIN_CMD_setMACAddressResponse, NULL, 0);
        USBCDCD_sendData(USBCDCD_RemoteControl, response, n, WAIT_FOREVER);

//        Task_sleep((unsigned int)50);
    }
}

void vUSBBinaryCMD_getMACAddress(char *payload, int32_t size)
{
    int n;
//    uint32_t ulUser0, ulUser1, ulUser2, ulUser3;

    char response[sizeof(tUSBBinaryCMD_getMACAddressResponseFrame)];
    tUSBBinaryCMD_macAddress_payload respPayload;

    if (xUSBRCBinaryCMD_checksum((int *)payload, 2) == 0) {
//        FlashAllUserRegisterGet(&ulUser0, &ulUser1, &ulUser2, &ulUser3);
        FlashUserGet(&respPayload.user0, &respPayload.user1);


        n = xUSBRCBinaryCMD_frameCreate(response, USBBIN_CMD_getMACAddressResponse, (char *)&respPayload, sizeof(tUSBBinaryCMD_macAddress_payload));
        USBCDCD_sendData(USBCDCD_RemoteControl, response, n, WAIT_FOREVER);

    }
}


void vUSBBinaryCMD_setIfConfig(char *payload, int32_t size)
{
    int n;
    tUSBBinaryCMD_setIpConfig_payload *pCmdPayload = (tUSBBinaryCMD_setIpConfig_payload *) payload;

    char response[sizeof(tUSBBinaryCMD_noPayload)];

    if (xUSBRCBinaryCMD_checksum((int *)payload, 2 + sizeof(tUSBBinaryCMD_ipconfig_payload)/4 ) == 0) {
        /*
         * Configure IP
         */
        vEEPIpConfg_set((tEEPROM_ipConfigData *)&pCmdPayload->payload);

        n = xUSBRCBinaryCMD_frameCreate(response, USBBIN_CMD_setIfConfigResponse, NULL, 0);
        USBCDCD_sendData(USBCDCD_RemoteControl, response, n, WAIT_FOREVER);

    }
}

void vUSBBinaryCMD_getIfConfig(char *payload, int32_t size)
{
    int n;
    tEEPROM_ipConfigData *psEEPIpConfig;
    char response[sizeof(tUSBBinaryCMD_getIpConfigResponseFrame)];
    tUSBBinaryCMD_ipconfig_payload respPayload;

    if (xUSBRCBinaryCMD_checksum((int *)payload, 2) == 0) {

//        respPayload.isIPAuto = 1;
//        respPayload.IPAddr = inet_addr("192.168.1.2");
//        respPayload.IPMask = inet_addr("255.255.255.0");
//        respPayload.IPGateAddr = inet_addr("192.168.1.1");
//        strcpy(respPayload.Domain, "ALTOTECH.net");
        psEEPIpConfig = (tEEPROM_ipConfigData *)psEEPIpConfg_get();
        respPayload.isIPAuto = psEEPIpConfig->isIPAuto;
        respPayload.IPAddr = psEEPIpConfig->IPAddr;
        respPayload.IPMask = psEEPIpConfig->IPMask;
        respPayload.IPGateAddr = psEEPIpConfig->IPGateAddr;
        if (strlen(psEEPIpConfig->Domain) < USBBINCMD_CFG_DOMAIN_MAX) {
            strcpy(respPayload.Domain, psEEPIpConfig->Domain);
        }else {
            memcpy(respPayload.Domain, psEEPIpConfig->Domain, USBBINCMD_CFG_DOMAIN_MAX);
        }

        n = xUSBRCBinaryCMD_frameCreate(response, USBBIN_CMD_getIfConfigResponse, (char *)&respPayload, sizeof(tUSBBinaryCMD_ipconfig_payload));
        USBCDCD_sendData(USBCDCD_RemoteControl, response, n, WAIT_FOREVER);

    }
}


