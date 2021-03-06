/*
 * USBComposite.c
 *
 *  Created on: May 27, 2020
 *      Author: epffpe
 */


#include "includes.h"


/*
 *  ======== transmitFxn ========
 *  Task to transmit serial data.
 *
 *  This task periodically sends data to the USB host once it's connected.
 */
void *vUSBSerialTest_transmitFxn(void *arg0)
{
    uint32_t time = 2000000;
    char *text = (char *)arg0;
    while (true) {

        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(USBCDCD_Console, WAIT_FOREVER);

        USBCDCD_sendData(USBCDCD_Console, text, strlen(text)+1, WAIT_FOREVER);
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
void *vUSBSerialTest_receiveFxn(void *arg0)
{
    unsigned int received;
    unsigned char data[32];


    while (true) {

        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(USBCDCD_Console, WAIT_FOREVER);

        received = USBCDCD_receiveData(USBCDCD_Console, data, 31, WAIT_FOREVER);
        data[received] = '\0';
//        GPIO_toggle(CONFIG_LED_1);
        if (received) {
//            Display_printf(displayHandle, 0, 0, "Received \"%s\" (%d bytes)\r\n", data, received);
        }
    }
}


/*
 *  ======== transmitFxn ========
 *  Task to transmit serial data.
 *
 *  This task periodically sends data to the USB host once it's connected.
 */
void *vSUBSerialTest2_transmitFxn(void *arg0)
{
    uint32_t time = 2000000;
    char *text = (char *)arg0;
    while (true) {

        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(USBCDCD_ForteVerifier, WAIT_FOREVER);

        USBCDCD_sendData(USBCDCD_ForteVerifier, text, strlen(text)+1, WAIT_FOREVER);
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
void *vSUBSerialTest2_receiveFxn(void *arg0)
{
    unsigned int received;
    unsigned char data[32];


    while (true) {

        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(USBCDCD_ForteVerifier, WAIT_FOREVER);

        received = USBCDCD_receiveData(USBCDCD_ForteVerifier, data, 31, WAIT_FOREVER);
        data[received] = '\0';
//        GPIO_toggle(CONFIG_LED_1);
        if (received) {
//            Display_printf(displayHandle, 0, 0, "Received \"%s\" (%d bytes)\r\n", data, received);
        }
    }
}



void USBSerialTest_init()
{
    pthread_t         thread;
    pthread_attr_t    attrs;
    struct sched_param  priParam;
    int                 retc;

    char *text = "SMC controls USB Console.\r\n";
    char *text2 = "SMC controls USB Forte Verifier.\r\n";
      /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    priParam.sched_priority = 1;

    retc = pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &priParam);

    retc |= pthread_attr_setstacksize(&attrs, 768);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

   /*
    * USB Console test
    */

//    retc = pthread_create(&thread, &attrs, vSUBSerialTest_transmitFxn, (void *)text);
//    if (retc != 0) {
//        /* pthread_create() failed */
//        while (1);
//    }
//
//    priParam.sched_priority = 2;
//
//    pthread_attr_setschedparam(&attrs, &priParam);
//
//    retc = pthread_create(&thread, &attrs, vSUBSerialTest_receiveFxn, NULL);
//    if (retc != 0) {
//        /* pthread_create() failed */
//        while (1);
//    }

    /*
     * Forte Verifier test
     */

    retc = pthread_create(&thread, &attrs, vSUBSerialTest2_transmitFxn, (void *)text2);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = 2;

    pthread_attr_setschedparam(&attrs, &priParam);

    retc = pthread_create(&thread, &attrs, vSUBSerialTest2_receiveFxn, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }
}

