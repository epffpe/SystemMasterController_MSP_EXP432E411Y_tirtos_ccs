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

/*
 *  ======== MSP_EXP432E401Y.c ========
 *  This file is responsible for setting up the board specific items for the
 *  MSP_EXP432E401Y board.
 */
#include <stdint.h>
#include <stdlib.h>

#ifndef __MSP432E411Y__
#define __MSP432E411Y__
#endif

#include <ti/devices/msp432e4/inc/msp432.h>

#include <ti/devices/msp432e4/driverlib/adc.h>
#include <ti/devices/msp432e4/driverlib/interrupt.h>
#include <ti/devices/msp432e4/driverlib/pwm.h>
#include <ti/devices/msp432e4/driverlib/sysctl.h>
#include <ti/devices/msp432e4/driverlib/udma.h>

#include <ti/drivers/Power.h>

#include "MSP_EXP432E401Y.h"

/*
 *  =============================== ADC ===============================
 */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCMSP432E4.h>

/* ADC objects */
ADCMSP432E4_Object adcMSP432E4Objects[MSP_EXP432E401Y_ADCCOUNT];

/* ADC configuration structure */
const ADCMSP432E4_HWAttrsV1 adcMSP432E4HWAttrs[MSP_EXP432E401Y_ADCCOUNT] = {
    {
        .adcPin = ADCMSP432E4_PE_3_A0,
        .refVoltage = ADCMSP432E4_VREF_INTERNAL,
        .adcModule = ADCMSP432E4_MOD0,
        .adcSeq = ADCMSP432E4_SEQ0
    },
    {
        .adcPin = ADCMSP432E4_PE_2_A1,
        .refVoltage = ADCMSP432E4_VREF_INTERNAL,
        .adcModule = ADCMSP432E4_MOD1,
        .adcSeq = ADCMSP432E4_SEQ0
    }
};

const ADC_Config ADC_config[MSP_EXP432E401Y_ADCCOUNT] = {
    {
        .fxnTablePtr = &ADCMSP432E4_fxnTable,
        .object = &adcMSP432E4Objects[MSP_EXP432E401Y_ADC0],
        .hwAttrs = &adcMSP432E4HWAttrs[MSP_EXP432E401Y_ADC0]
    },
    {
        .fxnTablePtr = &ADCMSP432E4_fxnTable,
        .object = &adcMSP432E4Objects[MSP_EXP432E401Y_ADC1],
        .hwAttrs = &adcMSP432E4HWAttrs[MSP_EXP432E401Y_ADC1]
    }
};

const uint_least8_t ADC_count = MSP_EXP432E401Y_ADCCOUNT;

/*
 *  =============================== ADCBuf ===============================
 */
#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufMSP432E4.h>

/* ADC objects */
ADCBufMSP432E4_Object adcbufMSP432E4Objects[MSP_EXP432E401Y_ADCBUFCOUNT];

ADCBufMSP432E4_Channels adcBuf0MSP432E4Channels[MSP_EXP432E401Y_ADCBUF0CHANNELCOUNT] = {
    {
        .adcPin = ADCBufMSP432E4_PE_3_A0,
        .adcSequence = ADCBufMSP432E4_Seq_0,
        .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
        .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
        .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
        .refVoltage = 3300000
    },
    {
        .adcPin = ADCBufMSP432E4_PE_2_A1,
        .adcSequence = ADCBufMSP432E4_Seq_1,
        .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
        .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
        .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
        .refVoltage = 3300000
    },
    {
        .adcPin = ADCBufMSP432E4_PIN_NONE,
        .adcSequence = ADCBufMSP432E4_Seq_0,
        .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
        .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
        .adcInternalSource = ADCBufMSP432E4_TEMPERATURE_MODE,
        .refVoltage = 3300000
    },
    {
        .adcPin = ADCBufMSP432E4_PE_3_A0,
        .adcSequence = ADCBufMSP432E4_Seq_0,
        .adcInputMode = ADCBufMSP432E4_DIFFERENTIAL,
        .adcDifferentialPin = ADCBufMSP432E4_PE_2_A1,
        .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
        .refVoltage = 3300000
    },
    {
        .adcPin = ADCBufMSP432E4_PE_2_A1,
        .adcSequence = ADCBufMSP432E4_Seq_0,
        .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
        .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
        .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
        .refVoltage = 3300000
    }
};

/* ADC sequencer priorities for SS0-SS3, set to 0-3 to initialize sequencer */
static ADCBufMSP432E4_SequencePriorities seqPriorities[ADCBufMSP432E4_SEQUENCER_COUNT] = {
    ADCBufMSP432E4_Priority_0,
    ADCBufMSP432E4_Seq_Disable,
    ADCBufMSP432E4_Seq_Disable,
    ADCBufMSP432E4_Seq_Disable
};

/* ADC sequencer tigger source for SS0-SS3*/
static ADCBufMSP432E4_TriggerSource triggerSource[ADCBufMSP432E4_SEQUENCER_COUNT] = {
    ADCBufMSP432E4_TIMER_TRIGGER,
    ADCBufMSP432E4_TIMER_TRIGGER,
    ADCBufMSP432E4_TIMER_TRIGGER,
    ADCBufMSP432E4_TIMER_TRIGGER
};

