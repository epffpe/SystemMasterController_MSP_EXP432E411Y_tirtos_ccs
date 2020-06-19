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



typedef struct {
    uint8_t build;
    uint8_t month;
    uint8_t minor;
    uint8_t major;
}tUSBBinaryCMD_getFirmwareVersion_payload;

#define USBBINCMD_FIRMWARE_VERSION_PAYLOAD_SIZE      ((sizeof(tUSBBinaryCMD_getFirmwareVersion_payload) + 3)/4 )


typedef struct {
    uint32_t    header;
    uint32_t    cmdType;
    tUSBBinaryCMD_getFirmwareVersion_payload payload;
    uint32_t    crc;
    uint32_t    eof;
}tUSBBinaryCMD_getFirmwareVersionResponseFrame;


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
__DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_EXT
void vUSBBinaryCMD_getFirmwareVersion(char *payload, int32_t size);

#ifdef __cplusplus
}
#endif




#endif /* DEVICES_USBSERIAL_USBREMOTECONTROLLER_USBBINARYCOMMANDS_FIRMWAREUPGRADE_USBBINARYCMD_FIRMWAREUPGRADE_H_ */
