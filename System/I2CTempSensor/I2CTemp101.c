/*
 * I2CTemp101.c
 *
 *  Created on: May 20, 2019
 *      Author: epenate
 */

#define _SYSTEM_I2CTEMPSENSOR_I2CTEMP101_GLOBAL
#include "includes.h"


/* TMP116 DIE Temp Result Register */
#define TMP116_DIE_TEMP     0x0000
#define TMP116_ADDR         0x48

Void vI2CTemp101Fxn(UArg arg0, UArg arg1)
{

//    uint16_t        sample;
    int16_t         temperature;
    uint8_t         txBuffer[1];
    uint8_t         rxBuffer[2];
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;


    /* Turn on user LED */
//    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
//    Display_printf(g_SMCDisplay, 0, 0, "Starting the i2ctmp116 example\n");
    Display_printf(g_SMCDisplay, 0, 0, "Starting the vI2CTemp101Fxn\n");

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL) {
        Display_printf(g_SMCDisplay, 0, 0, "Error Initializing I2C\n");
        while (1);
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "I2C Initialized!\n");
    }

    /* Point to the die tempature register and read its 2 bytes */
    txBuffer[0] = TMP116_DIE_TEMP;
    i2cTransaction.slaveAddress = TMP116_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;


//    for (sample = 0;; sample++) {
    for (;;) {
        if (I2C_transfer(i2c, &i2cTransaction)) {
            /* Extract degrees C from the received data; see TMP116 datasheet */
            temperature = (rxBuffer[0] << 4) | (rxBuffer[1] >> 4);
//            temperature *= 0.0078125;

            /*
             * If the MSB is set '1', then we have a 2's complement
             * negative value which needs to be sign extended
             */
            if (rxBuffer[0] & 0x80) {
                temperature |= 0xF000;
            }
//            g_fI2CTempSensor = temperature;
//            g_fI2CTempSensor *= 0.0625;

            temperature *= 0.0625; //shift 4 to right

            g_i16I2CTempSensor = temperature;

//            Display_printf(g_SMCDisplay, 0, 0, "Sample %u: %d (C)",
//                sample, g_i16I2CTempSensor);
        }
        else {
            Display_printf(g_SMCDisplay, 0, 0, "I2C Bus fault.");
        }

        /* Sleep for 1 second */
        sleep(1);
    }

//    I2C_close(i2c);
//    Display_printf(g_SMCDisplay, 0, 0, "I2C closed!");

}


void vI2CTemp101_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing heartbeat\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.priority = I2CTEMP101_TASK_PRIORITY;
    taskParams.stackSize = I2CTEMP101_TASK_STACK_SIZE;
    Task_create((Task_FuncPtr)vI2CTemp101Fxn, &taskParams, NULL);
}

