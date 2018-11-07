/*
 * SystemAnalogMonitor.c
 *
 *  Created on: Nov 5, 2018
 *      Author: epenate
 */

#define __SYSTEM_SYSTEMANALOGMONITOR_GLOBAL
#include "includes.h"

#define MON_INT_TEMP        ADC_CTL_TS
#define MON_28V             ADC_CTL_CH2
#define MON_LOGICCur        ADC_CTL_CH7
#define MON_PERCur          ADC_CTL_CH6
#define MON_5V_Main         ADC_CTL_CH22
#define MON_5V_Per          ADC_CTL_CH23

#define ADCBUFFERSIZE    (60)

uint16_t SYSAMON_sampleBufferOne[ADCBUFFERSIZE];
uint16_t SYSAMON_sampleBufferTwo[ADCBUFFERSIZE];
uint16_t SYSAMON_sampleBuffer[ADCBUFFERSIZE];
float CBuffer[ADCBUFFERSIZE];


float avgTemperature;
float avgTemp;



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


//    avgTemperature = 0;
//    memcpy(SYSAMON_sampleBuffer, completedADCBuffer, ADCBUFFERSIZE);
    /* Calculate average temperature */

    for (i = 0; i < ADCBUFFERSIZE; i++) {
        SYSAMON_sampleBuffer[i] = rawTemperatureBuf[i];
//        if (!(i % Board_ADCBUF0CHANNELCOUNT)) {
//            avgTemperature += rawTemperatureBuf[i];
//        }
//        avgTemperature += rawTemperatureBuf[i];
        avgTemp = (rawTemperatureBuf[i] + 3 * avgTemp) / 4;
    }
//    avgTemperature = avgTemperature/ADCBUFFERSIZE;
//    avgTemperature = avgTemperature/(ADCBUFFERSIZE / Board_ADCBUF0CHANNELCOUNT);
    avgTemperature = (1475*4096 - (75 * 33 * avgTemp))/ 40960;
    /* Convert ADC value to Celsius */
//    avgTemperature = (1475*4096 - (75 * 33 * avgTemperature))/ 40960;
//    avgTemperature = 36.3 * (avgTemperature / 4096);
//    avgTemperature = (660 * avgTemperature) / 4096; // (3.3 * avgTemperature) / 4096 / 50 * 10
//    avgTemperature = (330 * avgTemperature) / 4096; // (3.3 * avgTemperature) / 4096 / 100 * 10

    /* post adcbuf semaphore */
    sem_post(&adcbufSem);
}


void test2()
{
    ADCBuf_Params adcBufParams;
    /* Driver handles shared between the task and the callback function */
    ADCBuf_Handle adcBuf;
//    ADCBuf_Conversion continuousConversion[Board_ADCBUF0CHANNELCOUNT];
    ADCBuf_Conversion continuousConversion;

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
    adcBufParams.samplingFrequency = 100000;

    adcBuf = ADCBuf_open(Board_ADCBUF0, &adcBufParams);

    if (!adcBuf){
        /* AdcBuf did not open correctly. */
        while(1);
    }

//    /* Configure the conversion struct */
//    continuousConversion[0].arg = NULL;
//    continuousConversion[0].adcChannel = Board_ADCBUF0CHANNEL0;
//    continuousConversion[0].sampleBuffer = SYSAMON_sampleBufferOne;
//    continuousConversion[0].sampleBufferTwo = SYSAMON_sampleBufferTwo;
//    continuousConversion[0].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversion[1].arg = NULL;
//    continuousConversion[1].adcChannel = Board_ADCBUF0CHANNEL1;
//    continuousConversion[1].sampleBuffer = SYSAMON_sampleBufferOne;
//    continuousConversion[1].sampleBufferTwo = SYSAMON_sampleBufferTwo;
//    continuousConversion[1].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversion[2].arg = NULL;
//    continuousConversion[2].adcChannel = Board_ADCBUF0CHANNEL2;
//    continuousConversion[2].sampleBuffer = SYSAMON_sampleBufferOne;
//    continuousConversion[2].sampleBufferTwo = NULL;
//    continuousConversion[2].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversion[3].arg = NULL;
//    continuousConversion[3].adcChannel = Board_ADCBUF0CHANNEL3;
//    continuousConversion[3].sampleBuffer = SYSAMON_sampleBufferOne;
//    continuousConversion[3].sampleBufferTwo = SYSAMON_sampleBufferTwo;
//    continuousConversion[3].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversion[4].arg = NULL;
//    continuousConversion[4].adcChannel = Board_ADCBUF0CHANNEL4;
//    continuousConversion[4].sampleBuffer = SYSAMON_sampleBufferOne;
//    continuousConversion[4].sampleBufferTwo = SYSAMON_sampleBufferTwo;
//    continuousConversion[4].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversion[5].arg = NULL;
//    continuousConversion[5].adcChannel = Board_ADCBUF0CHANNEL5;
//    continuousConversion[5].sampleBuffer = SYSAMON_sampleBufferOne;
//    continuousConversion[5].sampleBufferTwo = SYSAMON_sampleBufferTwo;
//    continuousConversion[5].samplesRequestedCount = ADCBUFFERSIZE;

    /* Configure the conversion struct */
    continuousConversion.arg = NULL;
    continuousConversion.adcChannel = Board_ADCBUF0CHANNEL0;
    continuousConversion.sampleBuffer = SYSAMON_sampleBufferOne;
    continuousConversion.sampleBufferTwo = SYSAMON_sampleBufferTwo;
    continuousConversion.samplesRequestedCount = ADCBUFFERSIZE;

    /*
     * Go to sleep in the foreground thread forever. The data will be collected
     * and transfered in the background thread
     */
    while(1) {
        sleep(1);
        continuousConversion.adcChannel = Board_ADCBUF0CHANNEL0;
        /* Start converting. */
//        if (ADCBuf_convert(adcBuf, &continuousConversion[0], Board_ADCBUF0CHANNELCOUNT) != ADCBuf_STATUS_SUCCESS) {
        if (ADCBuf_convert(adcBuf, &continuousConversion, 1) == ADCBuf_STATUS_SUCCESS) {
            /* Did not start conversion process correctly. */
//            while(1);
            /* Wait for semaphore and print average temperature */
            sem_wait(&adcbufSem);
            /* Print a message with average temperature */
            Display_printf(g_SMCDisplay, 0, 0, "The average temperature is %.3fC", avgTemperature);
        }else{
            Display_printf(g_SMCDisplay, 0, 0, "ADCBuf_convert failed");
        }
    }
}