/* ADC configuration structure */
const ADCBufMSP432E4_HWAttrsV1 adcbufMSP432E4HWAttrs[MSP_EXP432E401Y_ADCBUFCOUNT] = {
    {
        .intPriority = ~0,
        .adcBase = ADC0_BASE,
        .channelSetting = adcBuf0MSP432E4Channels,
        .sequencePriority = seqPriorities,
        .adcTriggerSource = triggerSource,
        .modulePhase =  ADCBufMSP432E4_Phase_Delay_0,
        .refSource = ADCBufMSP432E4_VREF_INTERNAL,
        .useDMA = 1,
        .adcTimerSource = TIMER1_BASE,
    }
};

const ADCBuf_Config ADCBuf_config[MSP_EXP432E401Y_ADCBUFCOUNT] = {
    {
        .fxnTablePtr = &ADCBufMSP432E4_fxnTable,
        .object = &adcbufMSP432E4Objects[MSP_EXP432E401Y_ADCBUF0],
        .hwAttrs = &adcbufMSP432E4HWAttrs[MSP_EXP432E401Y_ADCBUF0]
    }
};

const uint_least8_t ADCBuf_count = MSP_EXP432E401Y_ADCBUFCOUNT;

/*
 *  =============================== CAN ===============================
 */
#include <ti/drivers/CAN.h>
#include <ti/drivers/can/CANMSP432E4.h>

/*!
 * @brief PT0 is used for CAN0 RX
 */
#define CANMSP432E4_PT0_CAN0RX GPIOMSP432E4_pinConfigMask(GPIOMSP432E4_PORTA, 0, GPIO_PT0_CAN0RX)

/*!
 * @brief PT1 is used for CAN0 TX
 */
#define CANMSP432E4_PT1_CAN0TX GPIOMSP432E4_pinConfigMask(GPIOMSP432E4_PORTA, 1, GPIO_PT1_CAN0TX)

/*!
 * @brief PT2 is used for CAN1 RX
 */
#define CANMSP432E4_PT2_CAN1RX GPIOMSP432E4_pinConfigMask(GPIOMSP432E4_PORTB, 0, GPIO_PT2_CAN1RX)

/*!
 * @brief PT3 is used for CAN1 TX
 */
#define CANMSP432E4_PT3_CAN1TX GPIOMSP432E4_pinConfigMask(GPIOMSP432E4_PORTB, 1, GPIO_PT3_CAN1TX)


CANMSP432E4_Object canMSP432E4Objects[MSP_EXP432E401Y_CANCOUNT];
CAN_Frame canMSP432E4RxBuffer[MSP_EXP432E401Y_CANCOUNT][4];
CAN_Frame canMSP432E4TxBuffer[MSP_EXP432E401Y_CANCOUNT][4];

/* CAN configuration structure */
const CANMSP432E4_HWAttrs canMSP432E4HWAttrs[MSP_EXP432E401Y_CANCOUNT] = {
    {
        .baseAddr = CAN0_BASE,
        .intNum = INT_CAN0,
        .intPriority = (~0),
        .rxPin = CANMSP432E4_PT0_CAN0RX,
        .txPin = CANMSP432E4_PT1_CAN0TX,
        .baudRate = 125000,
        .errorFxn = NULL
    },
    {
        .baseAddr = CAN1_BASE,
        .intNum = INT_CAN1,
        .intPriority = (~0),
        .rxPin = CANMSP432E4_PT2_CAN1RX,
        .txPin = CANMSP432E4_PT3_CAN1TX,
        .baudRate = 125000,
        .errorFxn = NULL
    }
};

const CAN_Config CAN_config[MSP_EXP432E401Y_CANCOUNT] = {
    {
        .fxnTablePtr = &CANMSP432E4_fxnTable,
        .object = &canMSP432E4Objects[MSP_EXP432E401Y_CAN0],
        .hwAttrs = &canMSP432E4HWAttrs[MSP_EXP432E401Y_CAN0],
        .rxBufPtr = canMSP432E4RxBuffer[MSP_EXP432E401Y_CAN0],
        .txBufPtr = canMSP432E4TxBuffer[MSP_EXP432E401Y_CAN0],
        .rxBufSize = sizeof(canMSP432E4RxBuffer[MSP_EXP432E401Y_CAN0]),
        .txBufSize = sizeof(canMSP432E4TxBuffer[MSP_EXP432E401Y_CAN0]),
    },
    {
        .fxnTablePtr = &CANMSP432E4_fxnTable,
        .object = &canMSP432E4Objects[MSP_EXP432E401Y_CAN1],
        .hwAttrs = &canMSP432E4HWAttrs[MSP_EXP432E401Y_CAN1],
        .rxBufPtr = canMSP432E4RxBuffer[MSP_EXP432E401Y_CAN1],
        .txBufPtr = canMSP432E4TxBuffer[MSP_EXP432E401Y_CAN1],
        .rxBufSize = sizeof(canMSP432E4RxBuffer[MSP_EXP432E401Y_CAN1]),
        .txBufSize = sizeof(canMSP432E4TxBuffer[MSP_EXP432E401Y_CAN1]),
    }
};

const uint_least8_t CAN_count = MSP_EXP432E401Y_CANCOUNT;

/*
 *  ============================= Display =============================
 */
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>
#include <ti/display/DisplaySharp.h>
#include "BoardRev10.h"
#define MAXPRINTLEN 1024

/* This value can be changed to 96 for use with the 430BOOST-SHARP96 BoosterPack. */
#define BOARD_DISPLAY_SHARP_SIZE    128

