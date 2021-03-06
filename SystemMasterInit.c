/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== pwmled1.c ========
 */

#include "includes.h"


extern void ti_ndk_config_Global_startupFxn();
void vDiscreteIO_init();
void vHeartBeat_init();
void test();
void vTest_CRC();
void vTest_NVS();
void USBSerialTest_init();
void vRead_NVSSteveConfigParams();

extern void pvPPPCU_inputLineIntHandler(uint_least8_t index);
//extern void test2();
//extern void test3();
extern void *EFS_mainThread(void *arg0);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
    System_printf("Assertion violation: file %s, line %d\n", pcFilename, ui32Line);
    System_flush();
}
#endif


/*
 *  ======== mainThread ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
void *mainThread(void *arg0)
{
    Error_Block eb;
    Device_Params deviceParams;

    /* Call driver init functions */
//    ADCBuf_init();
    GPIO_init();
    PWM_init();
    // I2C_init();
    // SDSPI_init();
//    SPI_init();
    UART_init();
    // Watchdog_init();

    SMCDisplay_init();
    GPIO_write(SMC_SERIAL0_DE, 1);
    Display_printf(g_SMCDisplay, 0, 0, "Starting the System Master Controller\n"
                   "-- Compiled: "__DATE__" "__TIME__" --\n");

    EFS_mainThread(NULL);

    vDiscreteIO_init();
    vHeartBeat_init();
    vI2CTemp101_init();

    /*
     * Initialize interfaces
     */
    vIF_init();
    /*
     * Initialize Devices
     */
    vDevice_init();

    Error_init(&eb);

    vALTOMultinetDevice_Params_init(&deviceParams, 32);
    xDevice_add(&deviceParams, &eb);

    vALTOAmpDevice_Params_init(&deviceParams, 33);
    deviceParams.arg0 = (void *)IF_SERIAL_3;
    xDevice_add(&deviceParams, &eb);

    vALTOAmpDevice_Params_init(&deviceParams, 34);
    deviceParams.arg0 = (void *)IF_SERIAL_4;
    xDevice_add(&deviceParams, &eb);


//    CANTTest_init();

    vForteManagerDevice_Params_init(&deviceParams, 35, IF_SERIAL_0);
//    xDevice_add(&deviceParams, &eb);



//    test3();
    return 0;
}



