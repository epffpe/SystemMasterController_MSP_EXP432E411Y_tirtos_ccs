/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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

#ifndef __BOARD_H
#define __BOARD_H

#define Board_MSP_EXP432E401Y

#ifdef __cplusplus
extern "C" {
#endif

#include "MSP_EXP432E401Y.h"

#define Board_initGeneral           MSP_EXP432E401Y_initGeneral

#define Board_ADC0                  MSP_EXP432E401Y_ADC0
#define Board_ADC1                  MSP_EXP432E401Y_ADC1

#define Board_ADCBUF0               MSP_EXP432E401Y_ADCBUF0
#define Board_ADCBUF0CHANNEL0       MSP_EXP432E401Y_ADCBUF0CHANNEL0
#define Board_ADCBUF0CHANNEL1       MSP_EXP432E401Y_ADCBUF0CHANNEL1
#define Board_ADCBUF0CHANNEL2       MSP_EXP432E401Y_ADCBUF0CHANNEL2
#define Board_ADCBUF0CHANNEL3       MSP_EXP432E401Y_ADCBUF0CHANNEL3
#define Board_ADCBUF0CHANNEL4       MSP_EXP432E401Y_ADCBUF0CHANNEL4
#define Board_ADCBUF0CHANNEL5       MSP_EXP432E401Y_ADCBUF0CHANNEL5
#define Board_ADCBUF0CHANNEL6       MSP_EXP432E401Y_ADCBUF0CHANNEL6

#define Board_ADCBUF0CHANNELCOUNT   MSP_EXP432E401Y_ADCBUF0CHANNELCOUNT

#define Board_CAN0                  MSP_EXP432E401Y_CAN0
#define Board_CAN1                  MSP_EXP432E401Y_CAN1

#define Board_GPIO_LED_ON           MSP_EXP432E401Y_GPIO_LED_ON
#define Board_GPIO_LED_OFF          MSP_EXP432E401Y_GPIO_LED_OFF
#define Board_GPIO_LED0             MSP_EXP432E401Y_GPIO_D1
#define Board_GPIO_LED1             MSP_EXP432E401Y_GPIO_D2
#define Board_GPIO_LED2             MSP_EXP432E401Y_GPIO_D2
#define Board_GPIO_BUTTON0          MSP_EXP432E401Y_GPIO_USR_SW1
#define Board_GPIO_BUTTON1          MSP_EXP432E401Y_GPIO_USR_SW2

#define Board_LED3                  SMC_LED_D6
#define Board_LED4                  SMC_LED_D20


#define Board_I2C0                  MSP_EXP432E401Y_I2C0
#define Board_I2C_TMP               MSP_EXP432E401Y_I2C0
#define Board_I2C_TPL0401           MSP_EXP432E401Y_I2C7

#define Board_NVSINTERNAL           MSP_EXP432E401Y_NVSMSP432E40
#define Board_NVSEXTERNAL           MSP_EXP432E401Y_EXT_FLASH

#define Board_PWM0                  MSP_EXP432E401Y_PWM0

#define Board_SD0                   MSP_EXP432E401Y_SDSPI0

#define Board_SDFatFS0              MSP_EXP432E401Y_SDSPI0

#define Board_SPI0                  MSP_EXP432E401Y_SPI2
#define Board_SPI1                  MSP_EXP432E401Y_SPI3
#define Board_SPI2                  MSP_EXP432E401Y_SPI0
#define Board_SPI3                  MSP_EXP432E401Y_SPI1

#define Board_SPI_ADC               MSP_EXP432E401Y_SPI1
#define Board_SPI_DAC               MSP_EXP432E401Y_SPI2
#define Board_SPI_FLASH             MSP_EXP432E401Y_SPI3
#define Board_SPI_FRAM              MSP_EXP432E401Y_SPI0

#define Board_SPI_MASTER            MSP_EXP432E401Y_SPI2
#define Board_SPI_SLAVE             MSP_EXP432E401Y_SPI2
#define Board_SPI_MASTER_READY      MSP_EXP432E401Y_SPI_MASTER_READY
#define Board_SPI_SLAVE_READY       MSP_EXP432E401Y_SPI_SLAVE_READY

#define Board_TIMER0                MSP_EXP432E401Y_TIMER0
#define Board_TIMER1                MSP_EXP432E401Y_TIMER1
#define Board_TIMER2                MSP_EXP432E401Y_TIMER2



#define Board_GPI_0                 SMC_GPI_0
#define Board_GPI_1                 SMC_GPI_1
#define Board_GPI_2                 SMC_GPI_2
#define Board_GPI_3                 SMC_GPI_3
#define Board_GPI_4                 SMC_GPI_4
#define Board_GPI_5                 SMC_GPI_5


#define Board_GPO_0                 SMC_GPO_0
#define Board_GPO_1                 SMC_GPO_1
#define Board_GPO_2                 SMC_GPO_2
#define Board_GPO_3                 SMC_GPO_3
#define Board_GPO_4                 SMC_GPO_4
#define Board_GPO_5                 SMC_GPO_5


#define Board_GPIOCount             MSP_EXP432E401Y_GPIOCOUNT


/*
 * Serial Ports
 */
#define Board_Serial0               MSP_EXP432E401Y_UART3
#define Board_Serial1               MSP_EXP432E401Y_UART6
#define Board_Serial2               MSP_EXP432E401Y_UART4
#define Board_Serial3               MSP_EXP432E401Y_UART0
#define Board_Serial4               MSP_EXP432E401Y_UART2
#define Board_Serial5               MSP_EXP432E401Y_UART1
#define Board_Serial6               MSP_EXP432E401Y_UART5
#define Board_Serial7               MSP_EXP432E401Y_UART7

#define Board_SerialMultinet        Board_Serial6
#define Board_SerialDebug           Board_Serial5

#define Board_SerialCount           MSP_EXP432E401Y_UARTCOUNT










#define Board_WATCHDOG0             MSP_EXP432E401Y_WATCHDOG0

/* Board specific I2C addresses */
#define Board_TMP_ADDR              (0x40)
#define Board_SENSORS_BP_TMP_ADDR   Board_TMP_ADDR
#define Board_TPL0401_ADDR          (0x40)

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