DisplayUart_Object displayUartObject;
DisplaySharp_Object    displaySharpObject;

static char displayBuf[MAXPRINTLEN];
static uint_least8_t sharpDisplayBuf[BOARD_DISPLAY_SHARP_SIZE * BOARD_DISPLAY_SHARP_SIZE / 8];


const DisplayUart_HWAttrs displayUartHWAttrs = {
    .uartIdx = Board_Serial0,
    .baudRate = 115200,
    .mutexTimeout = (unsigned int)(-1),
    .strBuf = displayBuf,
    .strBufLen = MAXPRINTLEN
};

const DisplaySharp_HWAttrsV1 displaySharpHWattrs = {
    .spiIndex    = MSP_EXP432E401Y_SPI2,
    .csPin       = MSP_EXP432E401Y_LCD_CS,
    .powerPin    = MSP_EXP432E401Y_LCD_POWER,
    .enablePin   = MSP_EXP432E401Y_LCD_ENABLE,
    .pixelWidth  = BOARD_DISPLAY_SHARP_SIZE,
    .pixelHeight = BOARD_DISPLAY_SHARP_SIZE,
    .displayBuf  = sharpDisplayBuf,
};

#ifndef BOARD_DISPLAY_USE_UART
#define BOARD_DISPLAY_USE_UART 1
#endif
#ifndef BOARD_DISPLAY_USE_UART_ANSI
#define BOARD_DISPLAY_USE_UART_ANSI 0
#endif
#ifndef BOARD_DISPLAY_USE_LCD
#define BOARD_DISPLAY_USE_LCD 0
#endif

/*
 * This #if/#else is needed to workaround a problem with the
 * IAR compiler. The IAR compiler doesn't like the empty array
 * initialization. (IAR Error[Pe1345])
 */
 #if (BOARD_DISPLAY_USE_UART || BOARD_DISPLAY_USE_LCD)
const Display_Config Display_config[] = {
    {
#  if (BOARD_DISPLAY_USE_UART_ANSI)
        .fxnTablePtr = &DisplayUartAnsi_fxnTable,
#  else /* Default to minimal UART with no cursor placement */
        .fxnTablePtr = &DisplayUartMin_fxnTable,
#  endif
        .object = &displayUartObject,
        .hwAttrs = &displayUartHWAttrs
    },
#endif
#if (BOARD_DISPLAY_USE_LCD)
    {
        .fxnTablePtr = &DisplaySharp_fxnTable,
        .object      = &displaySharpObject,
        .hwAttrs     = &displaySharpHWattrs
    },
#endif
};

const uint_least8_t Display_count = sizeof(Display_config) / sizeof(Display_Config);

/*
 *  =============================== DMA ===============================
 */
#include <ti/drivers/dma/UDMAMSP432E4.h>

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#endif
static tDMAControlTable dmaControlTable[64];

/*
 *  ======== dmaErrorFxn ========
 *  This is the handler for the uDMA error interrupt.
 */
static void dmaErrorFxn(uintptr_t arg)
{
    int status = uDMAErrorStatusGet();
    uDMAErrorStatusClear();

    /* Suppress unused variable warning */
    (void)status;

    while (1);
}

UDMAMSP432E4_Object udmaMSP432E4Object;

const UDMAMSP432E4_HWAttrs udmaMSP432E4HWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn = (UDMAMSP432E4_ErrorFxn)dmaErrorFxn,
    .intNum = INT_UDMAERR,
    .intPriority = (~0)
};

const UDMAMSP432E4_Config UDMAMSP432E4_config = {
    .object = &udmaMSP432E4Object,
    .hwAttrs = &udmaMSP432E4HWAttrs
};

/*
 *  =============================== General ===============================
 */
/*
 *  ======== MSP_EXP432E401Y_initGeneral ========
 */
void MSP_EXP432E401Y_initGeneral(void)
{
    Power_init();

    /* Grant the DMA access to all FLASH memory */
    FLASH_CTRL->PP |= FLASH_PP_DFA;

    /* Region start address - match FLASH start address */
    FLASH_CTRL->DMAST = 0x00000000;

    /*
     * Access to FLASH is granted to the DMA in 2KB regions.  The value
     * assigned to DMASZ is the amount of 2KB regions to which the DMA will
     * have access.  The value can be determined via the following:
     *     2 * (num_regions + 1) KB
     *
     * To grant full access to entire 1MB of FLASH:
     *     2 * (511 + 1) KB = 1024 KB (1 MB)
     */
    FLASH_CTRL->DMASZ = 511;
}

/*
 *  =============================== EMAC ===============================
 */
/* minimize scope of ndk/inc/stk_main.h -- this eliminates TIPOSIX path dependencies */
#define NDK_NOUSERAPIS 1
#include <ti/drivers/emac/EMACMSP432E4.h>

/*
 *  Required by the Networking Stack (NDK). This array must be NULL terminated.
 *  This can be removed if NDK is not used.
 *  Double curly braces are needed to avoid GCC bug #944572
 *  https://bugs.launchpad.net/gcc-linaro/+bug/944572
 */
NIMU_DEVICE_TABLE_ENTRY NIMUDeviceTable[2] = {
    {
        /* Default: use Ethernet driver */
        .init = EMACMSP432E4_NIMUInit
    },
    {NULL}
};

