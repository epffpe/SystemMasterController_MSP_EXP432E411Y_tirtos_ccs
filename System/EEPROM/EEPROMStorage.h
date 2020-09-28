/*
 * EEPROMStorage.h
 *
 *  Created on: Sep 7, 2017
 *      Author: epenate
 */

#ifndef EEPROMSTORAGE_H_
#define EEPROMSTORAGE_H_
#include <stdint.h>

//#define xstr(s) str(s)
//#define str(s) #s

/*
 *
 * a.b.c.d
 * Increments : when
 * - d: bug fixes
 * - c: maintenance, e.g. performance improvement
 * - b: new features
 * - a: architecture change
 *
 */

//#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
//
//DEBUG("Something went wrong in iteration: %d", i);

#define PRODUCT_ID                          106623

#define FIRMWARE_VERSION_MAJOR              0
#define FIRMWARE_VERSION_MINOR              2
#define FIRMWARE_VERSION_MONTH              10
#define FIRMWARE_VERSION_BUILD              11


#define DEFAULT_MODEL_NUMBER                xstr(PRODUCT_ID)
#define DEFAULT_SERIAL_NUMBER               "123456"
#define DEFAULT_NUMBER_OF_CHANNELS          3
#define DEFAULT_UNIT_SERIAL_NUMBER          123456
#define DEFAULT_UDP_UNIT_NUMBER             1
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
#define DEFAULT_EEPROM_MEM_TEST             0x200
#define DEFAULT_EEPROM_MAC_CONFG            0x290
#define DEFAULT_EEPROM_IP_CONFG             0x300



typedef struct {
    uint32_t unitSerialNumber;
    uint32_t udpUnitNumber;
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
    uint32_t    DIDebounceDly;
    uint32_t    DIRptStartDly;
    uint32_t    DIRptDly;
    uint8_t     DIBypassEn;               /* Bypass enable switch (Bypass when TRUE) */
    uint8_t     DIModeSel;                /* Discrete input channel mode selector */
    uint8_t     DIDebounceEn;
}tEEPROM_DIConfigData;

typedef struct {
    uint32_t    DOA;
    uint32_t    DOB;
    uint32_t    DOBCtr;
    uint8_t     DOBypassEn;
    uint8_t     DOModeSel;
    uint8_t     DOBlinkEnSel;
    uint8_t     DOInv;
}tEEPROM_DOConfigData;

typedef struct {
    uint32_t                dioCfg[6];
    uint32_t                doSyncCtrMax;
    tEEPROM_DIConfigData    diConfig[6];
    tEEPROM_DIConfigData    di5VOutStatus;
    tEEPROM_DIConfigData    diInternal5VSwStatus;
    tEEPROM_DIConfigData    diIRDARx;
    tEEPROM_DIConfigData    diTempAlert;
    tEEPROM_DOConfigData    doConfig[6];
    tEEPROM_DOConfigData    do5VOutEn;
    tEEPROM_DOConfigData    doPWRPeripheralEn;
    tEEPROM_DOConfigData    doUARTDebug;
    tEEPROM_DOConfigData    doSerial5En;
    tEEPROM_DOConfigData    doLEDD6;
    tEEPROM_DOConfigData    doLEDD20;
    tEEPROM_DOConfigData    doUSB0EPEn;

}tEEPROM_DIOCfgData;




typedef struct {
    uint32_t                test;
}tEEPROM_memoryTestData;

#define EEPROM_DATA_PAYLOAD_SIZE      (sizeof(tEEPROM_Data) + 2 )




/* Max IPNet Domain name Length - Change requires NETTOOLS rebuild */
#define EEPROM_CFG_DOMAIN_MAX  64

typedef struct {
    uint32_t    isIPAuto;               /* staticIP = 0, DHCP=1 */
    uint32_t    IPAddr;                 /* IP Address */
    uint32_t    IPMask;                 /* Subnet Mask */
    uint32_t    IPGateAddr;             /* Gateway IP Address */
    char        Domain[EEPROM_CFG_DOMAIN_MAX]; /* IPNet Domain Name */
}tEEPROM_ipConfigData;

typedef struct {
    uint32_t    macReg0;
    uint32_t    macReg1;
}tEEPROM_macConfigData;


#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _EEPROMSTORAGE_GLOBAL
    #define EEPROMSTORAGE_EXT
#else
    #define EEPROMSTORAGE_EXT    extern
#endif

EEPROMSTORAGE_EXT
volatile tEEPROM_Data *INFO_get();
EEPROMSTORAGE_EXT
void INFO_set(tEEPROM_Data *info);
EEPROMSTORAGE_EXT
void INFO_init();
EEPROMSTORAGE_EXT
volatile tEEPROM_DIOCfgData *psEEPDIOConfg_get();
EEPROMSTORAGE_EXT
void vEEPDIOConfg_set(tEEPROM_DIOCfgData *info);
EEPROMSTORAGE_EXT
volatile tEEPROM_ipConfigData *psEEPIpConfg_get();
EEPROMSTORAGE_EXT
void vEEPIpConfg_set(tEEPROM_ipConfigData *info);

EEPROMSTORAGE_EXT
volatile tEEPROM_macConfigData *psEEPMACConfg_get();
EEPROMSTORAGE_EXT
void vEEPMACConfg_set(tEEPROM_macConfigData *info);


EEPROMSTORAGE_EXT
volatile tEEPROM_Data g_sEEPROMData;
EEPROMSTORAGE_EXT
volatile tEEPROM_DIOCfgData g_sEEPROMDIOCfgData;
EEPROMSTORAGE_EXT
volatile tEEPROM_ipConfigData g_sEEPROMIpCfgData;
EEPROMSTORAGE_EXT
volatile tEEPROM_macConfigData g_sEEPROMMACCfgData;
EEPROMSTORAGE_EXT
volatile uint32_t g_ui32EEPROMInit;

#ifdef __cplusplus
}
#endif




#endif /* EEPROMSTORAGE_H_ */
