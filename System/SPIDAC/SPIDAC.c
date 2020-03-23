/*
 * SPIDAC.c
 *
 *  Created on: Jul 12, 2019
 *      Author: epenate
 */


#define _SYSTEM_SPIDAC_SPIDAC_GLOBAL
#include "includes.h"

Queue_Handle g_SPIDACmsgQHandle;
Event_Handle g_SPIDACeventHandle;
Clock_Handle g_SPIDACclockHandle;

Void vSPIDAC101_taskFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    SPI_Handle      masterSpi;
    SPI_Params      spiParams;
    SPI_Transaction transaction;
    device_msg_t    *pMsg;
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
//        Task_sleep((unsigned int)1);
        events = Event_pend(g_SPIDACeventHandle, 0, DEVICE_ALL_EVENTS, BIOS_WAIT_FOREVER); //BIOS_WAIT_FOREVER


        if (events & DEVICE_PERIODIC_EVT) {
            events &= ~DEVICE_PERIODIC_EVT;

            //        }
        }

        if (events & DEVICE_APP_KILL_EVT) {
            events &= ~DEVICE_APP_KILL_EVT;

            Clock_stop(g_SPIDACclockHandle);
            Clock_delete(&g_SPIDACclockHandle);

            /*
             * Needs to flush the messages in the queue before remove it
             */
            while (!Queue_empty(g_SPIDACmsgQHandle)) {
                pMsg = Queue_get(g_SPIDACmsgQHandle);
                Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
            }
            Queue_delete(&g_SPIDACmsgQHandle);
            Event_delete(&g_SPIDACeventHandle);
            Task_exit();
        }

        // TODO: Implement messages
        // Process messages sent from another task or another context.
        while (!Queue_empty(g_SPIDACmsgQHandle))
        {
            pMsg = Queue_dequeue(g_SPIDACmsgQHandle);

            // Process application-layer message probably sent from ourselves.
//            vALTOAmp_processApplicationMessage(pMsg, myDeviceID, ifHandle, txbuff, rxbuff);


            dacVal = *(uint16_t *)pMsg->pdu;
            if (dacVal > 1023) {
                dacVal = 1023;
            }
//            dacVal = 512;

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

            // Free the received message.
            Memory_free(pMsg->heap, pMsg, pMsg->pduLen);
//            Task_sleep(3000);
        }

    }
//    SPI_close(masterSpi);
}


void vSPIDAC_setRawValue(uint16_t value)
{
    Error_Block eb;

    ASSERT(value < 1024);

//    if (devHandle == NULL) {
//        return;
//    }

    Error_init(&eb);
    // Allocate memory for the message.
    device_msg_t *pMsg = Memory_alloc(NULL, sizeof(device_msg_t) + sizeof(uint16_t), 0, &eb);

    if (pMsg != NULL)
    {
        pMsg->heap = NULL;
        pMsg->type = APP_MSG_SERVICE_WRITE;
        pMsg->pduLen = sizeof(device_msg_t) + sizeof(uint16_t);

        // Copy data into message
        memcpy(pMsg->pdu, &value, sizeof(uint16_t));

        // Enqueue the message using pointer to queue node element.
        Queue_enqueue(g_SPIDACmsgQHandle, &pMsg->_elem);
        //    // Let application know there's a message.
        Event_post(g_SPIDACeventHandle, DEVICE_APP_MSG_EVT);
    }
}

void vSPIDAC_setVoltageLevel(float volts)
{
    float fGain;
    float fVal;
    uint16_t ui16RawValue;
    fGain = 1.0 + (g_ui32SPIDAC101_R46Val/g_ui32SPIDAC101_R47Val);

    fVal = volts / fGain;

    fVal = fVal * 1024 / 5.0;

    ui16RawValue = (uint16_t)fVal;

    vSPIDAC_setRawValue(ui16RawValue);
}


void vSPIDAC_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, DEVICE_PERIODIC_EVT);
}

void vSPIDAC101_init(uint32_t r46, uint32_t r47)
{
    Task_Params     taskParams;
    Clock_Params    clockParams;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);


    Display_printf(g_SMCDisplay, 0, 0, "Initializing vSPIDAC101_taskFxn\n");

    g_SPIDACeventHandle = Event_create(NULL, &eb);
    g_SPIDACmsgQHandle = Queue_create(NULL, NULL);

    g_ui32SPIDAC101_R46Val = r46;
    g_ui32SPIDAC101_R47Val = r47;

//    Clock_Params_init(&clockParams);
//    clockParams.period = SPIDAC_CLOCK_PERIOD;
//    clockParams.startFlag = TRUE;
//    clockParams.arg = (UArg)g_SPIDACeventHandle;
//    g_SPIDACclockHandle = Clock_create(vSPIDAC_clockHandler, SPIDAC_CLOCK_TIMEOUT, &clockParams, &eb);
//    if (g_SPIDACclockHandle == NULL) {
//        System_abort("Clock create failed");
//    }

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.priority = SPIDAC101_TASK_PRIORITY;
    taskParams.stackSize = SPIDAC101_TASK_STACK_SIZE;
    Task_create((Task_FuncPtr)vSPIDAC101_taskFxn, &taskParams, NULL);
}