/*
 *  Ethernet MAC address
 *  NOTE: By default (i.e. when each octet is 0xff), the driver reads the MAC
 *        address that's stored in flash. To override this behavior, manually
 *        set the octets of the MAC address you wish to use into the array here:
 */
unsigned char macAddress[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
//unsigned char macAddress[6] = {0x55, 0xbf, 0x03, 0xB6, 0x1A, 00};

/* EMAC configuration structure */
const EMACMSP432E4_HWAttrs EMACMSP432E4_hwAttrs = {
    .baseAddr = EMAC0_BASE,
    .intNum = INT_EMAC0,
    .intPriority = (~0),
    .led0Pin = EMACMSP432E4_PK4_EN0LED0, //EMACMSP432E4_PF0_EN0LED0,
    .led1Pin = EMACMSP432E4_PK6_EN0LED1, //EMACMSP432E4_PF4_EN0LED1,
    .macAddress = macAddress
};

/*
 *  =============================== GPIO ===============================
 */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432E4.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432E401Y.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* Input pins */
//    /* MSP_EXP432E401Y_USR_SW1 */
//    GPIOMSP432E4_PJ0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
//    /* MSP_EXP432E401Y_USR_SW2 */
//    GPIOMSP432E4_PJ1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
//
//    /* MSP_EXP432E401Y_SPI_MASTER_READY */
//    GPIOMSP432E4_PM3 | GPIO_DO_NOT_CONFIG,
//    /* MSP_EXP432E401Y_SPI_SLAVE_READY */
//    GPIOMSP432E4_PL0 | GPIO_DO_NOT_CONFIG,
//
//    /* Output pins */
//    /* MSP_EXP432E401Y_USR_D1 */
//    GPIOMSP432E4_PN1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
//    /* MSP_EXP432E401Y_USR_D2 */
//    GPIOMSP432E4_PN0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
//
//    /* MSP_EXP432E401Y_SDSPI_CS */
//    GPIOMSP432E4_PC7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
//
//    /* Sharp Display - GPIO configurations will be done in the Display files */
//    GPIOMSP432E4_PE5 | GPIO_DO_NOT_CONFIG, /* SPI chip select */
//    GPIOMSP432E4_PC6 | GPIO_DO_NOT_CONFIG, /* LCD power control */
//    GPIOMSP432E4_PE4 | GPIO_DO_NOT_CONFIG, /*LCD enable */

    /* SMC_5V_OUT_STAT */
    GPIOMSP432E4_PF7 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
    /* SMC_INTERNAL_5V_SW_STATUS */
    GPIOMSP432E4_PJ2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
    /* SMC_IRDA_RX */
    GPIOMSP432E4_PL4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_TEMP_ALERT */
    GPIOMSP432E4_PP5 | GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_RISING,
    /* SMC_GPI_0 */
    GPIOMSP432E4_PM0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_GPI_1 */
    GPIOMSP432E4_PM1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_GPI_2 */
    GPIOMSP432E4_PL2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_GPI_3 */
    GPIOMSP432E4_PJ4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_GPI_4 */
    GPIOMSP432E4_PM3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_GPI_5 */
    GPIOMSP432E4_PM2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
    /* SMC_MAX10_JAM_TDO */
    GPIOMSP432E4_PH4 | GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_0 */
    GPIOMSP432E4_PH0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_1 */
    GPIOMSP432E4_PH3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_2 */
    GPIOMSP432E4_PH2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_3 */
    GPIOMSP432E4_PH1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_4 */
    GPIOMSP432E4_PC4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_5 */
    GPIOMSP432E4_PK3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_6 */
    GPIOMSP432E4_PK2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_7 */
    GPIOMSP432E4_PK1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_8 */
    GPIOMSP432E4_PB7 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_9 */
    GPIOMSP432E4_PB6 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_10 */
    GPIOMSP432E4_PE7 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_11 */
    GPIOMSP432E4_PN5 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_12 */
    GPIOMSP432E4_PN3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_13 */
    GPIOMSP432E4_PN4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_14 */
    GPIOMSP432E4_PP2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_15 */
    GPIOMSP432E4_PS3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_16 */
    GPIOMSP432E4_PS2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_17 */
    GPIOMSP432E4_PR3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_18 */
    GPIOMSP432E4_PS4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_MAX10_AUX_IN_19 */
    GPIOMSP432E4_PR6 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* SMC_BLE_SRDY */
    GPIOMSP432E4_PE2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
    /* SMC_ADC_SSTRB */
    GPIOMSP432E4_PE6 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,

    /* Output pins */
    /* SMC_UART_DEBUG */
    GPIOMSP432E4_PN1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_SERIAL5_EN_ */
    GPIOMSP432E4_PN2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
#ifdef __ETH_LED_AS_GPIO
    /* SMC_LED_D10 */
    GPIOMSP432E4_PK5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    /* SMC_LED_D11 */
    GPIOMSP432E4_PK6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
    /* SMC_LED_D12 */
    GPIOMSP432E4_PK4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
#endif
    /* SMC_LED_D6 */
    GPIOMSP432E4_PL5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
    /* SMC_LED_D20 */
    GPIOMSP432E4_PL1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
    /* SMC_USB0EPEN */
    GPIOMSP432E4_PD6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL0_DE */
    GPIOMSP432E4_PM7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL0_RE */
    GPIOMSP432E4_PM4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_SERIAL1_DE */
    GPIOMSP432E4_PQ6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL1_RE */
    GPIOMSP432E4_PS5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_SERIAL2_DE */
    GPIOMSP432E4_PG6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL2_RE */
    GPIOMSP432E4_PM6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_SERIAL3_DE */
    GPIOMSP432E4_PN7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL3_RE */
    GPIOMSP432E4_PR7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_SERIAL4_DE */
    GPIOMSP432E4_PF2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL4_RE */
    GPIOMSP432E4_PF1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_5V_OUT_EN */
    GPIOMSP432E4_PS6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_PWR_PERIPHERAL_EN */
    GPIOMSP432E4_PM5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL6_DE */
    GPIOMSP432E4_PS1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_SERIAL6_RE */
    GPIOMSP432E4_PF6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,
    /* SMC_GPO_0 */
    GPIOMSP432E4_PM0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_GPO_1 */
    GPIOMSP432E4_PM1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_GPO_2 */
    GPIOMSP432E4_PL2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_GPO_3 */
    GPIOMSP432E4_PJ4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_GPO_4 */
    GPIOMSP432E4_PM3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_GPO_5 */
    GPIOMSP432E4_PM2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_IRDA_TX */
    GPIOMSP432E4_PL3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_JAM_TMS */
    GPIOMSP432E4_PN0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_JAM_TCK */
    GPIOMSP432E4_PD0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_JAM_TDI */
    GPIOMSP432E4_PP3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_CFG_SEL */
    GPIOMSP432E4_PH0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_NCONFIG NOT CONNECTED TO THE CPU ON REV 1.0 */
    //    GPIOMSP432E4_PM_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_0 */
    GPIOMSP432E4_PH5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_1 */
    GPIOMSP432E4_PH3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_2 */
    GPIOMSP432E4_PH2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_3 */
    GPIOMSP432E4_PH1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_4 */
    GPIOMSP432E4_PC4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_5 */
    GPIOMSP432E4_PK3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_6 */
    GPIOMSP432E4_PK2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_7 */
    GPIOMSP432E4_PK1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_8 */
    GPIOMSP432E4_PB7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_9 */
    GPIOMSP432E4_PB6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_10 */
    GPIOMSP432E4_PE7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_11 */
    GPIOMSP432E4_PN5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_12 */
    GPIOMSP432E4_PN3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_13 */
    GPIOMSP432E4_PN4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_14 */
    GPIOMSP432E4_PP2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_15 */
    GPIOMSP432E4_PS3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_16 */
    GPIOMSP432E4_PS2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_17 */
    GPIOMSP432E4_PR3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_18 */
    GPIOMSP432E4_PS4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_MAX10_AUX_OUT_19 */
    GPIOMSP432E4_PR6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_BLE_MRDY */
    GPIOMSP432E4_PE3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_BLE_NRESET */
    GPIOMSP432E4_PJ6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_BLE_1OE */
    GPIOMSP432E4_PR5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_BLE_2OE */
    GPIOMSP432E4_PR2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_FLASH_WP */
    GPIOMSP432E4_PR2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW,
    /* SMC_FLASH_RESET */
    GPIOMSP432E4_PR2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH,

};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432E401Y.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    NULL,  /* SMC_5V_OUT_STAT */
    NULL,  /* SMC_INTERNAL_5V_SW_STATUS */
    NULL,   /* SMC_IRDA_RX */
    NULL,   /* SMC_TEMP_ALERT */
    NULL,   /* SMC_GPI_0 */
    NULL,   /* SMC_GPI_1 */
    NULL,   /* SMC_GPI_2 */
    NULL,   /* SMC_GPI_3 */
    NULL,   /* SMC_GPI_4 */
    NULL,   /* SMC_GPI_5 */
    NULL,   /* SMC_MAX10_JAM_TDO */
    NULL,   /* SMC_MAX10_AUX_IN_0 */
    NULL,   /* SMC_MAX10_AUX_IN_1 */
    NULL,   /* SMC_MAX10_AUX_IN_2 */
    NULL,   /* SMC_MAX10_AUX_IN_3 */
    NULL,   /* SMC_MAX10_AUX_IN_4 */
    NULL,   /* SMC_MAX10_AUX_IN_5 */
    NULL,   /* SMC_MAX10_AUX_IN_6 */
    NULL,   /* SMC_MAX10_AUX_IN_7 */
    NULL,   /* SMC_MAX10_AUX_IN_8 */
    NULL,   /* SMC_MAX10_AUX_IN_9 */
    NULL,   /* SMC_MAX10_AUX_IN_10 */
    NULL,   /* SMC_MAX10_AUX_IN_11 */
    NULL,   /* SMC_MAX10_AUX_IN_12 */
    NULL,   /* SMC_MAX10_AUX_IN_13 */
    NULL,   /* SMC_MAX10_AUX_IN_14 */
    NULL,   /* SMC_MAX10_AUX_IN_15 */
    NULL,   /* SMC_MAX10_AUX_IN_16 */
    NULL,   /* SMC_MAX10_AUX_IN_17 */
    NULL,   /* SMC_MAX10_AUX_IN_18 */
    NULL,   /* SMC_MAX10_AUX_IN_19 */
    NULL,   /* SMC_BLE_SRDY */
};

