/*
 * BootloaderInterface.c
 *
 *  Created on: May 22, 2020
 *      Author: epffpe
 */

#define _SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_GLOBAL
#include "includes.h"




//volatile uint32_t g_ui32SysClockFreq;

//*****************************************************************************
//
// Function call to jump to the boot loader.
//
//*****************************************************************************
void JumpToBootLoader(void)
{
    /* We must make sure we turn off SysTick and its interrupt before entering
     * the boot loader! */
    MAP_SysTickIntDisable();
    MAP_SysTickDisable();

    /* Disable all processor interrupts.  Instead of disabling them one at a
     * time, a direct write to NVIC is done to disable all peripheral
     * interrupts. */
    NVIC->ICER[0] = 0xffffffff;
    NVIC->ICER[1] = 0xffffffff;
    NVIC->ICER[2] = 0xffffffff;
    NVIC->ICER[3] = 0xffffffff;

    /* Return control to the boot loader.  This is a call to the SVC handler
     * in the boot loader. */
    (*((void (*)(void))(*(uint32_t *)0x2c)))();
}

////*****************************************************************************
////
//// Initialize UART0 and set the appropriate communication parameters.
////
////*****************************************************************************
//void
//SetupForUART(void)
//{
//    /* We need to make sure that UART0 and its associated GPIO port are
//     * enabled before we pass control to the boot loader.  The serial boot
//     * loader does not enable or configure these peripherals for us if we
//     * enter it via its SVC vector. */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
//
//    /* Set GPIO PA0 and PA1 as UART. */
//    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
//    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
//    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//
//    /* Configure the UART for 115200, n, 8, 1 */
//    MAP_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClockFreq, 115200,
//                            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
//                             UART_CONFIG_WLEN_8));
//
//    /* Enable the UART operation. */
//    MAP_UARTEnable(UART0_BASE);
//}
//
////*****************************************************************************
////
//// Initialize I2C0 and set the appropriate communication parameters.
////
////*****************************************************************************
//void
//SetupForI2C(void)
//{
//    /* We need to make sure that UART0 and its associated GPIO port are
//     * enabled before we pass control to the boot loader.  The serial boot
//     * loader does not enable or configure these peripherals for us if we
//     * enter it via its SVC vector. */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
//
//    /* Set GPIO PB2 and PB3 as I2C. Setting the internal weak pull up and
//     * clearing the internal weak pull down in case there is no board level
//     * pull up is not provided. This is not a preferred method but will
//     * provide some functionality. It is strongly advised to have board pull
//     * up of 1.0K to 3.3K for proper operation of the I2C bus. */
//    MAP_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
//    MAP_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
//    MAP_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
//    MAP_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
//    GPIOB->PUR |= (GPIO_PIN_3 | GPIO_PIN_2);
//    GPIOB->PDR &= !(GPIO_PIN_3 | GPIO_PIN_2);
//
//
//    /* Configure the I2C for slave mode with Slave address of 0x42 */
//    MAP_I2CSlaveInit(I2C0_BASE, 0x42);
//}
//
////*****************************************************************************
////
//// Initialize SSI0 and set the appropriate communication parameters.
////
////*****************************************************************************
//void
//SetupForSSI(void)
//{
//    /* We need to make sure that UART0 and its associated GPIO port are
//     * enabled before we pass control to the boot loader.  The serial boot
//     * loader does not enable or configure these peripherals for us if we
//     * enter it via its SVC vector. */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
//
//    /* Set GPIO PA2, PA3, PA4 and PA5 as SSI.  */
//    MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
//    MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
//    MAP_GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
//    MAP_GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
//    MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_3 |
//                                        GPIO_PIN_4 | GPIO_PIN_5));
//
//    /* Configure the SSI Slave mode with 1 Mbps data rate*/
//    MAP_SSIConfigSetExpClk(SSI0_BASE, g_ui32SysClockFreq,
//                           SSI_FRF_MOTO_MODE_0, SSI_MODE_SLAVE,
//                           1000000, 8);
//
//    /* Enable the UART operation. */
//    MAP_SSIEnable(SSI0_BASE);
//}


