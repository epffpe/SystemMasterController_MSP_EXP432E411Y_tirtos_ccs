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
/** ============================================================================
 *  @file       MSP_EXP432E401Y.h
 *
 *  @brief      MSP_EXP432E401Y Board Specific APIs
 *
 *  The MSP_EXP432E401Y header file should be included in an application as
 *  follows:
 *  @code
 *  #include <MSP_EXP432E401Y.h>
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef __MSP_EXP432E401Y_H
#define __MSP_EXP432E401Y_H

#ifdef __cplusplus
extern "C" {
#endif

/* LEDs on MSP_EXP432E401Y are active high. */
#define MSP_EXP432E401Y_GPIO_LED_OFF (0)
#define MSP_EXP432E401Y_GPIO_LED_ON  (1)

/*!
 *  @def    MSP_EXP432E401Y_ADCName
 *  @brief  Enum of ADC channels on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_ADCName {
    MSP_EXP432E401Y_ADC0 = 0,
    MSP_EXP432E401Y_ADC1,

    MSP_EXP432E401Y_ADCCOUNT
} MSP_EXP432E401Y_ADCName;

/*!
 *  @def    MSP_EXP432E401Y_ADCBufName
 *  @brief  Enum of ADC hardware peripherals on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_ADCBufName {
    MSP_EXP432E401Y_ADCBUF0 = 0,

    MSP_EXP432E401Y_ADCBUFCOUNT
} MSP_EXP432E401Y_ADCBufName;

/*!
 *  @def    MSP_EXP432E401Y_ADCBuf0ChannelName
 *  @brief  Enum of ADCBuf channels on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_ADCBuf0ChannelName {
    MSP_EXP432E401Y_ADCBUF0CHANNEL0 = 0,
    MSP_EXP432E401Y_ADCBUF0CHANNEL1,
    MSP_EXP432E401Y_ADCBUF0CHANNEL2,
    MSP_EXP432E401Y_ADCBUF0CHANNEL3,
    MSP_EXP432E401Y_ADCBUF0CHANNEL4,
    MSP_EXP432E401Y_ADCBUF0CHANNEL5,
    MSP_EXP432E401Y_ADCBUF0CHANNEL6,

    MSP_EXP432E401Y_ADCBUF0CHANNELCOUNT
} MSP_EXP432E401Y_ADCBuf0ChannelName;

/*!
 *  @def    MSP_EXP432E401Y_AESCBCName
 *  @brief  Enum of virtual AESCBC instances on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_AESCBCName {
    MSP_EXP432E401Y_AESCBC0 = 0,
    MSP_EXP432E401Y_AESCBCCOUNT
} MSP_EXP432E401Y_AESCBCName;

/*!
 *  @def    MSP_EXP432E401Y_AESCTRName
 *  @brief  Enum of virtual AESCTR instances on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_AESCTRName {
    MSP_EXP432E401Y_AESCTR0 = 0,
    MSP_EXP432E401Y_AESCTRCOUNT
} MSP_EXP432E401Y_AESCTRName;

/*!
 *  @def    MSP_EXP432E401Y_AESCCMName
 *  @brief  Enum of virtual AESCCM instances on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_AESCCMName {
    MSP_EXP432E401Y_AESCCM0 = 0,
    MSP_EXP432E401Y_AESCCMCOUNT
} MSP_EXP432E401Y_AESCCMName;

/*!
 *  @def    MSP_EXP432E401Y_AESGCMName
 *  @brief  Enum of virtual AESGCM instances on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_AESGCMName {
    MSP_EXP432E401Y_AESGCM0 = 0,
    MSP_EXP432E401Y_AESGCMCOUNT
} MSP_EXP432E401Y_AESGCMName;

/*!
 *  @def    MSP_EXP432E401Y_AESCTRDRBGName
 *  @brief  Enum of virtual AESCTRDRBG instances on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_AESCTRDRBGName {
    MSP_EXP432E401Y_AESCTRDRBG0 = 0,
    MSP_EXP432E401Y_AESCTRDRBGCOUNT
} MSP_EXP432E401Y_AESCTRDRBGName;

/*!
 *  @def    MSP_EXP432E401Y_CANName
 *  @brief  Enum of CAN controllers on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_CANName {
    MSP_EXP432E401Y_CAN0 = 0,
    MSP_EXP432E401Y_CAN1,

    MSP_EXP432E401Y_CANCOUNT
} MSP_EXP432E401Y_CANName;

/*!
 *  @def    MSP_EXP432E401Y_CRCName
 *  @brief  Enum of CRC instances on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_CRCName {
    MSP_EXP432E401Y_CRC0 = 0,
    MSP_EXP432E401Y_CRCCOUNT
} MSP_EXP432E401Y_CRCName;

/*!
 *  @def    MSP_EXP432E401Y_GPIOName
 *  @brief  Enum of LED names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_GPIOName {
//    MSP_EXP432E401Y_GPIO_USR_SW1 = 0,
//    MSP_EXP432E401Y_GPIO_USR_SW2,
//    MSP_EXP432E401Y_SPI_MASTER_READY,
//    MSP_EXP432E401Y_SPI_SLAVE_READY,
//    MSP_EXP432E401Y_GPIO_D1,
//    MSP_EXP432E401Y_GPIO_D2,
//
//    MSP_EXP432E401Y_SDSPI_CS,
//
//    /* Sharp LCD Pins */
//    MSP_EXP432E401Y_LCD_CS,
//    MSP_EXP432E401Y_LCD_POWER,
//    MSP_EXP432E401Y_LCD_ENABLE,
    /****************************************************
     * INPUTS
     ***************************************************/
    /*
     * POWER
     */
    SMC_5V_OUT_STAT = 0,
    SMC_INTERNAL_5V_SW_STATUS,
    /*
     * DIGITAL INPUTS AND OUTPUTS
     */
    SMC_IRDA_RX,
    SMC_TEMP_ALERT,
    SMC_GPI_0,
    SMC_GPI_1,
    SMC_GPI_2,
    SMC_GPI_3,
    SMC_GPI_4,
    SMC_GPI_5,
    /*
     * EXTENDED INTERFACE TO MAX10 FPGA
     */
    SMC_MAX10_JAM_TDO,
    SMC_MAX10_AUX_IN_0,
    SMC_MAX10_AUX_IN_1,
    SMC_MAX10_AUX_IN_2,
    SMC_MAX10_AUX_IN_3,
    SMC_MAX10_AUX_IN_4,
    SMC_MAX10_AUX_IN_5,
    SMC_MAX10_AUX_IN_6,
    SMC_MAX10_AUX_IN_7,
    SMC_MAX10_AUX_IN_8,
    SMC_MAX10_AUX_IN_9,
    SMC_MAX10_AUX_IN_10,
    SMC_MAX10_AUX_IN_11,
    SMC_MAX10_AUX_IN_12,
    SMC_MAX10_AUX_IN_13,
    SMC_MAX10_AUX_IN_14,
    SMC_MAX10_AUX_IN_15,
    SMC_MAX10_AUX_IN_16,
    SMC_MAX10_AUX_IN_17,
    SMC_MAX10_AUX_IN_18,
    SMC_MAX10_AUX_IN_19,
    SMC_BLE_SRDY,
    /*
     * ANALOG
     */
    SMC_ADC_SSTRB,




    /* REV 1.0 FORGOT TO CONNECT TO THE CPU */