/* The device-specific GPIO_config structure */
const GPIOMSP432E4_Config GPIOMSP432E4_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  =============================== I2C ===============================
 */
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CMSP432E4.h>

I2CMSP432E4_Object i2cMSP432E4Objects[MSP_EXP432E401Y_I2CCOUNT];

const I2CMSP432E4_HWAttrs i2cMSP432E4HWAttrs[MSP_EXP432E401Y_I2CCOUNT] = {
    {
        .baseAddr = I2C0_BASE,
        .intNum = INT_I2C0,
        .intPriority = (~0),
        .sclPin = I2CMSP432E4_PB2_I2C0SCL,
        .sdaPin = I2CMSP432E4_PB3_I2C0SDA,
        .masterCode = 0x08
    },
    {
        .baseAddr = I2C7_BASE,
        .intNum = INT_I2C7,
        .intPriority = (~0),
        .sclPin = I2CMSP432E4_PD0_I2C7SCL,
        .sdaPin = I2CMSP432E4_PD1_I2C7SDA,
        .masterCode = 0x09
    }
};

const I2C_Config I2C_config[MSP_EXP432E401Y_I2CCOUNT] = {
    {
        .fxnTablePtr = &I2CMSP432E4_fxnTable,
        .object = &i2cMSP432E4Objects[MSP_EXP432E401Y_I2C0],
        .hwAttrs = &i2cMSP432E4HWAttrs[MSP_EXP432E401Y_I2C0]
    },
    {
        .fxnTablePtr = &I2CMSP432E4_fxnTable,
        .object = &i2cMSP432E4Objects[MSP_EXP432E401Y_I2C7],
        .hwAttrs = &i2cMSP432E4HWAttrs[MSP_EXP432E401Y_I2C7]
    },
};