void *SMC_initThread(void *arg0)
{
    Error_Block eb;
    tEEPROM_debugCfgData *psDebugCfgData;
    Watchdog_init();

    vWDG_init();
    /*
     * SMC_initThread runs with priority 11
     * Watchdog task runs with priority 10
     * Need 1 tick sleep so the Watchdog task can run and setup the watchdog before initialize the system.
     * If the system hangs at any point the watchdog will be triggered
     */
    Task_sleep((unsigned int)1);



    /* Call driver init functions */
    ADCBuf_init();
    GPIO_init();
    PWM_init();
    I2C_init();
    // SDSPI_init();
    SPI_init();
    UART_init();
    CAN_init();
    CRC_init();
//    NVS_init();

    MSP_EXP432E401Y_initUSB(MSP_EXP432E401Y_USBDEVICE);


//#if !(defined(TEST_FIXTURE) || defined(DUT))
    psDebugCfgData = (tEEPROM_debugCfgData *)psEEPDebugConfg_get();
    if (psDebugCfgData->sysLogDisplay) SMCDisplay_init();
//#endif
    GPIO_write(SMC_SERIAL0_DE, 1);
    Display_printf(g_SMCDisplay, 0, 0, "Starting the System Master Controller\n"
                   "-- Compiled: "__DATE__" "__TIME__" --\n");

    Display_printf(g_SMCDisplay, 0, 0, "Initializing vWDG_taskFxn\n");

    USBComposite_init(MSP_EXP432E401Y_USBDEVICE);

    vDiscreteIO_init();
    vHeartBeat_init();
    vI2CTemp101_init();
    vSAM_init();
    vSFFS_init();

    //disconnect Diode D20 and SMC_GPI_0
//    vPPPCU_init();


//    Types_FreqHz freq1; /* Timestamp frequency */
//    Types_FreqHz freq2; /* BIOS frequency */
//    Float factor; /* Clock ratio cpu/timestamp */
//    Timestamp_getFreq(&freq1);
//    BIOS_getCpuFreq(&freq2);
//    factor = (Float)freq2.lo / freq1.lo;

    /*
     * Initialize interfaces
     */
    vIF_init();
    /*
     * Initialize Devices
     */
    vDevice_init();


    SFFS_Handle hSFFS;
//    IFS_deviceInfoFile_t devInfo;

//    int retVal;
//    char buffer[32] = "Hello World!";
//    char buffer2[32];


//    devInfo.params.arg0 = NULL;
//    devInfo.params.arg1 = NULL;


    hSFFS = hSFFS_open(SFFS_Internal);

//    retVal = retVal;

    xSFFS_readDevicesConfiguration(hSFFS, BIOS_WAIT_FOREVER);

    vSFFS_close(hSFFS);


    vSPIDAC101_init(25e3, 25e3);
    vSPIDAC_setRawValue(0);
    vMAX1301_init();


//    Ptr Task_getEnv( Task_Handle handle );
//    Void Task_setEnv( Task_Handle handle, Ptr env );


//    vTest_CRC();
//    vTest_NVS();
#if defined(TEST_FIXTURE) || defined(DUT)
    vCANTest_init();
#endif

    DOSet(DIO_UART_DEBUG, 0);
    DOSet(DIO_SERIAL5_EN_, 1);

    Error_init(&eb);
    Device_Params deviceParams;

    vUSBRCBinaryDevice_Params_init(&deviceParams, USBRCBINDEVICE_ID);
    xDevice_add(&deviceParams, &eb);

    vUSBConsoleDevice_Params_init(&deviceParams, USBCONSOLEDEVICE_ID);
    xDevice_add(&deviceParams, &eb);

    USBSerialTest_init();

#ifdef TEST_FIXTURE
    /* Test Fixture */
    vTFUartTestDevice_Params_init(&deviceParams, 100, IF_SERIAL_0);
    deviceParams.timeout = 100;
    xDevice_add(&deviceParams, &eb);
    vTFUartTestDevice_Params_init(&deviceParams, 101, IF_SERIAL_1);
    deviceParams.timeout = 110;
    xDevice_add(&deviceParams, &eb);
    vTFUartTestDevice_Params_init(&deviceParams, 102, IF_SERIAL_2);
    deviceParams.timeout = 130;
    xDevice_add(&deviceParams, &eb);
    vTFUartTestDevice_Params_init(&deviceParams, 103, IF_SERIAL_3);
    deviceParams.timeout = 140;
    xDevice_add(&deviceParams, &eb);
    vTFUartTestDevice_Params_init(&deviceParams, 104, IF_SERIAL_4);
    deviceParams.timeout = 160;
    xDevice_add(&deviceParams, &eb);
    vTFUartTestDevice_Params_init(&deviceParams, 105, IF_SERIAL_5);
    deviceParams.timeout = 170;
    xDevice_add(&deviceParams, &eb);
    vTFUartTestDevice_Params_init(&deviceParams, 106, IF_SERIAL_6);
    deviceParams.timeout = 180;
    xDevice_add(&deviceParams, &eb);
#endif

#ifdef DUT
    /* Device under test */
    vDUTUartTestDevice_Params_init(&deviceParams, 200, IF_SERIAL_0);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 100;
    xDevice_add(&deviceParams, &eb);
    vDUTUartTestDevice_Params_init(&deviceParams, 201, IF_SERIAL_1);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 110;
    xDevice_add(&deviceParams, &eb);
    vDUTUartTestDevice_Params_init(&deviceParams, 202, IF_SERIAL_2);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 120;
    xDevice_add(&deviceParams, &eb);
    vDUTUartTestDevice_Params_init(&deviceParams, 203, IF_SERIAL_3);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 130;
    xDevice_add(&deviceParams, &eb);
    vDUTUartTestDevice_Params_init(&deviceParams, 204, IF_SERIAL_4);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 140;
    xDevice_add(&deviceParams, &eb);
    vDUTUartTestDevice_Params_init(&deviceParams, 205, IF_SERIAL_5);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 150;
    xDevice_add(&deviceParams, &eb);
    vDUTUartTestDevice_Params_init(&deviceParams, 206, IF_SERIAL_6);
//    deviceParams.period = 1000;
//    deviceParams.timeout = 160;
    xDevice_add(&deviceParams, &eb);

#endif

    ti_ndk_config_Global_startupFxn();

    vRead_NVSSteveConfigParams();

    return 0;
}



