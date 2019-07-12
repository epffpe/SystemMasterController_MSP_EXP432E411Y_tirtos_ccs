/*
 * SPIDAC.c
 *
 *  Created on: Jul 12, 2019
 *      Author: epenate
 */


#define _SYSTEM_SPIDAC_SPIDAC_GLOBAL
#include "includes.h"











Void vSPIDAC101_taskFxn(UArg arg0, UArg arg1)
{
    SPI_Handle      masterSpi;
    SPI_Params      spiParams;
    SPI_Transaction transaction;
//    uint32_t        i;
    bool            transferOK;
//    int32_t         status;
    uint16_t        dataVal;
    uint16_t        dacVal;

    GPIO_write(SMC_DAC_FSS, 0);
    /* Open SPI as master (default) */
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL1_PHA0;
    spiParams.dataSize = 16;
    spiParams.bitRate = 20000000;
    masterSpi = SPI_open(Board_SPI_DAC, &spiParams);
    if (masterSpi == NULL) {
        Display_printf(g_SMCDisplay, 0, 0, "Error initializing master SPI\n");
        return;
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "Master SPI initialized\n");
    }

    /* Initialize master SPI transaction structure */
//    masterTxBuffer[sizeof(MASTER_MSG) - 1] = (i % 10) + '0';
//    memset((void *) masterRxBuffer, 0, SPI_MSG_LENGTH);
    transaction.count = sizeof(uint16_t);
    transaction.txBuf = (void *) &dataVal;
    transaction.rxBuf = NULL;

    dacVal = 0;
    while(1) {
        Task_sleep((unsigned int)1);

        if (dacVal++ > 1023) {
            dacVal = 0;
        }

//        for (dacVal = 0; dacVal < 1024; dacVal++) {
            g_ui16SPIDAC101_dacVal = dacVal;

            dataVal = (dacVal & 0x3FF) << 2;
            //        dacVal += 100;
            GPIO_write(SMC_DAC_FSS, 1);
            GPIO_write(SMC_DAC_FSS, 0);
            transferOK = SPI_transfer(masterSpi, &transaction);
            if (transferOK) {
                //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
            }
            else {
                Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
            }
//        }

    }
//    SPI_close(masterSpi);
}


void vSPIDAC101_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing heartbeat\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.priority = SPIDAC101_TASK_PRIORITY;
    taskParams.stackSize = SPIDAC101_TASK_STACK_SIZE;
    Task_create((Task_FuncPtr)vSPIDAC101_taskFxn, &taskParams, NULL);
}