static Semaphore_Handle semADC0SS0;
static Semaphore_Handle semADC0SS1;

Void HIB_ADC0SS1hwiFunc(UArg a0)
{
//    uint32_t ui32Status;
//    ui32Status = ui32Status;
//    ui32Status = ADCIntStatus(ADC0_BASE, 1, true);
    ADCIntClear(ADC0_BASE, 1);
    Semaphore_post(semADC0SS1);
}

Void HIB_ADC0SS0hwiFunc(UArg a0)
{
//    uint32_t ui32Status;
//    ui32Status = ui32Status;
//    ui32Status = ADCIntStatus(ADC0_BASE, 2, true);
    ADCIntClear(ADC0_BASE, 0);
    Semaphore_post(semADC0SS0);
}

void test3()
{
    Hwi_Handle hwi0;
    Hwi_Params hwiParams;
    Semaphore_Params semParams;
    Error_Block eb;
    uint32_t getADCValue[8];


    Error_init(&eb);
    Hwi_Params_init(&hwiParams);
    hwiParams.arg = 2;
    hwi0 = Hwi_create(INT_ADC0SS0, HIB_ADC0SS0hwiFunc, &hwiParams, &eb);
    if (hwi0 == NULL) {
        System_abort("Hwi create failed");
    }
    hwiParams.arg = 1;
    hwi0 = Hwi_create(INT_ADC0SS1, HIB_ADC0SS1hwiFunc, &hwiParams, &eb);
    if (hwi0 == NULL) {
        System_abort("Hwi create failed");
    }

    /* RTOS primitives */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    semADC0SS0 = Semaphore_create(0, &semParams, &eb);
    if (semADC0SS0 == NULL) {
        System_abort("Can't create ADC0SS2 semaphore");
    }
    semADC0SS1 = Semaphore_create(0, &semParams, &eb);
    if (semADC0SS1 == NULL) {
        System_abort("Can't create ADC0SS1 semaphore");
    }


    /* Configure PE0-PE3 as ADC input channel */
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_5);
    MAP_GPIOPinTypeADC(GPIO_PORTP_BASE, GPIO_PIN_6);
    MAP_GPIOPinTypeADC(GPIO_PORTP_BASE, GPIO_PIN_7);
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);

    /* Enable the clock to ADC-0 and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)))
    {
    }

    /* Configure Sequencer 2 to sample the analog channel : AIN0-AIN3. The
     * end of conversion and interrupt generation is set for AIN3 */
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, MON_PERCur);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 1, MON_28V);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 2, MON_LOGICCur);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 3, MON_5V_Per);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 4, MON_5V_Main | ADC_CTL_IE | ADC_CTL_END);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);

    /* Enable sample sequence 2 with a processor signal trigger.  Sequencer 2
     * will do a single sample when the processor sends a signal to start the
     * conversion */
    MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//    MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    /* Since sample sequence 2 is now configured, it must be enabled. */
    MAP_ADCSequenceEnable(ADC0_BASE, 0);

    /* Clear the interrupt status flag.  This is done to make sure the
     * interrupt flag is cleared before we sample. */
    MAP_ADCIntClear(ADC0_BASE, 0);
    MAP_ADCIntEnable(ADC0_BASE, 0);
//    ADCIntClear(ADC0_BASE, 1);
//    ADCIntEnable(ADC0_BASE, 1);

    /* Sample AIN0 forever.  Display the value on the console. */
    while(1)
    {
        /* Clear the ADC interrupt flag. */
        MAP_ADCIntClear(ADC0_BASE, 0);
        /* Trigger the ADC conversion. */
        MAP_ADCProcessorTrigger(ADC0_BASE, 0);

//        /* Wait for conversion to be completed. */
//        while(!MAP_ADCIntStatus(ADC0_BASE, 2, false))
//        {
//        }

        Semaphore_pend(semADC0SS0, BIOS_WAIT_FOREVER);


        /* Read ADC Value. */
        MAP_ADCSequenceDataGet(ADC0_BASE, 0, getADCValue);

        /* Display the AIN0-AIN03 (PE3-PE0) digital value on the console. */
        Display_printf(g_SMCDisplay, 0, 0, "AIN0-3 = %4d %4d %4d %4d %4d\r", getADCValue[0], getADCValue[1], getADCValue[2], getADCValue[3], getADCValue[4]);

        sleep(1);
    }
}



