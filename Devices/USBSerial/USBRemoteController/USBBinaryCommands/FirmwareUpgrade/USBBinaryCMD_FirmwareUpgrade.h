/*
 * USBBinaryCMD_FirmwareUpgrade.h
 *
 *  Created on: Jun 4, 2020
 *      Author: epffpe
 */

#ifndef DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_H_
#define DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_H_


typedef struct {
    uint32_t header;
    uint32_t kind;
    uint32_t crc;
    uint32_t eof;
}tUSBBinaryCMD_enterProgrammingModeResponse;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_GLOBAL
    #define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_EXT
#else
    #define __DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_EXT  extern
#endif

__DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_EXT
void vUSBBinaryCMD_enterProgrammingMode(char *payload, int32_t size);

#ifdef __cplusplus
}
#endif




#endif /* DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_H_ */
