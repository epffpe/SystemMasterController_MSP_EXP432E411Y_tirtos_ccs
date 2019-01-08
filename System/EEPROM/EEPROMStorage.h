/*
 * EEPROMStorage.h
 *
 *  Created on: Sep 7, 2017
 *      Author: epenate
 */

#ifndef EEPROMSTORAGE_H_
#define EEPROMSTORAGE_H_
#include <stdint.h>

#define xstr(s) str(s)
#define str(s) #s

#define PRODUCT_ID                          106623

#define FIRMWARE_VERSION_MAJOR              0
#define FIRMWARE_VERSION_MINOR              1
#define FIRMWARE_VERSION_MONTH              12
#define FIRMWARE_VERSION_BUILD              4


#define DEFAULT_MODEL_NUMBER                xstr(PRODUCT_ID)
#define DEFAULT_SERIAL_NUMBER               "123456"
#define DEFAULT_NUMBER_OF_CHANNELS          3
#define DEFAULT_UNIT_SERIAL_NUMBER          123456
#define DEFAULT_BOARD_HARDWARE_VERSION      1
#define DEFAULT_UNIT_SUBTYPE_CODE           1
#define DEFAULT_AIRCRAFT_MFG_CODE           1
#define DEFAULT_AIRCRAFT_MODEL_CODE         1
#define DEFAULT_AIRCRAFT_LAYOUT_CODE        1
#define DEFAULT_CAN_ADDRESS                 0
#define DEFAULT_CONFIG_VERSION              0
#define DEFAULT_MP_REVISION                 1
#define DEFAULT_EEPROM_CHECK                0xAA55
#define DEFAULT_EEPROM_SERIAL_BAUDRATE      115200
#define DEFAULT_EEPROM_CAN_BAUDRATE         500000

//ALTO MANUFACTURER INFORMATION
#define SOFTWARE_PN                         106558
#define UNIT_TYPE_CODE                      128

#define DEFAULT_EEPROM_ADDRESS              0x000
#define DEFAULT_EEPROM_DIO_CONFG            0x080



typedef struct {
    uint32_t unitSerialNumber;
    uint32_t serialBaudRate[8];
    uint32_t canBaudRate[2];
    uint16_t eepromCheck;
    uint8_t boardHardwareVersion;
    uint8_t unitSubTypeCode;
    uint8_t aircraftMFGCode;
    uint8_t aircraftModelCode;
    uint8_t aircraftLayoutCode;
    uint8_t configVersion;
    uint8_t mpRevision;
}tEEPROM_Data;

typedef struct {
    uint32_t dioCfg[6];
}tEEPROM_DIOCfgData;

#define EEPROM_DATA_PAYLOAD_SIZE      (sizeof(tEEPROM_Data) + 2 )


#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _EEPROMSTORAGE_GLOBAL
    #define EEPROMSTORAGE_EXT
#else
    #define EEPROMSTORAGE_EXT    extern
#endif

EEPROMSTORAGE_EXT volatile tEEPROM_Data *INFO_get();
EEPROMSTORAGE_EXT void INFO_set(tEEPROM_Data *info);
EEPROMSTORAGE_EXT void INFO_init();
EEPROMSTORAGE_EXT volatile tEEPROM_Data g_sEEPROMData;
EEPROMSTORAGE_EXT volatile tEEPROM_DIOCfgData g_sEEPROMDIOCfgData;
EEPROMSTORAGE_EXT volatile uint32_t g_ui32EEPROMInit;

#ifdef __cplusplus
}
#endif




#endif /* EEPROMSTORAGE_H_ */