const uint_least8_t I2C_count = MSP_EXP432E401Y_I2CCOUNT;

/*
 *  =============================== NVS ===============================
 */
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSMSP432E4.h>

#define SECTORSIZE       (0x4000)
#define NVS_REGIONS_BASE (0xE0000)
#define REGIONSIZE       (SECTORSIZE * 8)

/*
 * Reserve flash sectors for NVS driver use
 * by placing an uninitialized byte array
 * at the desired flash address.
 */
#if defined(__TI_COMPILER_VERSION__)

/*
 * Place uninitialized array at NVS_REGIONS_BASE
 */
#pragma LOCATION(flashBuf, NVS_REGIONS_BASE);
#pragma NOINIT(flashBuf);
static char flashBuf[REGIONSIZE];

#elif defined(__IAR_SYSTEMS_ICC__)

/*
 * Place uninitialized array at NVS_REGIONS_BASE
 */
static __no_init char flashBuf[REGIONSIZE] @ NVS_REGIONS_BASE;

#elif defined(__GNUC__)

/*
 * Place the flash buffers in the .nvs section created in the gcc linker file.
 * The .nvs section enforces alignment on a sector boundary but may
 * be placed anywhere in flash memory.  If desired the .nvs section can be set
 * to a fixed address by changing the following in the gcc linker file:
 *
 * .nvs (FIXED_FLASH_ADDR) (NOLOAD) : AT (FIXED_FLASH_ADDR) {
 *      *(.nvs)
 * } > REGION_TEXT
 */
__attribute__ ((section (".nvs")))
static char flashBuf[REGIONSIZE];

#endif

NVSMSP432E4_Object nvsMSP432E4Objects[MSP_EXP432E401Y_NVSCOUNT];

const NVSMSP432E4_HWAttrs nvsMSP432E4HWAttrs[MSP_EXP432E401Y_NVSCOUNT] = {
    {
        .regionBase = (void *) flashBuf,
        .regionSize = REGIONSIZE,
    },
};

const NVS_Config NVS_config[MSP_EXP432E401Y_NVSCOUNT] = {
    {
        .fxnTablePtr = &NVSMSP432E4_fxnTable,
        .object = &nvsMSP432E4Objects[MSP_EXP432E401Y_NVSMSP432E40],
        .hwAttrs = &nvsMSP432E4HWAttrs[MSP_EXP432E401Y_NVSMSP432E40],
    },
};

const uint_least8_t NVS_count = MSP_EXP432E401Y_NVSCOUNT;

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/power/PowerMSP432E4.h>
const PowerMSP432E4_Config PowerMSP432E4_config = {
    .policyFxn = &PowerMSP432E4_sleepPolicy,
    .enablePolicy = true
};

/*
 *  =============================== PWM ===============================
 */
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMMSP432E4.h>

PWMMSP432E4_Object pwmMSP432E4Objects[MSP_EXP432E401Y_PWMCOUNT];

const PWMMSP432E4_HWAttrs pwmMSP432E4HWAttrs[MSP_EXP432E401Y_PWMCOUNT] = {
    {
        .pwmBaseAddr = PWM0_BASE,
        .pwmOutput = PWM_OUT_7,
        .pwmGenOpts = PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN,
        .pinConfig = PWMMSP432E4_PK5_M0PWM7
    }
};

const PWM_Config PWM_config[MSP_EXP432E401Y_PWMCOUNT] = {
    {
        .fxnTablePtr = &PWMMSP432E4_fxnTable,
        .object = &pwmMSP432E4Objects[MSP_EXP432E401Y_PWM0],
        .hwAttrs = &pwmMSP432E4HWAttrs[MSP_EXP432E401Y_PWM0]
    },
};