void vDiscreteIO_init()
{
    DIO_init();
    Display_printf(g_SMCDisplay, 0, 0, "Configuring Discrete IO pins\n");

    DICfgMode(DIO_5V_OUT_STAT, g_sEEPROMDIOCfgData.di5VOutStatus.DIModeSel);
    DICfgMode(DIO_INTERNAL_5V_SW_STATUS, g_sEEPROMDIOCfgData.diInternal5VSwStatus.DIModeSel);
    DICfgMode(DIO_IRDA_RX, g_sEEPROMDIOCfgData.diIRDARx.DIModeSel);
    DICfgMode(DIO_TEMP_ALERT, g_sEEPROMDIOCfgData.diTempAlert.DIModeSel);

    DICfgDebounce   (DIO_GPI_0, g_sEEPROMDIOCfgData.diConfig[0].DIDebounceDly, g_sEEPROMDIOCfgData.diConfig[0].DIRptStartDly, g_sEEPROMDIOCfgData.diConfig[0].DIRptDly);
    DICfgDebounce   (DIO_GPI_1, g_sEEPROMDIOCfgData.diConfig[1].DIDebounceDly, g_sEEPROMDIOCfgData.diConfig[1].DIRptStartDly, g_sEEPROMDIOCfgData.diConfig[1].DIRptDly);
    DICfgDebounce   (DIO_GPI_2, g_sEEPROMDIOCfgData.diConfig[2].DIDebounceDly, g_sEEPROMDIOCfgData.diConfig[2].DIRptStartDly, g_sEEPROMDIOCfgData.diConfig[2].DIRptDly);
    DICfgDebounce   (DIO_GPI_3, g_sEEPROMDIOCfgData.diConfig[3].DIDebounceDly, g_sEEPROMDIOCfgData.diConfig[3].DIRptStartDly, g_sEEPROMDIOCfgData.diConfig[3].DIRptDly);
    DICfgDebounce   (DIO_GPI_4, g_sEEPROMDIOCfgData.diConfig[4].DIDebounceDly, g_sEEPROMDIOCfgData.diConfig[4].DIRptStartDly, g_sEEPROMDIOCfgData.diConfig[4].DIRptDly);
    DICfgDebounce   (DIO_GPI_5, g_sEEPROMDIOCfgData.diConfig[5].DIDebounceDly, g_sEEPROMDIOCfgData.diConfig[5].DIRptStartDly, g_sEEPROMDIOCfgData.diConfig[5].DIRptDly);

    DISetDebounceEn (DIO_GPI_0, g_sEEPROMDIOCfgData.diConfig[0].DIDebounceEn);
    DISetDebounceEn (DIO_GPI_1, g_sEEPROMDIOCfgData.diConfig[1].DIDebounceEn);
    DISetDebounceEn (DIO_GPI_2, g_sEEPROMDIOCfgData.diConfig[2].DIDebounceEn);
    DISetDebounceEn (DIO_GPI_3, g_sEEPROMDIOCfgData.diConfig[3].DIDebounceEn);
    DISetDebounceEn (DIO_GPI_4, g_sEEPROMDIOCfgData.diConfig[4].DIDebounceEn);
    DISetDebounceEn (DIO_GPI_5, g_sEEPROMDIOCfgData.diConfig[5].DIDebounceEn);

    DICfgMode(DIO_GPI_0, g_sEEPROMDIOCfgData.diConfig[0].DIModeSel);
    DICfgMode(DIO_GPI_1, g_sEEPROMDIOCfgData.diConfig[1].DIModeSel);
    DICfgMode(DIO_GPI_2, g_sEEPROMDIOCfgData.diConfig[2].DIModeSel);
    DICfgMode(DIO_GPI_3, g_sEEPROMDIOCfgData.diConfig[3].DIModeSel);
    DICfgMode(DIO_GPI_4, g_sEEPROMDIOCfgData.diConfig[4].DIModeSel);
    DICfgMode(DIO_GPI_5, g_sEEPROMDIOCfgData.diConfig[5].DIModeSel);


    DOSetSyncCtrMax(g_sEEPROMDIOCfgData.doSyncCtrMax);

    DOCfgMode (DIO_5V_OUT_EN, g_sEEPROMDIOCfgData.do5VOutEn.DOModeSel, g_sEEPROMDIOCfgData.do5VOutEn.DOInv);
    DOCfgMode (DIO_PWR_PERIPHERAL_EN, g_sEEPROMDIOCfgData.doPWRPeripheralEn.DOModeSel, g_sEEPROMDIOCfgData.doPWRPeripheralEn.DOInv);


    DOCfgMode (DIO_UART_DEBUG, g_sEEPROMDIOCfgData.doUARTDebug.DOModeSel, g_sEEPROMDIOCfgData.doUARTDebug.DOInv);
    DOCfgMode (DIO_SERIAL5_EN_, g_sEEPROMDIOCfgData.doSerial5En.DOModeSel, g_sEEPROMDIOCfgData.doSerial5En.DOInv);

//    Display_printf(g_SMCDisplay, 0, 0, "DIO_LED_D6 mode: %d, inv: %d\n", g_sEEPROMDIOCfgData.doLEDD6.DOModeSel, g_sEEPROMDIOCfgData.doLEDD6.DOInv);
    Display_printf(g_SMCDisplay, 0, 0, "Configuring Discrete IO pins\n");
    DOCfgMode (DIO_LED_D6, g_sEEPROMDIOCfgData.doLEDD6.DOModeSel, g_sEEPROMDIOCfgData.doLEDD6.DOInv);
    DOCfgMode (DIO_LED_D20, g_sEEPROMDIOCfgData.doLEDD20.DOModeSel, g_sEEPROMDIOCfgData.doLEDD20.DOInv);
    DOCfgMode (DIO_USB0EPEN, g_sEEPROMDIOCfgData.doUSB0EPEn.DOModeSel, g_sEEPROMDIOCfgData.doUSB0EPEn.DOInv);

    DOCfgBlink (DIO_GPO_0, g_sEEPROMDIOCfgData.doConfig[0].DOBlinkEnSel, g_sEEPROMDIOCfgData.doConfig[0].DOA, g_sEEPROMDIOCfgData.doConfig[0].DOB);
    DOCfgBlink (DIO_GPO_1, g_sEEPROMDIOCfgData.doConfig[1].DOBlinkEnSel, g_sEEPROMDIOCfgData.doConfig[1].DOA, g_sEEPROMDIOCfgData.doConfig[1].DOB);
    DOCfgBlink (DIO_GPO_2, g_sEEPROMDIOCfgData.doConfig[2].DOBlinkEnSel, g_sEEPROMDIOCfgData.doConfig[2].DOA, g_sEEPROMDIOCfgData.doConfig[2].DOB);
    DOCfgBlink (DIO_GPO_3, g_sEEPROMDIOCfgData.doConfig[3].DOBlinkEnSel, g_sEEPROMDIOCfgData.doConfig[3].DOA, g_sEEPROMDIOCfgData.doConfig[3].DOB);
    DOCfgBlink (DIO_GPO_4, g_sEEPROMDIOCfgData.doConfig[4].DOBlinkEnSel, g_sEEPROMDIOCfgData.doConfig[4].DOA, g_sEEPROMDIOCfgData.doConfig[4].DOB);
    DOCfgBlink (DIO_GPO_5, g_sEEPROMDIOCfgData.doConfig[5].DOBlinkEnSel, g_sEEPROMDIOCfgData.doConfig[5].DOA, g_sEEPROMDIOCfgData.doConfig[5].DOB);

    DOCfgMode (DIO_GPO_0, g_sEEPROMDIOCfgData.doConfig[0].DOModeSel, g_sEEPROMDIOCfgData.doConfig[0].DOInv);
    DOCfgMode (DIO_GPO_1, g_sEEPROMDIOCfgData.doConfig[1].DOModeSel, g_sEEPROMDIOCfgData.doConfig[1].DOInv);
    DOCfgMode (DIO_GPO_2, g_sEEPROMDIOCfgData.doConfig[2].DOModeSel, g_sEEPROMDIOCfgData.doConfig[2].DOInv);
    DOCfgMode (DIO_GPO_3, g_sEEPROMDIOCfgData.doConfig[3].DOModeSel, g_sEEPROMDIOCfgData.doConfig[3].DOInv);
    DOCfgMode (DIO_GPO_4, g_sEEPROMDIOCfgData.doConfig[4].DOModeSel, g_sEEPROMDIOCfgData.doConfig[4].DOInv);
    DOCfgMode (DIO_GPO_5, g_sEEPROMDIOCfgData.doConfig[5].DOModeSel, g_sEEPROMDIOCfgData.doConfig[5].DOInv);



    if (g_sEEPROMDIOCfgData.dioCfg[0] & GPIO_CFG_INPUT) {
        GPIO_setConfig(Board_GPI_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    }else{
        GPIO_setConfig(Board_GPI_0, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    }

    if (g_sEEPROMDIOCfgData.dioCfg[1] & GPIO_CFG_INPUT) {
        GPIO_setConfig(Board_GPI_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    }else{
        GPIO_setConfig(Board_GPI_1, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    }

    if (g_sEEPROMDIOCfgData.dioCfg[2] & GPIO_CFG_INPUT) {
        GPIO_setConfig(Board_GPI_2, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    }else{
        GPIO_setConfig(Board_GPI_2, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    }

    if (g_sEEPROMDIOCfgData.dioCfg[3] & GPIO_CFG_INPUT) {
        GPIO_setConfig(Board_GPI_3, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    }else{
        GPIO_setConfig(Board_GPI_3, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    }

    if (g_sEEPROMDIOCfgData.dioCfg[4] & GPIO_CFG_INPUT) {
        GPIO_setConfig(Board_GPI_4, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    }else{
        GPIO_setConfig(Board_GPI_4, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    }

    if (g_sEEPROMDIOCfgData.dioCfg[5] & GPIO_CFG_INPUT) {
        GPIO_setConfig(Board_GPI_5, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    }else{
        GPIO_setConfig(Board_GPI_5, GPIO_CFG_OUT_OD_PU | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    }


    DOCfgMode (DIO_IRDA_TX, DO_MODE_DIRECT, false);

    Display_printf(g_SMCDisplay, 0, 0, "Setting 5V Out to ON\n");
    DOSet(DIO_5V_OUT_EN, DO_ON);
    Display_printf(g_SMCDisplay, 0, 0, "Setting Power Peripheral to ON\n");
    DOSet(DIO_PWR_PERIPHERAL_EN, DO_ON);
    Display_printf(g_SMCDisplay, 0, 0, "Setting UART Debug ON\n");
    DOSet(DIO_UART_DEBUG, DO_ON);

}

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    Display_printf(g_SMCDisplay, 0, 0, "Heartbeat task started\n");
    while (1) {
//        GPIO_write(SMC_BLE_2OE, 1);
//        GPIO_write(Board_LED3,0);
        DOSet(DIO_LED_D6, DO_ON);
        Task_sleep((unsigned int)50);
//        GPIO_write(Board_LED3,1);
        DOSet(DIO_LED_D6, DO_OFF);
        Task_sleep((unsigned int)50);
//        GPIO_write(Board_LED3,0);
        DOSet(DIO_LED_D6, DO_ON);
        Task_sleep((unsigned int)50);
//        GPIO_write(Board_LED3,1);
        DOSet(DIO_LED_D6, DO_OFF);
//        GPIO_write(SMC_BLE_2OE, 0);
        Task_sleep((unsigned int)850);

//        Task_sleep((unsigned int)10000);
////        if ( DIGet(DIO_GPI_5) == 0)
//        {
//            JumpToBootLoader();
//        }
    }
}



void vHeartBeat_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing heartbeat\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    taskParams.stackSize = 512;
    Task_create((Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
}




void test()
{
    /* Period and duty in microseconds */
    uint16_t   pwmPeriod = 3000;
    uint16_t   duty = 0;
    uint16_t   dutyInc = 100;

    /* Sleep time in microseconds */
    uint32_t   time = 50000;
    PWM_Handle pwm1 = NULL;
    PWM_Params params;





//    Display_printf(g_SMCDisplay, 0, 0, "SlNetIf_init fail\n");

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;
    pwm1 = PWM_open(Board_PWM0, &params);
    if (pwm1 == NULL) {
        /* Board_PWM0 did not open */
        while (1);
    }

    PWM_start(pwm1);

    /* Loop forever incrementing the PWM duty */
    while (1) {
        PWM_setDuty(pwm1, duty);

        duty = (duty + dutyInc);

        if (duty == pwmPeriod || (!duty)) {
            dutyInc = - dutyInc;
        }

        usleep(time);
    }
}




/* Expected CRC for CRC_32_IEEE with full endianness reversal */
static const uint32_t expectedCrc = 0x4C4B4461;

/* Example test vector */
static const size_t srcSize = 32;
static const uint8_t src [] = {
    0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
    0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
    0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
    0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78
};



void vTest_CRC()
{
    CRC_Handle handle;
    CRC_Params params;
    int_fast16_t status;
    uint32_t result;

    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;
    params.returnBehavior = CRC_RETURN_BEHAVIOR_BLOCKING;
    params.polynomial = CRC_POLYNOMIAL_CRC_32_IEEE;
    params.dataSize = CRC_DATA_SIZE_32BIT;
    params.seed = 0xFFFFFFFF;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        while(1);
    }

    /* Calculate the CRC of all 32 bytes in the source array */
    status = CRC_calculateFull(handle, src, srcSize, &result);
    if (status != CRC_STATUS_SUCCESS || result != expectedCrc)
    {
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        while(1);
    }

    /* This is another way of achieving the same result; for instance
     * if data is arriving in blocks (over UART) this method may be preferable.
     * CRC_reset() may be used to clear an ongoing partial if the result is no
     * longer needed. CRC_finalise() also resets the state. */
    status = CRC_addData(handle, src, srcSize/2);
    if (status != CRC_STATUS_SUCCESS)
    {
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        while(1);
    }

    status = CRC_addData(handle, &src[srcSize/2], srcSize/2);

    /* Extract the result from the internal state */
    CRC_finalize(handle, &result);

    if (status != CRC_STATUS_SUCCESS || result != expectedCrc)
    {
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        while(1);
    }

//    UART_write(uart, postOpMessage, sizeof(postOpMessage));

    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);



    /* Set data processing options, including endianness control */
    CRC_Params_init(&params);
    params.polynomial = CRC_POLYNOMIAL_CRC_16_CCITT;
    params.dataSize = CRC_DATA_SIZE_8BIT;
    params.seed = 0xFFFF;
    params.byteSwapInput = CRC_BYTESWAP_UNCHANGED;
//    params.byteSwapInput = CRC_BYTESWAP_BYTES_AND_HALF_WORDS;

    /* Open the driver using the settings above */
    handle = CRC_open(MSP_EXP432E401Y_CRC0, &params);
    if (handle == NULL)
    {
        /* If the handle is already open, execution will stop here */
        while(1);
    }

    char buff[] = "1234";

    result = 0;
    /* Calculate the CRC of all 32 bytes in the source array */
    status = CRC_calculateFull(handle, buff, 4, &result);
    if (status != CRC_STATUS_SUCCESS)
    {
        /* If the CRC engine is busy or if an error occurs execution will stop here */
        while(1);
    }


    /* Close the driver to allow other users to access this driver instance */
    CRC_close(handle);
}


#define FOOTER "=================================================="
#define NVSSPI25X_CMD_MASS_ERASE        (NVS_CMD_RESERVED + 0)

char g_sectorBuffer[16];

void vTest_NVS()
{
    NVS_Handle nvsHandle;
    NVS_Attrs regionAttrs;
    NVS_Params nvsParams;
    uint32_t ui32Address;
    int_fast16_t i16RetVal;
    bool bisMemOk = true;


    /* Buffer placed in RAM to hold bytes read from non-volatile storage. */
    char buffer[64];

    /*
     * Some devices have a minimum FLASH write size of 4-bytes (1 word). Trying
     * to write a non-multiple of 4 amount of data will fail. This array is
     * rounded up (to next multiple of 4) to meet this requirement. Refer to NVS
     * driver documentation for more details.
     */
    static const char signature[52] =
    {"SimpleLink SDK Non-Volatile Storage (NVS) Example."};


    NVS_Params_init(&nvsParams);
    nvsHandle = NVS_open(Board_NVSEXTERNAL, &nvsParams);

    if (nvsHandle == NULL) {
        Display_printf(g_SMCDisplay, 0, 0, "NVS_open() failed.");
        NVS_close(nvsHandle);
        return;
    }

    Display_printf(g_SMCDisplay, 0, 0, "\n");

    /*
     * This will populate a NVS_Attrs structure with properties specific
     * to a NVS_Handle such as region base address, region size,
     * and sector size.
     */
    NVS_getAttrs(nvsHandle, &regionAttrs);

    /* Display the NVS region attributes */
    Display_printf(g_SMCDisplay, 0, 0, "Region Base Address: 0x%x",
            regionAttrs.regionBase);
    Display_printf(g_SMCDisplay, 0, 0, "Sector Size: 0x%x",
            regionAttrs.sectorSize);
    Display_printf(g_SMCDisplay, 0, 0, "Region Size: 0x%x\n",
            regionAttrs.regionSize);

    NVS_read(nvsHandle, 0, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));
    NVS_read(nvsHandle, 4096, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));
    /* Erase the entire flash sector. */
    NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);
    NVS_erase(nvsHandle, 4096, regionAttrs.sectorSize);
    NVS_control(nvsHandle, NVSSPI25X_CMD_MASS_ERASE, NULL);
    NVS_read(nvsHandle, 0, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));
    NVS_read(nvsHandle, 4096, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));

    ui32Address = 0;
    i16RetVal = NVS_write(nvsHandle, ui32Address, (void *) signature, sizeof(signature),
                         NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
    NVS_read(nvsHandle, ui32Address, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));

    ui32Address += 4096;

    NVS_read(nvsHandle, ui32Address, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));
    i16RetVal = NVS_write(nvsHandle, ui32Address, (void *) signature, sizeof(signature),
//                         NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
                         NVS_WRITE_POST_VERIFY);
    NVS_read(nvsHandle, 0, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));
    NVS_read(nvsHandle, ui32Address, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));

    i16RetVal = NVS_write(nvsHandle, ui32Address, (void *) "Hello", sizeof("Hello"),
                          NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
    NVS_read(nvsHandle, 0, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));
    NVS_read(nvsHandle, ui32Address, (void *) g_sectorBuffer, sizeof(g_sectorBuffer));

//
//    ui32Address = 0;
//    while(ui32Address < (regionAttrs.regionSize - sizeof(signature))) {
//        NVS_read(nvsHandle, ui32Address, (void *) buffer, sizeof(signature));
//        ui32Address += sizeof(signature);
//        if (strcmp((char *) buffer, (char *) signature) != 0) {
//            bisMemOk = false;
//            break;
//        }
//    }
//
//
//    /*
//     * Copy "sizeof(signature)" bytes from the NVS region base address into
//     * buffer. An offset of 0 specifies the offset from region base address.
//     * Therefore, the bytes are copied from regionAttrs.regionBase.
//     */
////    NVS_read(nvsHandle, 0, (void *) buffer, sizeof(signature));
//
//    /*
//     * Determine if the NVS region contains the signature string.
//     * Compare the string with the contents copied into buffer.
//     */
////    if (strcmp((char *) buffer, (char *) signature) == 0) {
//    if (bisMemOk) {
//
//        /* Write signature directly from the NVS region to the UART console. */
//        Display_printf(g_SMCDisplay, 0, 0, "%s", regionAttrs.regionBase);
//        Display_printf(g_SMCDisplay, 0, 0, "Erasing flash sector...");
//
//        /* Erase the entire flash sector. */
//        NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);
//        NVS_control(nvsHandle, NVSSPI25X_CMD_MASS_ERASE, NULL);
//    }
//    else {
//
//        /* The signature was not found in the NVS region. */
//        Display_printf(g_SMCDisplay, 0, 0, "Writing signature to flash...");
//
//        /* Write signature to memory. The flash sector is erased prior
//         * to performing the write operation. This is specified by
//         * NVS_WRITE_ERASE.
//         */
//        NVS_write(nvsHandle, 0, (void *) signature, sizeof(signature),
//            NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
//        ui32Address = 0;
//
//        while(ui32Address < (regionAttrs.regionSize - sizeof(signature))) {
//            i16RetVal = NVS_write(nvsHandle, ui32Address, (void *) signature, sizeof(signature),
//                                 NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
//            if (i16RetVal != NVS_STATUS_SUCCESS) {
//                break;
//            }
//            ui32Address += sizeof(signature);
//        }
//    }

//    Display_printf(g_SMCDisplay, 0, 0, "Reset the device.");
    Display_printf(g_SMCDisplay, 0, 0, FOOTER);
    NVS_close(nvsHandle);
}

int32_t g_i32AircraftID;
int32_t g_i32ConfigRev;

void vRead_NVSSteveConfigParams()
{
    NVS_Handle nvsHandle;
    NVS_Attrs regionAttrs;
    NVS_Params nvsParams;

    tURLHandlerHeader header;


    NVS_Params_init(&nvsParams);
    nvsHandle = NVS_open(Board_NVSEXTERNAL, &nvsParams);

    if (nvsHandle == NULL) {
        Display_printf(g_SMCDisplay, 0, 0, "NVS_open() failed.");
        NVS_close(nvsHandle);
        return;
    }

//    Display_printf(g_SMCDisplay, 0, 0, "\n");
//    Display_printf(g_SMCDisplay, 0, 0, FOOTER);

    /*
     * This will populate a NVS_Attrs structure with properties specific
     * to a NVS_Handle such as region base address, region size,
     * and sector size.
     */
    NVS_getAttrs(nvsHandle, &regionAttrs);

    /* Display the NVS region attributes */
    Display_printf(g_SMCDisplay, 0, 0, "Region Base Address: 0x%x",
            regionAttrs.regionBase);
    Display_printf(g_SMCDisplay, 0, 0, "Sector Size: 0x%x",
            regionAttrs.sectorSize);
    Display_printf(g_SMCDisplay, 0, 0, "Region Size: 0x%x",
            regionAttrs.regionSize);

    NVS_read(nvsHandle, 0, (void *)&header, sizeof(tURLHandlerHeader));
    g_i32AircraftID = header.aircraftID;
    g_i32ConfigRev = header.configRev;

    Display_printf(g_SMCDisplay, 0, 0, "App Config file: AircraftID: %d ConfigRev: %d Size: %d bytes\n", header.aircraftID, header.configRev, header.file_hdr.uncompressed_size);
//    Display_printf(g_SMCDisplay, 0, 0, FOOTER);
    NVS_close(nvsHandle);
}


