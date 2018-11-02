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


/* Example/Board Header files */

void vDiscreteIO_init();
void vHeartBeat_init();
void test();
void test2();

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
    ADCBuf_init();
    GPIO_init();
    PWM_init();
    // I2C_init();
    // SDSPI_init();
    // SPI_init();
    UART_init();
    // Watchdog_init();

    SMCDisplay_init();
    GPIO_write(SMC_SERIAL0_DE, 1);
    Display_printf(g_SMCDisplay, 0, 0, "Starting the System Master Controller\n"
                   "-- Compiled: "__DATE__" "__TIME__" --\n");

    vDiscreteIO_init();
    vHeartBeat_init();

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

    test2();
    return 0;
}


void vDiscreteIO_init()
{
    DIO_init();
    Display_printf(g_SMCDisplay, 0, 0, "Configuring Discrete IO pins\n");
    DICfgMode(DIO_5V_OUT_STAT, DI_MODE_INV);
    DICfgMode(DIO_INTERNAL_5V_SW_STATUS, DI_MODE_INV);
    DICfgMode(DIO_IRDA_RX, DI_MODE_DIRECT);
    DICfgMode(DIO_TEMP_ALERT, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_0, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_1, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_2, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_3, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_4, DI_MODE_DIRECT);
    DICfgMode(DIO_GPI_5, DI_MODE_DIRECT);

    DOCfgMode (DIO_5V_OUT_EN, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_PWR_PERIPHERAL_EN, DO_MODE_DIRECT, true);


    DOCfgMode (DIO_UART_DEBUG, DO_MODE_DIRECT, true);
    DOCfgMode (DIO_SERIAL5_EN_, DO_MODE_DIRECT, true);
    DOCfgMode (DIO_LED_D6, DO_MODE_DIRECT, true);
    DOCfgMode (DIO_LED_D20, DO_MODE_DIRECT, true);
    DOCfgMode (DIO_USB0EPEN, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_0, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_1, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_2, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_3, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_4, DO_MODE_DIRECT, false);
    DOCfgMode (DIO_GPO_5, DO_MODE_DIRECT, false);
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


#define ADCBUFFERSIZE    (100)

uint16_t sampleBufferOne[ADCBUFFERSIZE];
float CBuffer[ADCBUFFERSIZE];
float avgTemperature;

/* Driver handles shared between the task and the callback function */
ADCBuf_Handle adcBuf;
ADCBuf_Conversion continuousConversion[2];


/* ADCBuf semaphore */
sem_t adcbufSem;


/*
 * This function is called whenever a buffer is full.
 * The content of the buffer is then averaged and converted into degrees Celsius format
 * and sent to the PC via the UART.
 */
void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
    void *completedADCBuffer, uint32_t completedChannel) {
    uint_fast16_t i;
    uint16_t *rawTemperatureBuf = (uint16_t *) completedADCBuffer;

    avgTemperature = 0;

    /* Calculate average temperature */
    for (i = 0; i < ADCBUFFERSIZE; i++) {
        avgTemperature += rawTemperatureBuf[i];
    }
    avgTemperature = avgTemperature/ADCBUFFERSIZE;

    /* Convert ADC value to Celsius */
//    avgTemperature = (1475*4096 - (75 * 33 * avgTemperature))/ 40960;
//    avgTemperature = 36.3 * (avgTemperature / 4096);
//    avgTemperature = (660 * avgTemperature) / 4096; // (3.3 * avgTemperature) / 4096 / 50 * 10
    avgTemperature = (330 * avgTemperature) / 4096; // (3.3 * avgTemperature) / 4096 / 100 * 10

    /* post adcbuf semaphore */
    sem_post(&adcbufSem);
}


void test2()
{
    ADCBuf_Params adcBufParams;

    /* Call driver init functions */
    ADCBuf_init();
    GPIO_init();
    Display_init();
    int32_t         status;


    status = sem_init(&adcbufSem, 0, 0);
    if (status != 0) {
        Display_printf(g_SMCDisplay, 0, 0, "Error creating adcbufSem\n");
        while(1);
    }

    Display_printf(g_SMCDisplay, 0, 0, "Starting the ADCBuf temperature example");

    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
    ADCBuf_Params_init(&adcBufParams);
    adcBufParams.callbackFxn = adcBufCallback;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    adcBufParams.samplingFrequency = 1000;

    adcBuf = ADCBuf_open(Board_ADCBUF0, &adcBufParams);

    if (!adcBuf){
        /* AdcBuf did not open correctly. */
        while(1);
    }

    /* Configure the conversion struct */
    continuousConversion[0].arg = NULL;
    continuousConversion[0].adcChannel = Board_ADCBUF0CHANNEL0;
    continuousConversion[0].sampleBuffer = sampleBufferOne;
    continuousConversion[0].sampleBufferTwo = sampleBufferOne;
    continuousConversion[0].samplesRequestedCount = ADCBUFFERSIZE;

    /* Configure the conversion struct */
    continuousConversion[1].arg = NULL;
    continuousConversion[1].adcChannel = Board_ADCBUF0CHANNEL1;
    continuousConversion[1].sampleBuffer = sampleBufferOne;
    continuousConversion[1].sampleBufferTwo = sampleBufferOne;
    continuousConversion[1].samplesRequestedCount = ADCBUFFERSIZE;

    /*
     * Go to sleep in the foreground thread forever. The data will be collected
     * and transfered in the background thread
     */
    while(1) {
        sleep(1);
        /* Start converting. */
        if (ADCBuf_convert(adcBuf, &continuousConversion[1], 1) !=
            ADCBuf_STATUS_SUCCESS) {
            /* Did not start conversion process correctly. */
            while(1);
        }
        /* Wait for semaphore and print average temperature */
        sem_wait(&adcbufSem);
        /* Print a message with average temperature */
        Display_printf(g_SMCDisplay, 0, 0, "The average temperature is %.3fC",
                       avgTemperature);
    }
}