const uint_least8_t PWM_count = MSP_EXP432E401Y_PWMCOUNT;

/*
 *  =============================== SDFatFS ===============================
 */
#include <ti/drivers/SD.h>
#include <ti/drivers/SDFatFS.h>

/*
 * Note: The SDFatFS driver provides interface functions to enable FatFs
 * but relies on the SD driver to communicate with SD cards.  Opening a
 * SDFatFs driver instance will internally try to open a SD driver instance
 * reusing the same index number (opening SDFatFs driver at index 0 will try to
 * open SD driver at index 0).  This requires that all SDFatFs driver instances
 * have an accompanying SD driver instance defined with the same index.  It is
 * acceptable to have more SD driver instances than SDFatFs driver instances
 * but the opposite is not supported & the SDFatFs will fail to open.
 */
SDFatFS_Object sdfatfsObjects[MSP_EXP432E401Y_SDFatFSCOUNT];

const SDFatFS_Config SDFatFS_config[MSP_EXP432E401Y_SDFatFSCOUNT] = {
    {
        .object = &sdfatfsObjects[MSP_EXP432E401Y_SDFatFS0]
    }
};

const uint_least8_t SDFatFS_count = MSP_EXP432E401Y_SDFatFSCOUNT;

/*
 *  =============================== SD ===============================
 */
#include <ti/drivers/SD.h>
#include <ti/drivers/sd/SDSPI.h>

SDSPI_Object sdspiObjects[MSP_EXP432E401Y_SDCOUNT];

const SDSPI_HWAttrs sdspiHWAttrs[MSP_EXP432E401Y_SDCOUNT] = {
    {
        .spiIndex = MSP_EXP432E401Y_SPI2,
        .spiCsGpioIndex = MSP_EXP432E401Y_SDSPI_CS
    }
};

const SD_Config SD_config[MSP_EXP432E401Y_SDCOUNT] = {
    {
        .fxnTablePtr = &SDSPI_fxnTable,
        .object = &sdspiObjects[MSP_EXP432E401Y_SDSPI0],
        .hwAttrs = &sdspiHWAttrs[MSP_EXP432E401Y_SDSPI0]
    },
};

const uint_least8_t SD_count = MSP_EXP432E401Y_SDCOUNT;

/*
 *  =============================== SPI ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432E4DMA.h>

SPIMSP432E4DMA_Object spiMSP432E4DMAObjects[MSP_EXP432E401Y_SPICOUNT];

/*
 * NOTE: The SPI instances below can be used by the SD driver to communicate
 * with a SD card via SPI.  The 'defaultTxBufValue' fields below are set to
 * (~0) to satisfy the SDSPI driver requirement.
 */
const SPIMSP432E4DMA_HWAttrs spiMSP432E4DMAHWAttrs[MSP_EXP432E401Y_SPICOUNT] = {
    {
        .baseAddr = SSI2_BASE,
        .intNum = INT_SSI2,
        .intPriority = (~0),
        .defaultTxBufValue = (~0),
        .rxDmaChannel = UDMA_CH12_SSI2RX,
        .txDmaChannel = UDMA_CH13_SSI2TX,
        .minDmaTransferSize = 10,
        .clkPinMask = SPIMSP432E4_PD3_SSI2CLK,
        .fssPinMask = SPIMSP432E4_PD2_SSI2FSS,
        .xdat0PinMask = SPIMSP432E4_PD1_SSI2XDAT0,
        .xdat1PinMask = SPIMSP432E4_PD0_SSI2XDAT1
    },
    {
        .baseAddr = SSI3_BASE,
        .intNum = INT_SSI3,
        .intPriority = (~0),
        .defaultTxBufValue = (~0),
        .minDmaTransferSize = 10,
        .rxDmaChannel = UDMA_CH14_SSI3RX,
        .txDmaChannel = UDMA_CH15_SSI3TX,
        .clkPinMask = SPIMSP432E4_PQ0_SSI3CLK,
        .fssPinMask = SPIMSP432E4_PQ1_SSI3FSS,
        .xdat0PinMask = SPIMSP432E4_PQ2_SSI3XDAT0,
        .xdat1PinMask = SPIMSP432E4_PQ3_SSI3XDAT1
    }
};

const SPI_Config SPI_config[MSP_EXP432E401Y_SPICOUNT] = {
    {
        .fxnTablePtr = &SPIMSP432E4DMA_fxnTable,
        .object = &spiMSP432E4DMAObjects[MSP_EXP432E401Y_SPI2],
        .hwAttrs = &spiMSP432E4DMAHWAttrs[MSP_EXP432E401Y_SPI2]
    },
    {
        .fxnTablePtr = &SPIMSP432E4DMA_fxnTable,
        .object = &spiMSP432E4DMAObjects[MSP_EXP432E401Y_SPI3],
        .hwAttrs = &spiMSP432E4DMAHWAttrs[MSP_EXP432E401Y_SPI3]
    },
};

const uint_least8_t SPI_count = MSP_EXP432E401Y_SPICOUNT;

/*
 *  =============================== Timer ===============================
 */
#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerMSP432E4.h>

TimerMSP432E4_Object timerMSP432E4Objects[MSP_EXP432E401Y_TIMERCOUNT];

