/*
 * EEPROMStorage.c
 *
 *  Created on: Sep 7, 2017
 *      Author: epenate
 */


#define _EEPROMSTORAGE_GLOBAL
#include "includes.h"




const tEEPROM_Data g_sDefaultEEPROMData =
{
 .unitSerialNumber = DEFAULT_UNIT_SERIAL_NUMBER,
 .serialBaudRate[0] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[1] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[2] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[3] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[4] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[5] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[6] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .serialBaudRate[7] = DEFAULT_EEPROM_SERIAL_BAUDRATE,
 .canBaudRate[0] = DEFAULT_EEPROM_CAN_BAUDRATE,
 .canBaudRate[1] = DEFAULT_EEPROM_CAN_BAUDRATE,
 .eepromCheck = DEFAULT_EEPROM_CHECK,
 .boardHardwareVersion = DEFAULT_BOARD_HARDWARE_VERSION,
 .unitSubTypeCode = DEFAULT_UNIT_SUBTYPE_CODE,
 .aircraftMFGCode = DEFAULT_AIRCRAFT_MFG_CODE,
 .aircraftModelCode = DEFAULT_AIRCRAFT_MODEL_CODE,
 .aircraftLayoutCode = DEFAULT_AIRCRAFT_LAYOUT_CODE,
 .configVersion = DEFAULT_CONFIG_VERSION,
 .mpRevision = DEFAULT_MP_REVISION,
};

// .dioCfg[0] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
// .dioCfg[0] = GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
const tEEPROM_DIOCfgData g_sDefaultEEPROMDIOCfgData =
{
 .dioCfg[0] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
 .dioCfg[1] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
 .dioCfg[2] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
 .dioCfg[3] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
 .dioCfg[4] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
 .dioCfg[5] = GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
 .diConfig[0] =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .diConfig[1] =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .diConfig[2] =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .diConfig[3] =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .diConfig[4] =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .diConfig[5] =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .di5VOutStatus =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_INV,
          .DIDebounceEn = 0,
         },
 .diInternal5VSwStatus =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_INV,
          .DIDebounceEn = 0,
         },
 .diIRDARx =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .diTempAlert =
         {
          .DIDebounceDly = 50,
          .DIRptStartDly = 500,
          .DIRptDly = 200,
          .DIBypassEn = 0,
          .DIModeSel = DI_MODE_DIRECT,
          .DIDebounceEn = 0,
         },
 .doConfig[0] =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .doConfig[1] =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .doConfig[2] =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .doConfig[3] =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .doConfig[4] =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .doConfig[5] =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .do5VOutEn =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
 .doPWRPeripheralEn =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = true
         },
 .doUARTDebug =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = true
         },
 .doSerial5En =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = true
         },
 .doLEDD6 =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = true
         },
 .doLEDD20 =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = true
         },
 .doUSB0EPEn =
         {
          .DOA = 100,
          .DOB = 200,
          .DOBCtr = 300,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN,
          .DOInv = false
         },
};


volatile tEEPROM_Data *INFO_get()
{
    return &g_sEEPROMData;
}

void INFO_set(tEEPROM_Data *info)
{
    EEPROMProgram((uint32_t *)info, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
    g_sEEPROMData = *info;
}


volatile tEEPROM_DIOCfgData *psEEPDIOConfg_get()
{
    return &g_sEEPROMDIOCfgData;
}

void vEEPDIOConfg_set(tEEPROM_DIOCfgData *info)
{
//    EEPROMProgram((uint32_t *)info, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
    EEPROMProgram((uint32_t *)&info, DEFAULT_EEPROM_DIO_CONFG, sizeof(tEEPROM_DIOCfgData));
    g_sEEPROMDIOCfgData = *info;
}


void INFO_init()
{
//    uint32_t pui32Data[8];
    uint32_t ui32EEPROMSize;
    uint32_t ui32TimeOutCounter = 1e6;

    ui32EEPROMSize = ui32EEPROMSize;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    ui32TimeOutCounter--;
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0) && ui32TimeOutCounter)
    {
        ui32TimeOutCounter--;
    }


    if (ui32TimeOutCounter == 0) {
        g_ui32EEPROMInit = EEPROM_INIT_ERROR;
        g_sEEPROMData = g_sDefaultEEPROMData;
        return;
    }

    g_ui32EEPROMInit = EEPROMInit();

    if (g_ui32EEPROMInit != EEPROM_INIT_OK){
        g_sEEPROMData = g_sDefaultEEPROMData;
        return;
    }

    ui32EEPROMSize = EEPROMSizeGet();

    EEPROMRead((uint32_t *)&g_sEEPROMData, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
    EEPROMRead((uint32_t *)&g_sEEPROMDIOCfgData, DEFAULT_EEPROM_DIO_CONFG, sizeof(tEEPROM_DIOCfgData));

    if (g_sEEPROMData.eepromCheck != DEFAULT_EEPROM_CHECK) {
        g_sEEPROMData = g_sDefaultEEPROMData;
        EEPROMProgram((uint32_t *)&g_sEEPROMData, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));

        g_sEEPROMDIOCfgData = g_sDefaultEEPROMDIOCfgData;
        EEPROMProgram((uint32_t *)&g_sEEPROMDIOCfgData, DEFAULT_EEPROM_DIO_CONFG, sizeof(tEEPROM_DIOCfgData));
    }
}

