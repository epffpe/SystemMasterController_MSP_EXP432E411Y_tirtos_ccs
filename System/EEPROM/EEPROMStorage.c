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


volatile tEEPROM_Data *INFO_get()
{
    return &g_sEEPROMData;
}

void INFO_set(tEEPROM_Data *info)
{
    EEPROMProgram((uint32_t *)info, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
    g_sEEPROMData = *info;
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

    if (g_sEEPROMData.eepromCheck != DEFAULT_EEPROM_CHECK) {
        g_sEEPROMData = g_sDefaultEEPROMData;
        EEPROMProgram((uint32_t *)&g_sEEPROMData, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
    }
}