const TimerMSP432E4_HWAttrs timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMERCOUNT] = {
    {
        .baseAddress = TIMER2_BASE,
        .subTimer = TimerMSP432E4_timer32,
        .intNum = INT_TIMER2A,
        .intPriority = ~0
    },
    {
        .baseAddress = TIMER1_BASE,
        .subTimer = TimerMSP432E4_timer16A,
        .intNum = INT_TIMER1A,
        .intPriority = ~0
    },
    {
         .baseAddress = TIMER1_BASE,
         .subTimer = TimerMSP432E4_timer16B,
         .intNum = INT_TIMER1B,
         .intPriority = ~0
    },
};

const Timer_Config Timer_config[MSP_EXP432E401Y_TIMERCOUNT] = {
    {
        .fxnTablePtr = &TimerMSP432E4_fxnTable,
        .object = &timerMSP432E4Objects[MSP_EXP432E401Y_TIMER0],
        .hwAttrs = &timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMER0]
    },
    {
        .fxnTablePtr = &TimerMSP432E4_fxnTable,
        .object = &timerMSP432E4Objects[MSP_EXP432E401Y_TIMER1],
        .hwAttrs = &timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMER1]
    },
    {
        .fxnTablePtr = &TimerMSP432E4_fxnTable,
        .object = &timerMSP432E4Objects[MSP_EXP432E401Y_TIMER2],
        .hwAttrs = &timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMER2]
    },
};

const uint_least8_t Timer_count = MSP_EXP432E401Y_TIMERCOUNT;

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432E4.h>

UARTMSP432E4_Object uartMSP432E4Objects[MSP_EXP432E401Y_UARTCOUNT];
unsigned char uartMSP432E4RingBuffer[MSP_EXP432E401Y_UARTCOUNT][128];

/* UART configuration structure */
const UARTMSP432E4_HWAttrs uartMSP432E4HWAttrs[MSP_EXP432E401Y_UARTCOUNT] = {
    {
        .baseAddr = UART0_BASE,
        .intNum = INT_UART0,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART0],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART0]),
        .rxPin = UARTMSP432E4_PA0_U0RX,
        .txPin = UARTMSP432E4_PA1_U0TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART1_BASE,
        .intNum = INT_UART1,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART1],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART1]),
        .rxPin = UARTMSP432E4_PQ4_U1RX,
        .txPin = UARTMSP432E4_PQ5_U1TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART2_BASE,
        .intNum = INT_UART2,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART2],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART2]),
        .rxPin = UARTMSP432E4_PA6_U2RX,
        .txPin = UARTMSP432E4_PA7_U2TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART3_BASE,
        .intNum = INT_UART3,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART3],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART3]),
        .rxPin = UARTMSP432E4_PJ0_U3RX,
        .txPin = UARTMSP432E4_PJ1_U3TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART4_BASE,
        .intNum = INT_UART4,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART4],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART4]),
        .rxPin = UARTMSP432E4_PR1_U4RX,
        .txPin = UARTMSP432E4_PR0_U4TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART5_BASE,
        .intNum = INT_UART5,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART5],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART5]),
        .rxPin = UARTMSP432E4_PC6_U5RX,
        .txPin = UARTMSP432E4_PC7_U5TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART6_BASE,
        .intNum = INT_UART6,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART6],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART6]),
        .rxPin = UARTMSP432E4_PP0_U6RX,
        .txPin = UARTMSP432E4_PP1_U6TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    },
    {
        .baseAddr = UART7_BASE,
        .intNum = INT_UART7,
        .intPriority = (~0),
        .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
        .ringBufPtr  = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART7],
        .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART7]),
        .rxPin = UARTMSP432E4_PH6_U7RX,
        .txPin = UARTMSP432E4_PH7_U7TX,
        .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
        .errorFxn = NULL
    }
};

const UART_Config UART_config[MSP_EXP432E401Y_UARTCOUNT] = {
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART0],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART0]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART1],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART1]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART2],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART2]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART3],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART3]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART4],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART4]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART5],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART5]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART6],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART6]
    },
    {
        .fxnTablePtr = &UARTMSP432E4_fxnTable,
        .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART7],
        .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART7]
    }
};

const uint_least8_t UART_count = MSP_EXP432E401Y_UARTCOUNT;

/*
 *  =============================== Watchdog ===============================
 */
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogMSP432E4.h>

WatchdogMSP432E4_Object watchdogMSP432E4Objects[MSP_EXP432E401Y_WATCHDOGCOUNT];

const WatchdogMSP432E4_HWAttrs watchdogMSP432E4HWAttrs[MSP_EXP432E401Y_WATCHDOGCOUNT] = {
    {
        .baseAddr = WATCHDOG0_BASE,
        .intNum = INT_WATCHDOG,
        .intPriority = (~0),
        .reloadValue = 80000000 /* 1 second period at default CPU clock freq */
    },
};

const Watchdog_Config Watchdog_config[MSP_EXP432E401Y_WATCHDOGCOUNT] = {
    {
        .fxnTablePtr = &WatchdogMSP432E4_fxnTable,
        .object = &watchdogMSP432E4Objects[MSP_EXP432E401Y_WATCHDOG0],
        .hwAttrs = &watchdogMSP432E4HWAttrs[MSP_EXP432E401Y_WATCHDOG0]
    },
};

const uint_least8_t Watchdog_count = MSP_EXP432E401Y_WATCHDOGCOUNT;
