/*
 * SystemAnalogMonitor.c
 *
 *  Created on: Nov 5, 2018
 *      Author: epenate
 */

#define __SYSTEM_SYSTEMANALOGMONITOR_GLOBAL
#include "includes.h"

//#define MON_INT_TEMP        ADC_CTL_TS
//#define MON_28V             ADC_CTL_CH2
//#define MON_LOGICCur        ADC_CTL_CH7
//#define MON_PERCur          ADC_CTL_CH6
//#define MON_5V_Main         ADC_CTL_CH22
//#define MON_5V_Per          ADC_CTL_CH23
//
//#define ADCBUFFERSIZE    (60)
//
//uint16_t g_SYSAMON_sampleBufferOne[ADCBUFFERSIZE];
//uint16_t g_SYSAMON_sampleBufferTwo[ADCBUFFERSIZE];
//uint16_t g_SYSAMON_sampleBuffer[ADCBUFFERSIZE];
//uint32_t g_microvoltBuffer[ADCBUFFERSIZE];
//float g_CBuffer[ADCBUFFERSIZE];
//
//
//
//float g_avgTemp;
//
//
//
///* ADCBuf semaphore */
//sem_t g_adcbufSem;
//
//
///*
// * This function is called whenever a buffer is full.
// * The content of the buffer is then averaged and converted into degrees Celsius format
// * and sent to the PC via the UART.
// */
//void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
//    void *completedADCBuffer, uint32_t completedChannel) {
//    uint_fast16_t i;
//    uint16_t *rawTemperatureBuf = (uint16_t *) completedADCBuffer;
//
//
//    switch(completedChannel) {
//    case Board_ADCBUF0CHANNEL1:
//        // Adjust raw ADC values and convert them to microvolts
//        ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE, completedChannel);
//        ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel, completedADCBuffer, g_microvoltBuffer, ADCBUFFERSIZE);
//        for (i = 0; i < ADCBUFFERSIZE; i++) {
//            g_avgTemp = (g_microvoltBuffer[i] + 3 * g_avgTemp) / 4;
//        }
//        g_fSAMavgCPUTemperature = 0.1 * g_avgTemp / 1e3;
//        break;
//    case Board_ADCBUF0CHANNEL2:
//        // Adjust raw ADC values and convert them to microvolts
//        ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE, completedChannel);
//        ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel, completedADCBuffer, g_microvoltBuffer, ADCBUFFERSIZE);
//        for (i = 0; i < ADCBUFFERSIZE; i++) {
//            g_avgTemp = (g_microvoltBuffer[i] + 3 * g_avgTemp) / 4;
//        }
//        g_fSAMavgCPUTemperature = 0.2 * g_avgTemp / 1e3;
//        break;
//    case Board_ADCBUF0CHANNEL3:
//        // Adjust raw ADC values and convert them to microvolts
//        ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE, completedChannel);
//        ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel, completedADCBuffer, g_microvoltBuffer, ADCBUFFERSIZE);
//        for (i = 0; i < ADCBUFFERSIZE; i++) {
//            g_avgTemp = (g_microvoltBuffer[i] + 3 * g_avgTemp) / 4;
//        }
//        g_fSAMavgCPUTemperature = 2 * g_avgTemp / 1e6;
//        break;
//    case Board_ADCBUF0CHANNEL4:
//        // Adjust raw ADC values and convert them to microvolts
//        ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE, completedChannel);
//        ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel, completedADCBuffer, g_microvoltBuffer, ADCBUFFERSIZE);
//        for (i = 0; i < ADCBUFFERSIZE; i++) {
//            g_avgTemp = (g_microvoltBuffer[i] + 3 * g_avgTemp) / 4;
//        }
//        g_fSAMavgCPUTemperature = 2 * g_avgTemp / 1e6;
//        break;
//    case Board_ADCBUF0CHANNEL5:
//        // Adjust raw ADC values and convert them to microvolts
//        ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCBUFFERSIZE, completedChannel);
//        ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel, completedADCBuffer, g_microvoltBuffer, ADCBUFFERSIZE);
//        for (i = 0; i < ADCBUFFERSIZE; i++) {
//            g_avgTemp = (g_microvoltBuffer[i] + 3 * g_avgTemp) / 4;
//        }
//        g_fSAMavgCPUTemperature = 11 * g_avgTemp / 1e6;
//        break;
//    case Board_ADCBUF0CHANNEL0:
//        //    avgTemperature = 0;
//        //    memcpy(SYSAMON_sampleBuffer, completedADCBuffer, ADCBUFFERSIZE);
//        /* Calculate average temperature */
//
//        for (i = 0; i < ADCBUFFERSIZE; i++) {
//            g_SYSAMON_sampleBuffer[i] = rawTemperatureBuf[i];
//            //        if (!(i % Board_ADCBUF0CHANNELCOUNT)) {
//            //            avgTemperature += rawTemperatureBuf[i];
//            //        }
//            //        avgTemperature += rawTemperatureBuf[i];
//            g_avgTemp = (rawTemperatureBuf[i] + 3 * g_avgTemp) / 4;
//        }
////        avgTemperature = avgTemperature/ADCBUFFERSIZE;
//        //    avgTemperature = avgTemperature/(ADCBUFFERSIZE / Board_ADCBUF0CHANNELCOUNT);
//        g_fSAMavgCPUTemperature = (1475*4096 - (75 * 33 * g_avgTemp))/ 40960;
//        /* Convert ADC value to Celsius */
//        //    avgTemperature = (1475*4096 - (75 * 33 * avgTemperature))/ 40960;
//        //    avgTemperature = 36.3 * (avgTemperature / 4096);
//        //    avgTemperature = (660 * avgTemperature) / 4096; // (3.3 * avgTemperature) / 4096 / 50 * 10
//        //    avgTemperature = (330 * avgTemperature) / 4096; // (3.3 * avgTemperature) / 4096 / 100 * 10
//        break;
//    }
//
//    /* post adcbuf semaphore */
//    sem_post(&g_adcbufSem);
//
//}
//
//
//void test2()
//{
//    ADCBuf_Params adcBufParams;
//    /* Driver handles shared between the task and the callback function */
//    ADCBuf_Handle adcBuf;
//    ADCBuf_Conversion continuousConversions[6];
////    ADCBuf_Conversion continuousConversion;
//
//    int32_t         status;
//
//
//    status = sem_init(&g_adcbufSem, 0, 0);
//    if (status != 0) {
//        Display_printf(g_SMCDisplay, 0, 0, "Error creating adcbufSem\n");
//        while(1);
//    }
//
//    Display_printf(g_SMCDisplay, 0, 0, "Starting the ADCBuf temperature example");
//
//    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
//    ADCBuf_Params_init(&adcBufParams);
//    adcBufParams.callbackFxn = adcBufCallback;
////    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
//    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
//    adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
////    adcBufParams.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
//    adcBufParams.samplingFrequency = 10000;
//
//    adcBuf = ADCBuf_open(Board_ADCBUF0, &adcBufParams);
//
//    if (!adcBuf){
//        /* AdcBuf did not open correctly. */
//        while(1);
//    }
//
//    /* Configure the conversion struct */
//    continuousConversions[0].arg = NULL;
//    continuousConversions[0].adcChannel = Board_ADCBUF0CHANNEL0;
//    continuousConversions[0].sampleBuffer = g_SYSAMON_sampleBufferOne;
//    continuousConversions[0].sampleBufferTwo = g_SYSAMON_sampleBufferTwo;
//    continuousConversions[0].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversions[1].arg = NULL;
//    continuousConversions[1].adcChannel = Board_ADCBUF0CHANNEL1;
//    continuousConversions[1].sampleBuffer = NULL;
//    continuousConversions[1].sampleBufferTwo = NULL;
//    continuousConversions[1].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversions[2].arg = NULL;
//    continuousConversions[2].adcChannel = Board_ADCBUF0CHANNEL2;
//    continuousConversions[2].sampleBuffer = NULL;
//    continuousConversions[2].sampleBufferTwo = NULL;
//    continuousConversions[2].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversions[3].arg = NULL;
//    continuousConversions[3].adcChannel = Board_ADCBUF0CHANNEL3;
//    continuousConversions[3].sampleBuffer = NULL;
//    continuousConversions[3].sampleBufferTwo = NULL;
//    continuousConversions[3].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversions[4].arg = NULL;
//    continuousConversions[4].adcChannel = Board_ADCBUF0CHANNEL4;
//    continuousConversions[4].sampleBuffer = NULL;
//    continuousConversions[4].sampleBufferTwo = NULL;
//    continuousConversions[4].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Configure the conversion struct */
//    continuousConversions[5].arg = NULL;
//    continuousConversions[5].adcChannel = Board_ADCBUF0CHANNEL5;
//    continuousConversions[5].sampleBuffer = NULL;
//    continuousConversions[5].sampleBufferTwo = NULL;
//    continuousConversions[5].samplesRequestedCount = ADCBUFFERSIZE;
//
//    /* Start converting sequencer 0. */
//    if (ADCBuf_convert(adcBuf, continuousConversions, 6) != ADCBuf_STATUS_SUCCESS) {
//        /* Did not start conversion process correctly. */
//        while(1);
//    }
//
//    sem_wait(&g_adcbufSem);
//
////    /* Configure the conversion struct */
////    continuousConversion.arg = NULL;
////    continuousConversion.adcChannel = Board_ADCBUF0CHANNEL0;
////    continuousConversion.sampleBuffer = g_SYSAMON_sampleBufferOne;
////    continuousConversion.sampleBufferTwo = g_SYSAMON_sampleBufferTwo;
////    continuousConversion.samplesRequestedCount = ADCBUFFERSIZE;
//
//    /*
//     * Go to sleep in the foreground thread forever. The data will be collected
//     * and transfered in the background thread
//     */
//    while(1) {
////        sleep(1);
//
//
////        continuousConversion.adcChannel = Board_ADCBUF0CHANNEL5;
////        continuousConversions[0].adcChannel = Board_ADCBUF0CHANNEL5;
////        /* Start converting. */
//////        if (ADCBuf_convert(adcBuf, &continuousConversions[0], 1) == ADCBuf_STATUS_SUCCESS) {
////        if (ADCBuf_convert(adcBuf, &continuousConversion, 1) == ADCBuf_STATUS_SUCCESS) {
////            /* Wait for semaphore and print average temperature */
////            sem_wait(&adcbufSem);
////
//////            int i;
//////            ADCBuf_adjustRawValues(adcBuf, SYSAMON_sampleBufferOne, ADCBUFFERSIZE, Board_ADCBUF0CHANNEL5);
//////            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL5, SYSAMON_sampleBufferOne, microvoltBuffer, ADCBUFFERSIZE);
//////            for (i = 0; i < ADCBUFFERSIZE; i++) {
//////                avgTemp = (microvoltBuffer[i] + 3 * avgTemp) / 4;
//////            }
//////            avgTemperature = 11 * avgTemp / 1e6;
////
////            /* Print a message with average temperature */
////            Display_printf(g_SMCDisplay, 0, 0, "The average temperature is %.3fC", avgTemperature);
////        }else{
////            Display_printf(g_SMCDisplay, 0, 0, "ADCBuf_convert failed");
////        }
//
////        /* Start converting sequencer 0. */
////        if (ADCBuf_convert(adcBuf, continuousConversions, 2) != ADCBuf_STATUS_SUCCESS) {
////            /* Did not start conversion process correctly. */
////            Display_printf(g_SMCDisplay, 0, 0, "ADCBuf_convert failed");
////        }else {
//            sem_wait(&g_adcbufSem);
////
////        }
//
//    }
//}
//
//
//static Semaphore_Handle semADC0SS0;
//static Semaphore_Handle semADC0SS1;
//
//Void HIB_ADC0SS1hwiFunc(UArg a0)
//{
////    uint32_t ui32Status;
////    ui32Status = ui32Status;
////    ui32Status = ADCIntStatus(ADC0_BASE, 1, true);
//    ADCIntClear(ADC0_BASE, 1);
//    Semaphore_post(semADC0SS1);
//}
//
//Void HIB_ADC0SS0hwiFunc(UArg a0)
//{
////    uint32_t ui32Status;
////    ui32Status = ui32Status;
////    ui32Status = ADCIntStatus(ADC0_BASE, 2, true);
//    ADCIntClear(ADC0_BASE, 0);
//    Semaphore_post(semADC0SS0);
//}
//
//void test3()
//{
//    Hwi_Handle hwi0;
//    Hwi_Params hwiParams;
//    Semaphore_Params semParams;
//    Error_Block eb;
//    uint32_t getADCValue[8];
//
//
//    Error_init(&eb);
//    Hwi_Params_init(&hwiParams);
//    hwiParams.arg = 2;
//    hwi0 = Hwi_create(INT_ADC0SS0, HIB_ADC0SS0hwiFunc, &hwiParams, &eb);
//    if (hwi0 == NULL) {
//        System_abort("Hwi create failed");
//    }
//    hwiParams.arg = 1;
//    hwi0 = Hwi_create(INT_ADC0SS1, HIB_ADC0SS1hwiFunc, &hwiParams, &eb);
//    if (hwi0 == NULL) {
//        System_abort("Hwi create failed");
//    }
//
//    /* RTOS primitives */
//    Semaphore_Params_init(&semParams);
//    semParams.mode = Semaphore_Mode_BINARY;
//    semADC0SS0 = Semaphore_create(0, &semParams, &eb);
//    if (semADC0SS0 == NULL) {
//        System_abort("Can't create ADC0SS2 semaphore");
//    }
//    semADC0SS1 = Semaphore_create(0, &semParams, &eb);
//    if (semADC0SS1 == NULL) {
//        System_abort("Can't create ADC0SS1 semaphore");
//    }
//
//
//    /* Configure PE0-PE3 as ADC input channel */
//    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);
//    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_5);
//    MAP_GPIOPinTypeADC(GPIO_PORTP_BASE, GPIO_PIN_6);
//    MAP_GPIOPinTypeADC(GPIO_PORTP_BASE, GPIO_PIN_7);
//    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
//
//    /* Enable the clock to ADC-0 and wait for it to be ready */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)))
//    {
//    }
//
//    /* Configure Sequencer 2 to sample the analog channel : AIN0-AIN3. The
//     * end of conversion and interrupt generation is set for AIN3 */
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, MON_PERCur);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 1, MON_28V);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 2, MON_LOGICCur);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 3, MON_5V_Per);
//    MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 4, MON_5V_Main | ADC_CTL_IE | ADC_CTL_END);
////    MAP_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
//
//    /* Enable sample sequence 2 with a processor signal trigger.  Sequencer 2
//     * will do a single sample when the processor sends a signal to start the
//     * conversion */
//    MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
////    MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
//    /* Since sample sequence 2 is now configured, it must be enabled. */
//    MAP_ADCSequenceEnable(ADC0_BASE, 0);
//
//    /* Clear the interrupt status flag.  This is done to make sure the
//     * interrupt flag is cleared before we sample. */
//    MAP_ADCIntClear(ADC0_BASE, 0);
//    MAP_ADCIntEnable(ADC0_BASE, 0);
////    ADCIntClear(ADC0_BASE, 1);
////    ADCIntEnable(ADC0_BASE, 1);
//
//    /* Sample AIN0 forever.  Display the value on the console. */
//    while(1)
//    {
//        /* Clear the ADC interrupt flag. */
//        MAP_ADCIntClear(ADC0_BASE, 0);
//        /* Trigger the ADC conversion. */
//        MAP_ADCProcessorTrigger(ADC0_BASE, 0);
//
////        /* Wait for conversion to be completed. */
////        while(!MAP_ADCIntStatus(ADC0_BASE, 2, false))
////        {
////        }
//
//        Semaphore_pend(semADC0SS0, BIOS_WAIT_FOREVER);
//
//
//        /* Read ADC Value. */
//        MAP_ADCSequenceDataGet(ADC0_BASE, 0, getADCValue);
//
//        /* Display the AIN0-AIN03 (PE3-PE0) digital value on the console. */
//        Display_printf(g_SMCDisplay, 0, 0, "AIN0-3 = %4d %4d %4d %4d %4d\r", getADCValue[0], getADCValue[1], getADCValue[2], getADCValue[3], getADCValue[4]);
//
//        sleep(1);
//    }
//}


