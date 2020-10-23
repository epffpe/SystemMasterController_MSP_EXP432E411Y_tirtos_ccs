/*
 * PassengerControlUnit.c
 *
 *  Created on: Dec 3, 2018
 *      Author: epenate
 */

#define __PANASONIC_PASSENGERCONTROLUNIT_GLOBAL
#include "includes.h"

volatile uint32_t g_ui32PPPCUTimeUp, g_ui32PPPCUTimeDown;




/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON0.
 */
void pvPPPCU_inputLineIntHandler(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */
//    GPIO_toggle(Board_GPIO_LED0);
    uint32_t t = Timestamp_get32();
    uint32_t n = GPIO_read(SMC_GPI_0);
    if(n) {
        g_ui32PPPCUTimeUp = t;
    }else {
        g_ui32PPPCUTimeDown = t;
        t -= g_ui32PPPCUTimeUp;
    }
    GPIO_write(SMC_LED_D20, n);
}

/*
 * disconnect Diode D20 and SMC_GPI_0 from DIO module
 */

void vPPPCU_init()
{
    //    GPIO_write(SMC_GPO_0, 1);
    //    GPIO_setConfig(SMC_GPO_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(SMC_GPI_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES);
    /* install Button callback */
    GPIO_setCallback(SMC_GPI_0, pvPPPCU_inputLineIntHandler);

    /* Enable interrupts */
    GPIO_enableInt(SMC_GPI_0);
}
