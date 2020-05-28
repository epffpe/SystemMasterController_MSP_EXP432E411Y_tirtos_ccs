/*
 * USBComposite.c
 *
 *  Created on: May 27, 2020
 *      Author: epffpe
 */




#include <stdbool.h>
#include <string.h>

/* For usleep() */
#include <ti/drivers/dpl/ClockP.h> 

/* Driver Header files */
#include <ti/display/Display.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
//#include "ti_drivers_config.h"

/* USB Reference Module Header file */
#include "USB/USBComposite.h"

/*
 *  ======== transmitFxn ========
 *  Task to transmit serial data.
 *
 *  This task periodically sends data to the USB host once it's connected.
 */
void *vSUBSerialTest_transmitFxn(void *arg0)
{
    uint32_t time = 2000000;
    char *text = (char *)arg0;
    while (true) {

        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(WAIT_FOREVER);

        USBCDCD_sendData(text, strlen(text)+1, WAIT_FOREVER);
//        GPIO_toggle(CONFIG_LED_0);

        /* Send data periodically */
        ClockP_usleep(time);
    }
}

/*
 *  ======== receiveFxn ========
 *  Task to receive serial data.
 *
 *  This task will receive data when data is available and block while the
 *  device is not connected to the USB host or if no data was received.
 */
void *vSUBSerialTest_receiveFxn(void *arg0)
{
    unsigned int received;
    unsigned char data[32];


    while (true) {

        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(WAIT_FOREVER);

        received = USBCDCD_receiveData(data, 31, WAIT_FOREVER);
        data[received] = '\0';
//        GPIO_toggle(CONFIG_LED_1);
        if (received) {
//            Display_printf(displayHandle, 0, 0, "Received \"%s\" (%d bytes)\r\n", data, received);
        }
    }
}