/*
 * *************************************************************************************************************
 */

#define SAM_SAMPLES_COUNT           10
#define SAM_BUFFER_SIZE             (SAM_SAMPLES_COUNT * Board_ADCBUF0CHANNELCOUNT)

uint32_t g_aui32SAMLogicCur[SAM_SAMPLES_COUNT], g_ui32SAMAvgLogicCur;
uint32_t g_aui32SAMPerCur[SAM_SAMPLES_COUNT], g_ui32SAMAvgPerCur;
uint32_t g_aui32SAMV5Main[SAM_SAMPLES_COUNT], g_ui32SAMAvgV5Main;
uint32_t g_aui32SAMV5Per[SAM_SAMPLES_COUNT], g_ui32SAMAvgV5Per;
uint32_t g_aui32V28[SAM_SAMPLES_COUNT], g_ui32SAMAvgV28;
uint32_t g_aui32MCUCur[SAM_SAMPLES_COUNT], g_ui32SAMAvgMCUCur;


uint16_t g_aui16SAMInternalTempSensor[SAM_SAMPLES_COUNT];
uint16_t g_aui16SAMLogicCur[SAM_SAMPLES_COUNT];
uint16_t g_aui16SAMPerCur[SAM_SAMPLES_COUNT];
uint16_t g_aui16SAMV5Main[SAM_SAMPLES_COUNT];
uint16_t g_aui16SAMV5Per[SAM_SAMPLES_COUNT];
uint16_t g_aui16SAMV28[SAM_SAMPLES_COUNT];
uint16_t g_aui16SAMMCUCur[SAM_SAMPLES_COUNT];

