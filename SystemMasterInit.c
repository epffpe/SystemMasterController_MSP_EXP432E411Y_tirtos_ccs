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


void test();
void vHeartBeat_init();

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
    /* Call driver init functions */
    GPIO_init();
    // I2C_init();
    // SDSPI_init();
    // SPI_init();
    UART_init();
    // Watchdog_init();

    SMCDisplay_init();
    GPIO_write(SMC_SERIAL0_DE, 1);
    Display_printf(g_SMCDisplay, 0, 0, "Starting the System Master Controller\n"
                   "-- Compiled: "__DATE__" "__TIME__" --\n");

    vHeartBeat_init();

    /*
     * Initialize interfaces
     */
    vIF_init();


    test();
    return 0;
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


    /* Call driver init functions. */
    PWM_init();

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
        GPIO_write(Board_LED3,0);
////        DOSet(DIO_LED_D6, DO_ON);
        Task_sleep((unsigned int)50);
        GPIO_write(Board_LED3,1);
////        DOSet(DIO_LED_D6, DO_OFF);
        Task_sleep((unsigned int)50);
        GPIO_write(Board_LED3,0);
////        DOSet(DIO_LED_D6, DO_ON);
        Task_sleep((unsigned int)50);
        GPIO_write(Board_LED3,1);
////        DOSet(DIO_LED_D6, DO_OFF);
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