//    SMC_MAX10_NSTATUS,
//    SMC_MAX10_CONF_DONE,

    /****************************************************
     * OUTPUTS
     ***************************************************/
    /*
     * MCU - POWER AND JTAG
     */
    SMC_UART_DEBUG,
    SMC_SERIAL5_EN_,
    /*
     * ETH AND USB
     */
#ifdef __ETH_LED_AS_GPIO
    SMC_LED_D10,
    SMC_LED_D11,
    SMC_LED_D12,
#endif

    SMC_LED_D6,
    SMC_LED_D20,
    SMC_USB0EPEN,
    /*
     * RS485
     */
    SMC_SERIAL0_DE,
    SMC_SERIAL0_RE,
    SMC_SERIAL1_DE,
    SMC_SERIAL1_RE,
    SMC_SERIAL2_DE,
    SMC_SERIAL2_RE,
    /*
     * RS485 4 AND 5
     */
    SMC_SERIAL3_DE,
    SMC_SERIAL3_RE,
    SMC_SERIAL4_DE,
    SMC_SERIAL4_RE,
    /*
     * POWER
     */
    SMC_5V_OUT_EN,
    SMC_PWR_PERIPHERAL_EN,
    /*
     * ALTO MULTINET
     */
    SMC_SERIAL6_DE,
    SMC_SERIAL6_RE,
    /*
     * Digital Input Outpus
     */
    SMC_GPO_0,
    SMC_GPO_1,
    SMC_GPO_2,
    SMC_GPO_3,
    SMC_GPO_4,
    SMC_GPO_5,
    SMC_IRDA_TX,
    /*
     * EXTENDED INTERFACE TO MAX10 FPGA
     */
    SMC_MAX10_JAM_TMS,
    SMC_MAX10_JAM_TCK,
    SMC_MAX10_JAM_TDI,
    SMC_MAX10_CFG_SEL,
    /* REV 1.0 FORGOT TO CONNECT TO THE CPU */
