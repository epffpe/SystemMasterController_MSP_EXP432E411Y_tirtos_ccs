/*
 * MAX1301.c
 *
 *  Created on: Jul 16, 2019
 *      Author: epenate
 */


#define _SYSTEM_MAX1301_MAX1301_GLOBAL
#include "includes.h"






Void vMAX1301_taskFxn(UArg arg0, UArg arg1)
{
    SPI_Handle      masterSpi;
    SPI_Params      spiParams;
    SPI_Transaction transaction;
//    uint32_t        i;
    bool            transferOK;
//    int32_t         status;
    uint8_t        ui8TxBuff[4], ui8RxBuff[4];
    int             val;
    float           fval;


    /* Open SPI as master (default) */
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA0;
    spiParams.dataSize = 16;
    spiParams.bitRate = 1000000;
    masterSpi = SPI_open(Board_SPI_ADC, &spiParams);
    if (masterSpi == NULL) {
        Display_printf(g_SMCDisplay, 0, 0, "Error initializing master SPI\n");
        return;
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "Master SPI initialized\n");
    }
    GPIO_write(SMC_MAX1301_FSS, 1);
    /* Initialize master SPI transaction structure */
//    masterTxBuffer[sizeof(MASTER_MSG) - 1] = (i % 10) + '0';
//    memset((void *) masterRxBuffer, 0, SPI_MSG_LENGTH);


    transaction.count = 1;
    transaction.txBuf = (void *)ui8TxBuff;
    transaction.rxBuf = NULL;

//    /*
//     * Reset
//     */
//    ui8TxBuff[3] = 0;
//    ui8TxBuff[2] = 0;
////    ui8TxBuff[1] = (0x80 + (2<<4) + 3) ;
//    ui8TxBuff[1] = 0;
//    ui8TxBuff[0] = (0x80 + (4<<4) + 0x08);
//
//    GPIO_write(SMC_MAX1301_FSS, 0);
//    transferOK = SPI_transfer(masterSpi, &transaction);
//    GPIO_write(SMC_MAX1301_FSS, 1);
//    if (transferOK) {
//        //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
//    }
//    else {
//        Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
//    }
//
//    Task_sleep((unsigned int)100);

    /*
     * Config Range
     */
    ui8TxBuff[3] = 0;
    ui8TxBuff[2] = 0;
    ui8TxBuff[1] = 0;
    ui8TxBuff[0] = 0;
    ui8TxBuff[1] = (0x80 + (1<<4) + 6) ;

    GPIO_write(SMC_MAX1301_FSS, 0);
    transferOK = SPI_transfer(masterSpi, &transaction);
    GPIO_write(SMC_MAX1301_FSS, 1);
    if (transferOK) {
        //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
    }

    ui8TxBuff[3] = 0;
    ui8TxBuff[2] = 0;
    ui8TxBuff[1] = 0;
    ui8TxBuff[0] = 0;
    ui8TxBuff[1] = (0x80 + (2<<4) + 3) ;

    GPIO_write(SMC_MAX1301_FSS, 0);
    transferOK = SPI_transfer(masterSpi, &transaction);
    GPIO_write(SMC_MAX1301_FSS, 1);
    if (transferOK) {
        //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
    }
    Task_sleep((unsigned int)1);
//
//    /*
//     * Config mode
//     */
//    ui8TxBuff[3] = 0;
//    ui8TxBuff[2] = 0;
////    ui8TxBuff[1] = (0x80 + (2<<4) + 3) ;
//    ui8TxBuff[1] = 0;
//    ui8TxBuff[0] = (0x80 + (2<<4) + 0x08);
//
//    GPIO_write(SMC_MAX1301_FSS, 0);
//    transferOK = SPI_transfer(masterSpi, &transaction);
//    GPIO_write(SMC_MAX1301_FSS, 1);
//    if (transferOK) {
//        //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
//    }
//    else {
//        Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
//    }
//    Task_sleep((unsigned int)1);

    transaction.count = 2;
    transaction.txBuf = (void *)ui8TxBuff;
    transaction.rxBuf = (void *)ui8RxBuff;

    while(1) {
        Task_sleep((unsigned int)1);

        /*
         * Channel 0 - VIN
         */
        ui8TxBuff[3] = 0;
        ui8TxBuff[2] = 0;
        ui8TxBuff[1] = 0;
        ui8TxBuff[0] = 0;
        ui8TxBuff[1] = (0x80 + (0<<4));

        GPIO_write(SMC_MAX1301_FSS, 0);
        transferOK = SPI_transfer(masterSpi, &transaction);
        GPIO_write(SMC_MAX1301_FSS, 1);
        if (transferOK) {
            //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
        }
        else {
            Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
        }

        val = (ui8RxBuff[3] << 8) | (ui8RxBuff[2] << 0);
        val -= 32763;
        fval = (6 * 4.096 * val) / 65536;
        g_fMAX1301Vin_volt = fval;

        /*
         * Channel 1 - VOUT
         */
        ui8TxBuff[3] = 0;
        ui8TxBuff[2] = 0;
        ui8TxBuff[1] = 0;
        ui8TxBuff[0] = 0;
        ui8TxBuff[1] = (0x80 + (1<<4));

        GPIO_write(SMC_MAX1301_FSS, 0);
        transferOK = SPI_transfer(masterSpi, &transaction);
        GPIO_write(SMC_MAX1301_FSS, 1);
        if (transferOK) {
            //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
        }
        else {
            Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
        }

        val = (ui8RxBuff[3] << 8) | (ui8RxBuff[2] << 0);
//        val -= 32763;
        fval = (3 * 4.096 * val) / 65536;
//        val -= 32763;
//        fval = (6 * 4.096 * val) / 65536;
        fval *= 5;
        g_fMAX1301Vout_volt = fval;



        /*
         * Channel 2 - 5V
         */
        ui8TxBuff[3] = 0;
        ui8TxBuff[2] = 0;
        ui8TxBuff[1] = 0;
        ui8TxBuff[0] = 0;
        ui8TxBuff[1] = (0x80 + (2<<4));

        GPIO_write(SMC_MAX1301_FSS, 0);
        transferOK = SPI_transfer(masterSpi, &transaction);
        GPIO_write(SMC_MAX1301_FSS, 1);
        if (transferOK) {
            //            Display_printf(g_SMCDisplay, 0, 0, "Transfer Ok");
        }
        else {
            Display_printf(g_SMCDisplay, 0, 0, "Unsuccessful master SPI transfer");
        }

        val = (ui8RxBuff[3] << 8) | (ui8RxBuff[2] << 0);
//        val -= 32763;
        fval = (1.5 * 4.096 * val) / 65536;

        g_fMAX1301V5_volt = fval;
    }
//    SPI_close(masterSpi);
}


void vMAX1301_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing vMAX1301_taskFxn\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.priority = MAX1301_TASK_PRIORITY;
    taskParams.stackSize = MAX1301_TASK_STACK_SIZE;
    Task_create((Task_FuncPtr)vMAX1301_taskFxn, &taskParams, NULL);
}