uint16_t g_aui16SAMSampleBufferOne[SAM_BUFFER_SIZE];
float g_fSAMAvgTemp;


Void vSAM_taskFxn(UArg arg0, UArg arg1)
{
    ADCBuf_Params adcBufParams;
    /* Driver handles shared between the task and the callback function */
    ADCBuf_Handle adcBuf;
    ADCBuf_Conversion continuousConversions[Board_ADCBUF0CHANNELCOUNT];
    SAM_Channels *psRAWChannels;


    int i;


    Display_printf(g_SMCDisplay, 0, 0, "Starting the System Analog Monitor Task");

    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
    ADCBuf_Params_init(&adcBufParams);
//    adcBufParams.callbackFxn = adcBufCallback;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
//    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
//    adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
    adcBufParams.samplingFrequency = 10000;

    adcBuf = ADCBuf_open(Board_ADCBUF0, &adcBufParams);

    if (!adcBuf){
        /* AdcBuf did not open correctly. */
        while(1);
    }
    /* Configure the conversion struct */
    continuousConversions[0].arg = NULL;
    continuousConversions[0].adcChannel = Board_ADCBUF0CHANNEL0;
    continuousConversions[0].sampleBuffer = g_aui16SAMSampleBufferOne;
    continuousConversions[0].sampleBufferTwo = NULL;
    continuousConversions[0].samplesRequestedCount = SAM_BUFFER_SIZE;

    /* Configure the conversion struct */
    continuousConversions[1].arg = NULL;
    continuousConversions[1].adcChannel = Board_ADCBUF0CHANNEL1;
    continuousConversions[1].sampleBuffer = NULL;
    continuousConversions[1].sampleBufferTwo = NULL;
    continuousConversions[1].samplesRequestedCount = SAM_BUFFER_SIZE;

    /* Configure the conversion struct */
    continuousConversions[2].arg = NULL;
    continuousConversions[2].adcChannel = Board_ADCBUF0CHANNEL2;
    continuousConversions[2].sampleBuffer = NULL;
    continuousConversions[2].sampleBufferTwo = NULL;
    continuousConversions[2].samplesRequestedCount = SAM_BUFFER_SIZE;

    /* Configure the conversion struct */
    continuousConversions[3].arg = NULL;
    continuousConversions[3].adcChannel = Board_ADCBUF0CHANNEL3;
    continuousConversions[3].sampleBuffer = NULL;
    continuousConversions[3].sampleBufferTwo = NULL;
    continuousConversions[3].samplesRequestedCount = SAM_BUFFER_SIZE;

    /* Configure the conversion struct */
    continuousConversions[4].arg = NULL;
    continuousConversions[4].adcChannel = Board_ADCBUF0CHANNEL4;
    continuousConversions[4].sampleBuffer = NULL;
    continuousConversions[4].sampleBufferTwo = NULL;
    continuousConversions[4].samplesRequestedCount = SAM_BUFFER_SIZE;

    /* Configure the conversion struct */
    continuousConversions[5].arg = NULL;
    continuousConversions[5].adcChannel = Board_ADCBUF0CHANNEL5;
    continuousConversions[5].sampleBuffer = NULL;
    continuousConversions[5].sampleBufferTwo = NULL;
    continuousConversions[5].samplesRequestedCount = SAM_BUFFER_SIZE;

    /* Configure the conversion struct */
    continuousConversions[6].arg = NULL;
    continuousConversions[6].adcChannel = Board_ADCBUF0CHANNEL6;
    continuousConversions[6].sampleBuffer = NULL;
    continuousConversions[6].sampleBufferTwo = NULL;
    continuousConversions[6].samplesRequestedCount = SAM_BUFFER_SIZE;


    /*
     * Go to sleep in the foreground thread forever. The data will be collected
     * and transfered in the background thread
     */
    while(1) {
        Task_sleep((unsigned int)10);
        /* Start converting. */
        if (ADCBuf_convert(adcBuf, continuousConversions, Board_ADCBUF0CHANNELCOUNT) == ADCBuf_STATUS_SUCCESS) {

            psRAWChannels = (SAM_Channels *)g_aui16SAMSampleBufferOne;
            for (i = 0; i < SAM_SAMPLES_COUNT; i++) {
                g_aui16SAMInternalTempSensor[i] = psRAWChannels[i].internalTempSensor;
                g_fSAMAvgTemp = (g_aui16SAMInternalTempSensor[i] + 3 * g_fSAMAvgTemp) / 4;

                g_aui16SAMLogicCur[i] = psRAWChannels[i].logicCur;
                g_aui16SAMPerCur[i] = psRAWChannels[i].perCur;
                g_aui16SAMV5Main[i] = psRAWChannels[i].v5Main;
                g_aui16SAMV5Per[i] = psRAWChannels[i].v5Per;
                g_aui16SAMV28[i] = psRAWChannels[i].v28;
                g_aui16SAMMCUCur[i] = psRAWChannels[i].mcuCur;
            }
            /* Convert ADC value to Celsius */
            g_fSAMavgCPUTemperature = (1475*4096 - (75 * 33 * g_fSAMAvgTemp))/ 40960;


            ADCBuf_adjustRawValues(adcBuf, g_aui16SAMLogicCur, SAM_SAMPLES_COUNT, Board_ADCBUF0CHANNEL1);
            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL1, g_aui16SAMLogicCur, g_aui32SAMLogicCur, SAM_SAMPLES_COUNT);

            ADCBuf_adjustRawValues(adcBuf, g_aui16SAMPerCur, SAM_SAMPLES_COUNT, Board_ADCBUF0CHANNEL2);
            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL2, g_aui16SAMPerCur, g_aui32SAMPerCur, SAM_SAMPLES_COUNT);

            ADCBuf_adjustRawValues(adcBuf, g_aui16SAMV5Main, SAM_SAMPLES_COUNT, Board_ADCBUF0CHANNEL3);
            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL3, g_aui16SAMV5Main, g_aui32SAMV5Main, SAM_SAMPLES_COUNT);

            ADCBuf_adjustRawValues(adcBuf, g_aui16SAMV5Per, SAM_SAMPLES_COUNT, Board_ADCBUF0CHANNEL4);
            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL4, g_aui16SAMV5Per, g_aui32SAMV5Per, SAM_SAMPLES_COUNT);

            ADCBuf_adjustRawValues(adcBuf, g_aui16SAMV28, SAM_SAMPLES_COUNT, Board_ADCBUF0CHANNEL5);
            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL5, g_aui16SAMV28, g_aui32V28, SAM_SAMPLES_COUNT);

            ADCBuf_adjustRawValues(adcBuf, g_aui16SAMMCUCur, SAM_SAMPLES_COUNT, Board_ADCBUF0CHANNEL6);
            ADCBuf_convertAdjustedToMicroVolts(adcBuf, Board_ADCBUF0CHANNEL5, g_aui16SAMMCUCur, g_aui32MCUCur, SAM_SAMPLES_COUNT);

            /*
             * Low Pass Filter
             */
            for (i = 0; i < SAM_SAMPLES_COUNT; i++) {
                g_ui32SAMAvgLogicCur = (g_aui32SAMLogicCur[i] + 3 * g_ui32SAMAvgLogicCur) / 4;
                g_ui32SAMAvgPerCur = (g_aui32SAMPerCur[i] + 3 * g_ui32SAMAvgPerCur) / 4;
                g_ui32SAMAvgV5Main = (g_aui32SAMV5Main[i] + 3 * g_ui32SAMAvgV5Main) / 4;
                g_ui32SAMAvgV5Per = (g_aui32SAMV5Per[i] + 3 * g_ui32SAMAvgV5Per) / 4;
                g_ui32SAMAvgV28 = (g_aui32V28[i] + 3 * g_ui32SAMAvgV28) / 4;
                g_ui32SAMAvgMCUCur = (g_aui32V28[i] + 3 * g_ui32SAMAvgMCUCur) / 4;

            }
            /*
             * Unit conversion
             */

            g_fSAMavgLogicCur = 0.1 * g_ui32SAMAvgLogicCur / 1e3;
            g_fSAMavgPerCur = 0.2 * g_ui32SAMAvgPerCur / 1e3;
            g_fSAMavgV5Main = 2 * g_ui32SAMAvgV5Main / 1e6;
            g_fSAMavgV5Per = 2 * g_ui32SAMAvgV5Per / 1e6;
            g_fSAMavgV28 = 11 * g_ui32SAMAvgV28 / 1e6;
            g_fSAMavgMCUCur = 0.1 * g_ui32SAMAvgMCUCur / 1e3;



            /* Print a message with average temperature */
//            Display_printf(g_SMCDisplay, 0, 0, "CPUTemp = %.2fC, LogicCur = %.2fA, PerCur = %.2fA, 5VMain = %.2fV, 5VPer = %.2fV, 28V = %.2fV",
//                           g_fSAMavgCPUTemperature,
//                           g_fSAMavgLogicCur,
//                           g_fSAMavgPerCur,
//                           g_fSAMavgV5Main,
//                           g_fSAMavgV5Per,
//                           g_fSAMavgV28);
        }else{
            Display_printf(g_SMCDisplay, 0, 0, "ADCBuf_convert failed");
        }

    }
}


void vSAM_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing System Analog Monitor\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.priority = 3;
    taskParams.stackSize = 1024;
    Task_create((Task_FuncPtr)vSAM_taskFxn, &taskParams, NULL);
}