//    SMC_MAX10_NCONFIG,  /* REV 1.0 FORGOT TO CONNECT TO THE CPU */
    SMC_MAX10_AUX_OUT_0,
    SMC_MAX10_AUX_OUT_1,
    SMC_MAX10_AUX_OUT_2,
    SMC_MAX10_AUX_OUT_3,
    SMC_MAX10_AUX_OUT_4,
    SMC_MAX10_AUX_OUT_5,
    SMC_MAX10_AUX_OUT_6,
    SMC_MAX10_AUX_OUT_7,
    SMC_MAX10_AUX_OUT_8,
    SMC_MAX10_AUX_OUT_9,
    SMC_MAX10_AUX_OUT_10,
    SMC_MAX10_AUX_OUT_11,
    SMC_MAX10_AUX_OUT_12,
    SMC_MAX10_AUX_OUT_13,
    SMC_MAX10_AUX_OUT_14,
    SMC_MAX10_AUX_OUT_15,
    SMC_MAX10_AUX_OUT_16,
    SMC_MAX10_AUX_OUT_17,
    SMC_MAX10_AUX_OUT_18,
    SMC_MAX10_AUX_OUT_19,
    SMC_BLE_MRDY,
    SMC_BLE_NRESET,
    SMC_BLE_1OE,
    SMC_BLE_2OE,
    /*
     * MCU
     */
    SMC_FLASH_CS,
    SMC_FLASH_WP,
    SMC_FLASH_RESET,

    SMC_DAC_FSS,

    MSP_EXP432E401Y_GPIOCOUNT,
} MSP_EXP432E401Y_GPIOName;

#define MSP_EXP432E401Y_SDSPI_CS    MSP_EXP432E401Y_GPIOCOUNT
/* Sharp LCD Pins */
#define MSP_EXP432E401Y_LCD_CS      MSP_EXP432E401Y_GPIOCOUNT
#define MSP_EXP432E401Y_LCD_POWER   MSP_EXP432E401Y_GPIOCOUNT
#define MSP_EXP432E401Y_LCD_ENABLE  MSP_EXP432E401Y_GPIOCOUNT

