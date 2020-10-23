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
 .udpUnitNumber = DEFAULT_UDP_UNIT_NUMBER,
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
 .doSyncCtrMax = 1000,
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
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .doConfig[1] =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .doConfig[2] =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .doConfig[3] =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .doConfig[4] =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .doConfig[5] =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .do5VOutEn =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
 .doPWRPeripheralEn =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = true
         },
 .doUARTDebug =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = true
         },
 .doSerial5En =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = true
         },
 .doLEDD6 =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = true
         },
 .doLEDD20 =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = true
         },
 .doUSB0EPEn =
         {
          .DOA = 500,
          .DOB = 1000,
          .DOBCtr = 0,
          .DOBypassEn = 0,
          .DOModeSel = DO_MODE_DIRECT,
          .DOBlinkEnSel = DO_BLINK_EN_NORMAL,
          .DOInv = false
         },
};

//typedef struct {
//    uint32_t    isIPAuto;               /* staticIP = 0, DHCP=1 */
//    uint32_t    IPAddr;                 /* IP Address */
//    uint32_t    IPMask;                 /* Subnet Mask */
//    uint32_t    IPGateAddr;             /* Gateway IP Address */
//    char        Domain[EEPROM_CFG_DOMAIN_MAX]; /* IPNet Domain Name */
//}tEEPROM_ipConfigData;


const tEEPROM_ipConfigData g_sDefaultEEPROMIPConfigData =
{
 .isIPAuto = 1,
 .IPAddr = 0x0201A8C0,
 .IPMask = 0x00FFFFFF,
 .IPGateAddr = 0x0101A8C0,
 .Domain = {"ALTO.net"},
};


const tEEPROM_macConfigData g_sDefaultEEPROMMACConfigData =
{
 .macReg0 = 0x00B61A00,
 .macReg1 = 0x0055BF03,
};



const tEEPROM_debugCfgData g_sDefaultEEPROMDebugConfigData =
{
 .sysLogDisplay = 0,
};



volatile tEEPROM_Data *INFO_get()
{
    return &g_sEEPROMData;
}

void INFO_set(tEEPROM_Data *info)
{
//    EEPROMProgram((uint32_t *)info, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
    g_sEEPROMData = *info;
    EEPROMProgram((uint32_t *)&g_sEEPROMData, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
}


volatile tEEPROM_DIOCfgData *psEEPDIOConfg_get()
{
    return &g_sEEPROMDIOCfgData;
}

void vEEPDIOConfg_set(tEEPROM_DIOCfgData *info)
{
//    EEPROMProgram((uint32_t *)info, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));
//    EEPROMProgram((uint32_t *)&info, DEFAULT_EEPROM_DIO_CONFG, sizeof(tEEPROM_DIOCfgData));
    g_sEEPROMDIOCfgData = *info;
    EEPROMProgram((uint32_t *)&g_sEEPROMDIOCfgData, DEFAULT_EEPROM_DIO_CONFG, sizeof(tEEPROM_DIOCfgData));
}


volatile tEEPROM_ipConfigData *psEEPIpConfg_get()
{
    return &g_sEEPROMIpCfgData;
}

void vEEPIpConfg_set(tEEPROM_ipConfigData *info)
{
    g_sEEPROMIpCfgData = *info;
    g_sEEPROMIpCfgData.Domain[EEPROM_CFG_DOMAIN_MAX - 1] = 0;
    EEPROMProgram((uint32_t *)&g_sEEPROMIpCfgData, DEFAULT_EEPROM_IP_CONFG, sizeof(tEEPROM_ipConfigData));
}


volatile tEEPROM_macConfigData *psEEPMACConfg_get()
{
    return &g_sEEPROMMACCfgData;
}

void vEEPMACConfg_set(tEEPROM_macConfigData *info)
{
    g_sEEPROMMACCfgData = *info;
    EEPROMProgram((uint32_t *)&g_sEEPROMMACCfgData, DEFAULT_EEPROM_MAC_CONFG, sizeof(tEEPROM_macConfigData));
}



volatile tEEPROM_debugCfgData *psEEPDebugConfg_get()
{
    return &g_sEEPROMDebugCfgData;
}

void vEEPDebugConfg_set(tEEPROM_debugCfgData *info)
{
    g_sEEPROMDebugCfgData = *info;
    EEPROMProgram((uint32_t *)&g_sEEPROMDebugCfgData, DEFAULT_EEPROM_DEBUG_CONFG_ADDRESS, sizeof(tEEPROM_debugCfgData));
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
    EEPROMRead((uint32_t *)&g_sEEPROMMACCfgData, DEFAULT_EEPROM_MAC_CONFG, sizeof(tEEPROM_macConfigData));
    EEPROMRead((uint32_t *)&g_sEEPROMIpCfgData, DEFAULT_EEPROM_IP_CONFG, sizeof(tEEPROM_ipConfigData));
    EEPROMRead((uint32_t *)&g_sEEPROMDebugCfgData, DEFAULT_EEPROM_DEBUG_CONFG_ADDRESS, sizeof(tEEPROM_debugCfgData));

    if (g_sEEPROMData.eepromCheck != DEFAULT_EEPROM_CHECK) {
        g_sEEPROMData = g_sDefaultEEPROMData;
        EEPROMProgram((uint32_t *)&g_sEEPROMData, DEFAULT_EEPROM_ADDRESS, sizeof(tEEPROM_Data));

        g_sEEPROMDIOCfgData = g_sDefaultEEPROMDIOCfgData;
        EEPROMProgram((uint32_t *)&g_sEEPROMDIOCfgData, DEFAULT_EEPROM_DIO_CONFG, sizeof(tEEPROM_DIOCfgData));

        g_sEEPROMMACCfgData = g_sDefaultEEPROMMACConfigData;
        EEPROMProgram((uint32_t *)&g_sEEPROMMACCfgData, DEFAULT_EEPROM_MAC_CONFG, sizeof(tEEPROM_macConfigData));

        g_sEEPROMIpCfgData = g_sDefaultEEPROMIPConfigData;
        EEPROMProgram((uint32_t *)&g_sEEPROMIpCfgData, DEFAULT_EEPROM_IP_CONFG, sizeof(tEEPROM_ipConfigData));

        g_sEEPROMDebugCfgData = g_sDefaultEEPROMDebugConfigData;
        EEPROMProgram((uint32_t *)&g_sEEPROMDebugCfgData, DEFAULT_EEPROM_DEBUG_CONFG_ADDRESS, sizeof(tEEPROM_debugCfgData));
    }

    g_sEEPROMIpCfgData.Domain[EEPROM_CFG_DOMAIN_MAX - 1] = 0;
}