/*!
 *  @def    MSP_EXP432E401Y_I2CName
 *  @brief  Enum of I2C names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_I2CName {
    MSP_EXP432E401Y_I2C0 = 0,
    MSP_EXP432E401Y_I2C7,

    MSP_EXP432E401Y_I2CCOUNT
} MSP_EXP432E401Y_I2CName;

/*!
 *  @def    MSP_EXP432E401Y_NVSName
 *  @brief  Enum of NVS names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_NVSName {
    MSP_EXP432E401Y_NVSMSP432E40 = 0,
    MSP_EXP432E401Y_EXT_FLASH,

    MSP_EXP432E401Y_NVSCOUNT
} MSP_EXP432E401Y_NVSName;

/*!
 *  @def    MSP_EXP432E401Y_PWMName
 *  @brief  Enum of PWM names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_PWMName {
    MSP_EXP432E401Y_PWM0 = 0,

    MSP_EXP432E401Y_PWMCOUNT
} MSP_EXP432E401Y_PWMName;

/*!
 *  @def    MSP_EXP432E401Y_SDFatFSName
 *  @brief  Enum of SDFatFS names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_SDFatFSName {
    MSP_EXP432E401Y_SDFatFS0 = 0,

    MSP_EXP432E401Y_SDFatFSCOUNT
} MSP_EXP432E401Y_SDFatFSName;

/*!
 *  @def    MSP_EXP432E401Y_SDName
 *  @brief  Enum of SD names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_SDName {
    MSP_EXP432E401Y_SDSPI0 = 0,

    MSP_EXP432E401Y_SDCOUNT
} MSP_EXP432E401Y_SDName;

/*!
 *  @def    MSP_EXP432E401Y_SHA2Name
 *  @brief  Enum of SHA2 driver instance names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_SHA2Name {
    MSP_EXP432E401Y_SHA20 = 0,

    MSP_EXP432E401Y_SHA2COUNT
} MSP_EXP432E401Y_SHA2Name;

/*!
 *  @def    MSP_EXP432E401Y_SPIName
 *  @brief  Enum of SPI names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_SPIName {
    MSP_EXP432E401Y_SPI2 = 0,
    MSP_EXP432E401Y_SPI3,
    MSP_EXP432E401Y_SPI0,
    MSP_EXP432E401Y_SPI1,

    MSP_EXP432E401Y_SPICOUNT
} MSP_EXP432E401Y_SPIName;

/*!
 *  @def    MSP_EXP432E401Y_TimerName
 *  @brief  Enum of Timer names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_TimerName {
    MSP_EXP432E401Y_TIMER0 = 0,
    MSP_EXP432E401Y_TIMER1,
    MSP_EXP432E401Y_TIMER2,

    MSP_EXP432E401Y_TIMERCOUNT
} MSP_EXP432E401Y_TimerName;

/*!
 *  @def    MSP_EXP432E401Y_UARTName
 *  @brief  Enum of UARTs on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_UARTName {
    MSP_EXP432E401Y_UART0 = 0,
    MSP_EXP432E401Y_UART1,
    MSP_EXP432E401Y_UART2,
    MSP_EXP432E401Y_UART3,
    MSP_EXP432E401Y_UART4,
    MSP_EXP432E401Y_UART5,
    MSP_EXP432E401Y_UART6,
    MSP_EXP432E401Y_UART7,

    MSP_EXP432E401Y_UARTCOUNT
} MSP_EXP432E401Y_UARTName;

/*
 *  @def    MSP_EXP432E401Y_WatchdogName
 *  @brief  Enum of Watchdogs on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_WatchdogName {
    MSP_EXP432E401Y_WATCHDOG0 = 0,

    MSP_EXP432E401Y_WATCHDOGCOUNT
} MSP_EXP432E401Y_WatchdogName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 *  This includes:
 *     - Enable clock sources for peripherals
 */
extern void MSP_EXP432E401Y_initGeneral(void);

#ifdef __cplusplus
}
#endif

#endif /* __MSP_EXP432E401Y_H */
