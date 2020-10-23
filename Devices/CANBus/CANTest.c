/*
 * CANTest.c
 *
 *  Created on: Oct 6, 2017
 *      Author: epenate
 */

#define _CANTEST_GLOBALS
#include "includes.h"


Semaphore_Struct g_CANTestSemStruct;
Semaphore_Handle g_CANTestSemHandle;

Swi_Struct g_CANTestSwi0Struct;
Swi_Handle g_CANTestSwi0Handle;

Hwi_Handle g_CANTestCANHwi;

Event_Handle g_CANDemoEventHandle;

//*****************************************************************************
//
// A counter that keeps track of the number of times the TX & RX interrupt has
// occurred, which should match the number of messages that were transmitted /
// received.
//
//*****************************************************************************
volatile uint32_t g_ui32RXMsgCount = 0;
volatile uint32_t g_ui32TXMsgCount = 0;
volatile uint32_t g_ui32RMTMsgCount = 0;
volatile uint32_t g_ui32HostErrorCount = 0;
volatile uint32_t g_ui32TXMsgData = 0;

//*****************************************************************************
//
// A flag for the interrupt handler to indicate that a message was received.
//
//*****************************************************************************
//volatile bool g_bRXFlag = 0;

//*****************************************************************************
//
// A global to keep track of the error flags that have been thrown so they may
// be processed. This is necessary because reading the error register clears
// the flags, so it is necessary to save them somewhere for processing.
//
//*****************************************************************************
volatile uint32_t g_ui32ErrFlag = 0;

//*****************************************************************************
//
// CAN message Objects for data being sent / received
//
//*****************************************************************************
tCANMsgObject g_sCAN0RxMessage;
tCANMsgObject g_sCAN0TxMessage;

//*****************************************************************************
//
// Message Identifiers and Objects
// RXID is set to 0 so all messages are received
//
//*****************************************************************************
#define CAN0RXID                0
#define RXOBJECT                3
#define CAN0TXID                2
#define TXOBJECT                2


#define DFU_DEVICE_ID                2
#define HOST_DEVICE_ID                3

#define CANDEMO_SDO_RX                  29
#define CANDEMO_SDO_TX                  30

#define CANDEMO_RPDO1                   21
#define CANDEMO_RPDO2                   22
#define CANDEMO_RPDO3                   23
#define CANDEMO_RPDO4                   24
#define CANDEMO_TPDO1                   25
#define CANDEMO_TPDO2                   26
#define CANDEMO_TPDO3                   27
#define CANDEMO_TPDO4                   28

#define CANDEMO_HeartBeat               32

#define CANDEMO_PERIODIC_EVT                Event_Id_00
#define CANDEMO_SDO_RX_EVT                  Event_Id_29

#define CANDEMO_ALL_EVENTS                       (CANDEMO_PERIODIC_EVT        | \
                                                CANDEMO_SDO_RX_EVT)
//*****************************************************************************
//
// Variables to hold character being sent / reveived
//
//*****************************************************************************
uint8_t g_ui8TXMsgData;
uint8_t g_aui8RXMsgData[8];



//*****************************************************************************
//
// Can ERROR handling. When a message is received if there is an erro it is
// saved to g_ui32ErrFlag, the Error Flag Set. Below the flags are checked
// and cleared. It is left up to the user to add handling fuctionality if so
// desiered.
//
// For more information on the error flags please see the CAN section of the
// microcontroller datasheet.
//
// NOTE: you may experience errors during setup when only one board is powered
// on. This is caused by one board sending signals and there not being another
// board there to acknoledge it. Dont worry about these errors, they can be
// disregarded.
//
//*****************************************************************************
void
CANErrorHandler(void)
{
    System_printf("CAN ERROR: %x, file %s, line %d\n", g_ui32ErrFlag, __FILE__, __LINE__);
//    System_printf("ERROR: %x \n",g_ui32ErrFlag);
    //
    // CAN controller has entered a Bus Off state.
    //
    if(g_ui32ErrFlag & CAN_STATUS_BUS_OFF)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_BUS_OFF \n");
        System_flush();

//        //
//        // Enable the controller again to allow it to start decrementing the
//        // error counter allowing the bus off condition to clear.
//        //
//        CANEnable(CAN0_BASE);
//        //
//        // Wait for the bus off condition to clear. This condition can be
//        // polled elsewhere depending on the application. But no CAN messages
//        // can be sent until this condition clears.
//        //
//        while(CANStatusGet(CAN0_BASE, CAN_STS_CONTROL) & CAN_STATUS_BUS_OFF)
//        {
//            Task_sleep(1);
//        }

        //
        // Clear CAN_STATUS_BUS_OFF Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_BUS_OFF);
//        Task_sleep(3000);
//        CANEnable(CAN0_BASE);
//        GPIO_write(Board_LED4, 1);

    }

    //
    // CAN controller error level has reached warning level.
    //
    if(g_ui32ErrFlag & CAN_STATUS_EWARN)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_EWARN \n");
        System_flush();

        //
        // Clear CAN_STATUS_EWARN Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_EWARN);
    }

    //
    // CAN controller error level has reached error passive level.
    //
    if(g_ui32ErrFlag & CAN_STATUS_EPASS)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_EPASS \n");
        System_flush();
        //
        // Clear CAN_STATUS_EPASS Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_EPASS);
    }

    //
    // A message was received successfully since the last read of this status.
    //
    if(g_ui32ErrFlag & CAN_STATUS_RXOK)
    {
        //
        // Handle Error Condition here
        //
//        System_printf("    ERROR: CAN_STATUS_RXOK \n");
        //
        // Clear CAN_STATUS_RXOK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_RXOK);
    }

    //
    // A message was transmitted successfully since the last read of this
    // status.
    //
    if(g_ui32ErrFlag & CAN_STATUS_TXOK)
    {
        //
        // Handle Error Condition here
        //
//        System_printf("    ERROR: CAN_STATUS_TXOK \n");
        //
        // Clear CAN_STATUS_TXOK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_TXOK);
    }

    //
    // This is the mask for the last error code field.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MSK)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_MSK \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_MSK Flag
        //
//        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MSK);
    }

    //
    // A bit stuffing error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_STUFF)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_STUFF \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_STUFF Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_STUFF);
    }

    //
    // A formatting error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_FORM)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_FORM \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_FORM Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_FORM);
    }

    //
    // An acknowledge error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_ACK)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_ACK \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_ACK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_ACK);
    }

    //
    // The bus remained a bit level of 1 for longer than is allowed.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT1)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_BIT1 \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_BIT1 Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT1);
    }

    //
    // The bus remained a bit level of 0 for longer than is allowed.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT0)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_BIT0 \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_BIT0 Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT0);
    }

    //
    // A CRC error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_CRC)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_LEC_CRC \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_CRC Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_CRC);
    }

    //
    // This is the mask for the CAN Last Error Code (LEC).
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MASK)
    {
        //
        // Handle Error Condition here
        //
        System_printf("    ERROR: CAN_STATUS_EPASS \n");
        System_flush();
        //
        // Clear CAN_STATUS_LEC_MASK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MASK);
    }

    //
    // If there are any bits still set in g_ui32ErrFlag then something unhandled
    // has happened. Print the value of g_ui32ErrFlag.
    //
    if(g_ui32ErrFlag !=0)
    {
        System_printf("    Unhandled ERROR: %x \n",g_ui32ErrFlag);
    }
    System_flush();

}


//*****************************************************************************
//
// CAN 0 Interrupt Handler. It checks for the cause of the interrupt, and
// maintains a count of all messages that have been transmitted / received
//
//*****************************************************************************
Void CAN0IntHandler(UArg arg)
{
    uint32_t ui32Status;
    ASSERT(BIOS_getThreadType() == BIOS_ThreadType_Hwi);
    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    // CAN_INT_STS_CAUSE register values
    // 0x0000        = No Interrupt Pending
    // 0x0001-0x0020 = Number of message object that caused the interrupt
    // 0x8000        = Status interrupt
    // all other numbers are reserved and have no meaning in this system
    //
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    //
    // If this was a status interrupt acknowledge it by reading the CAN
    // controller status register.
    //
    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors. Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_TXREQUEST);
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_NEWDAT);
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_MSGVAL);
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        //
        // Add ERROR flags to list of current errors. To be handled
        // later, because it would take too much time here in the
        // interrupt.
        //
        g_ui32ErrFlag |= ui32Status;
    }

    //
    // Check if the cause is message object RXOBJECT, which we are using
    // for receiving messages.
    //
    else if(ui32Status == RXOBJECT)
    {

        CANIntClear(CAN0_BASE, RXOBJECT);

        g_ui32RXMsgCount++;

        //
        // Set flag to indicate received message is pending.
        //
//        g_bRXFlag = true;
//        ASSERT(Swi_enabled() == TRUE);

        Swi_inc(g_CANTestSwi0Handle);

//        Semaphore_post(semHandle);

        g_ui32ErrFlag &= ~(CAN_STATUS_RXOK);
//        GPIO_write(MRB_LED1, DO_OFF);
    }

    //
    // Check if the cause is message object TXOBJECT, which we are using
    // for transmitting messages.
    //
    else if(ui32Status == TXOBJECT)
    {
        //
        // Getting to this point means that the TX interrupt occurred on
        // message object TXOBJECT, and the message reception is complete.
        // Clear the message object interrupt.
        //
//        GPIO_write(MRB_LED2, DO_ON);
        CANIntClear(CAN0_BASE, TXOBJECT);
//        CANMessageClear(CAN0_BASE, TXOBJECT);
        //
        // Increment a counter to keep track of how many messages have been
        // transmitted. In a real application this could be used to set
        // flags to indicate when a message is transmitted.
        //
        g_ui32TXMsgCount++;

        //
        // Since a message was transmitted, clear any error flags.
        // This is done because before the message is transmitted it triggers
        // a Status Interrupt for TX complete. by clearing the flag here we
        // prevent unnecessary error handling from happeneing
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_TXOK);
//        GPIO_write(MRB_LED2, DO_OFF);
    }
    else if(ui32Status == CANDEMO_SDO_RX)
    {
        CANIntClear(CAN0_BASE, CANDEMO_SDO_RX);
//        Swi_inc(g_CANTestSwi0Handle);
        Event_post(g_CANDemoEventHandle, CANDEMO_SDO_RX_EVT);
    }

    //
    // Check if the cause is message object TXOBJECT, which we are using
    // for transmitting messages.
    //
    else if(ui32Status == CANDEMO_SDO_TX)
    {
        //
        // Getting to this point means that the TX interrupt occurred on
        // message object TXOBJECT, and the message reception is complete.
        // Clear the message object interrupt.
        //
        CANIntClear(CAN0_BASE, CANDEMO_SDO_TX);
    }

    //
    // Otherwise, something unexpected caused the interrupt.  This should
    // never happen.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }
//    Swi_restore(swiKey);
}




//*****************************************************************************
//
// Setup CAN0 to both send and receive at 500KHz.
// Interrupts on
// Use PE4 / PE5
//
//*****************************************************************************
void
InitCAN0(void)
{
//    uint32_t ui32Clk;
    Types_FreqHz cpuFreq;
    Hwi_Params hwiParams;
    Error_Block eb;

    //
    // The GPIO port and pins have been set up for CAN.  The CAN peripheral
    // must be enabled.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    //
    // Configure the GPIO pin muxing to select CAN0 functions for these pins.
    // This step selects which alternate function is available for these pins.
    //
    GPIOPinConfigure(GPIO_PT0_CAN0RX);
    GPIOPinConfigure(GPIO_PT1_CAN0TX);

    //
    // Enable the alternate function on the GPIO pins.  The above step selects
    // which alternate function is available.  This step actually enables the
    // alternate function instead of GPIO for these pins.
    //
    GPIOPinTypeCAN(GPIO_PORTT_BASE, GPIO_PIN_0 | GPIO_PIN_1);


    //
    // Initialize the CAN controller
    //
    CANInit(CAN0_BASE);

    //
    // Set up the bit rate for the CAN bus.  This function sets up the CAN
    // bus timing for a nominal configuration.  You can achieve more control
    // over the CAN bus timing by using the function CANBitTimingSet() instead
    // of this one, if needed.
    // In this example, the CAN bus is set to 500 kHz.
    //
//    ui32Clk = SysCtlClockGet();
//    CANBitRateSet(CAN0_BASE, ui32Clk, 500000);
    BIOS_getCpuFreq(&cpuFreq);
    CANBitRateSet(CAN0_BASE, cpuFreq.lo, 500000);

    Hwi_Params_init(&hwiParams);
    Error_init(&eb);

    // set the argument you want passed to your ISR function
    hwiParams.arg = 1;

    // set the event id of the peripheral assigned to this interrupt
    hwiParams.eventId = 10;

    // don't allow this interrupt to nest itself
    hwiParams.maskSetting = Hwi_MaskingOption_SELF;

    //
    // Configure interrupt 5 to invoke "myIsr".
    // Automatically enables interrupt 5 by default
    // set params.enableInt = FALSE if you want to control
    // when the interrupt is enabled using Hwi_enableInterrupt()
    //

    g_CANTestCANHwi = Hwi_create(INT_CAN0, CAN0IntHandler, &hwiParams, &eb);

    if (g_CANTestCANHwi == NULL) {
        if (Error_check(&eb)) {
            // handle the error
            System_abort("g_CANTestCANHwi create failed");
        }
    }

    //
    // Enable interrupts on the CAN peripheral.  This example uses static
    // allocation of interrupt handlers which means the name of the handler
    // is in the vector table of startup code.
    //
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    /* Enable auto-retry on CAN transmit */
//    CANRetrySet(CAN0_BASE, true);
    //
    // Enable the CAN interrupt on the processor (NVIC).
    //
    IntEnable(INT_CAN0);

    //
    // Enable the CAN for operation.
    //
    CANEnable(CAN0_BASE);

//    g_ui8TXMsgData = 0;
//    g_sCAN0TxMessage.ui32MsgID = CAN0TXID;
//    g_sCAN0TxMessage.ui32MsgIDMask = 0;
//    g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
//    g_sCAN0TxMessage.ui32MsgLen = sizeof(g_ui8TXMsgData);
//    g_sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui8TXMsgData;
}



Void canTaskFxn(UArg arg0, UArg arg1)
{
    tCANMsgObject sCAN0TxMessage, sCAN0TxMessage2;
    uint32_t ui32TXMsgData = 0;
    //
    // Initialize CAN0
    //

//    Log_info0("CAN init\n");

    //
    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    //
    g_sCAN0RxMessage.ui32MsgID = 0x7E0 | (DFU_DEVICE_ID & 0x0F);
    g_sCAN0RxMessage.ui32MsgIDMask = 0x7FF;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object RXOBJECT for receiving messages (this is not the
    //same as the CAN ID which can be any value in this example).
    //
    CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);

    g_ui32RMTMsgCount = 0x04030201;
    sCAN0TxMessage.ui32MsgID = 0x7F0 | (DFU_DEVICE_ID & 0x0F);
    sCAN0TxMessage.ui32MsgIDMask = 0x7FF;
    sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    sCAN0TxMessage.ui32MsgLen = sizeof(g_ui32RMTMsgCount);
    sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui32RMTMsgCount;

    CANMessageSet(CAN0_BASE, 5, &sCAN0TxMessage, MSG_OBJ_TYPE_RXTX_REMOTE);

    while(1) {
        sCAN0TxMessage.ui32MsgID = 0x7E0 | (HOST_DEVICE_ID & 0x0F);
        sCAN0TxMessage.ui32MsgIDMask = 0;
        sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        sCAN0TxMessage.ui32MsgLen = sizeof(ui32TXMsgData);
        sCAN0TxMessage.pui8MsgData = (uint8_t *)&ui32TXMsgData;

//        GPIO_write(MRB_LED2, DO_ON);
        CANMessageSet(CAN0_BASE, 3, &sCAN0TxMessage, MSG_OBJ_TYPE_TX);

        Task_sleep(1);

        sCAN0TxMessage2.ui32MsgID = 0x7F0 | (HOST_DEVICE_ID & 0x0F);;
        sCAN0TxMessage2.ui32MsgIDMask = 0;
        sCAN0TxMessage2.ui32Flags = MSG_OBJ_NO_FLAGS;
        sCAN0TxMessage2.ui32MsgLen = sizeof(ui32TXMsgData);
        sCAN0TxMessage2.pui8MsgData = (uint8_t *)&ui32TXMsgData;
        CANMessageSet(CAN0_BASE, 4, &sCAN0TxMessage2, MSG_OBJ_TYPE_TX_REMOTE);

        ui32TXMsgData++;

        //
        // Error Handling
        //
        if(g_ui32ErrFlag != 0)
        {
            CANErrorHandler();
        }

        Task_sleep(10);
    }
}



Void swi0Fxn(UArg arg0, UArg arg1)
{
    UInt eventType = Swi_getTrigger();
    switch (eventType) {
    case 0x1: break;
    case 0x2: break;
    case 0x3: break;
    }
    Semaphore_post(g_CANTestSemHandle);
}


Void canReplayFxn(UArg arg0, UArg arg1)
{

    while (1) {
        Semaphore_pend(g_CANTestSemHandle, BIOS_WAIT_FOREVER);
//        GPIO_write(MRB_LED1, DO_ON);
        g_sCAN0RxMessage.pui8MsgData = (uint8_t *) g_aui8RXMsgData;

        CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, 0);

        g_ui8TXMsgData = g_aui8RXMsgData[0] + 1;
        g_sCAN0TxMessage.ui32MsgID = 0x7D0 | (DFU_DEVICE_ID & 0x00F);;
        g_sCAN0TxMessage.ui32MsgIDMask = 0;
        g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
        g_sCAN0TxMessage.ui32MsgLen = sizeof(g_ui8TXMsgData);
        g_sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui8TXMsgData;
        CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX);
//        GPIO_write(MRB_LED1, DO_OFF);
    }
}


Void CANTestDUTFxn(UArg arg0, UArg arg1)
{
    tCANMsgObject sCAN0TxMessage;
    uint32_t *ui32TXMsgData;
    //
    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    //
    g_sCAN0RxMessage.ui32MsgID = 0x7E0 | (DFU_DEVICE_ID & 0x0F);
    g_sCAN0RxMessage.ui32MsgIDMask = 0x7FF;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object RXOBJECT for receiving messages (this is not the
    //same as the CAN ID which can be any value in this example).
    //
    CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);  // <-------------------------------

    g_ui32RMTMsgCount = 0x04030201;
    sCAN0TxMessage.ui32MsgID = 0x7F0 | (DFU_DEVICE_ID & 0x0F);
    sCAN0TxMessage.ui32MsgIDMask = 0x7FF;
    sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    sCAN0TxMessage.ui32MsgLen = sizeof(g_ui32RMTMsgCount);
    sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui32RMTMsgCount;

    CANMessageSet(CAN0_BASE, 5, &sCAN0TxMessage, MSG_OBJ_TYPE_RXTX_REMOTE);  // <------------------------------->

    ui32TXMsgData = (uint32_t *)g_aui8RXMsgData;
    while(1) {
        Semaphore_pend(g_CANTestSemHandle, BIOS_WAIT_FOREVER);
//        GPIO_write(MRB_LED1, DO_ON);
        g_sCAN0RxMessage.pui8MsgData = (uint8_t *) g_aui8RXMsgData;

        CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, 0);  // <-------------------------------

//        g_ui8TXMsgData = g_aui8RXMsgData[0] + 1;
        (*ui32TXMsgData)++;

        g_sCAN0TxMessage.ui32MsgID = 0x7D0 | (HOST_DEVICE_ID & 0x00F);;
        g_sCAN0TxMessage.ui32MsgIDMask = 0;
        g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
        g_sCAN0TxMessage.ui32MsgLen = 4;
        g_sCAN0TxMessage.pui8MsgData = (uint8_t *)g_aui8RXMsgData;
        CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX);  // ------------------------------->
//        GPIO_write(MRB_LED1, DO_OFF);

        //
        // Error Handling
        //
        if(g_ui32ErrFlag != 0)
        {
            CANErrorHandler();
        }
    }
}



Void CANTestHostFxn(UArg arg0, UArg arg1)
{
    tCANMsgObject sCAN0TxMessage;


    //
    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    //
    g_sCAN0RxMessage.ui32MsgID = 0x7D0 | (HOST_DEVICE_ID & 0x0F);
    g_sCAN0RxMessage.ui32MsgIDMask = 0x7FF;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object RXOBJECT for receiving messages (this is not the
    //same as the CAN ID which can be any value in this example).
    //
    CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);  // <-------------------------------

    while(1) {
        sCAN0TxMessage.ui32MsgID = 0x7E0 | (DFU_DEVICE_ID & 0x0F);
        sCAN0TxMessage.ui32MsgIDMask = 0;
        sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        sCAN0TxMessage.ui32MsgLen = sizeof(g_ui32TXMsgData);
        sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui32TXMsgData;

//        GPIO_write(MRB_LED2, DO_ON);
        GPIO_write(Board_LED4, 0);
        CANMessageSet(CAN0_BASE, 4, &sCAN0TxMessage, MSG_OBJ_TYPE_TX);  // ------------------------------->

//        Task_sleep(1);
//
//        sCAN0TxMessage2.ui32MsgID = 0x7F0 | (DFU_DEVICE_ID & 0x0F);;
//        sCAN0TxMessage2.ui32MsgIDMask = 0;
//        sCAN0TxMessage2.ui32Flags = MSG_OBJ_NO_FLAGS;
//        sCAN0TxMessage2.ui32MsgLen = sizeof(g_ui32TXMsgData);
//        sCAN0TxMessage2.pui8MsgData = (uint8_t *)&g_ui32TXMsgData;
//        CANMessageSet(CAN0_BASE, 4, &sCAN0TxMessage2, MSG_OBJ_TYPE_TX_REMOTE);  // ------------------------------->


        if(g_ui32ErrFlag != 0)
        {
            CANErrorHandler();
        }

        if (Semaphore_pend(g_CANTestSemHandle, 1000)) {  // <-------------------------------
//            GPIO_write(MRB_LED1, DO_ON);
            GPIO_write(Board_LED4, 1);
            g_sCAN0RxMessage.pui8MsgData = (uint8_t *) g_aui8RXMsgData;

            CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, 0);

            //        g_ui8TXMsgData = g_aui8RXMsgData[0] + 1;
            //        g_sCAN0TxMessage.ui32MsgID = 0x7D0 | (DFU_DEVICE_ID & 0x00F);;
            //        g_sCAN0TxMessage.ui32MsgIDMask = 0;
            //        g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
            //        g_sCAN0TxMessage.ui32MsgLen = sizeof(g_ui8TXMsgData);
            //        g_sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui8TXMsgData;
            //        CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX);
//            GPIO_write(MRB_LED1, DO_OFF);
//            GPIO_write(Board_LED4, 1);
            g_ui32TXMsgData++;
        }else{
            g_ui32HostErrorCount++;
        }

        //
        // Error Handling
        //
        if(g_ui32ErrFlag != 0)
        {
            CANErrorHandler();
//            GPIO_write(Board_LED4,0);
        }

        Task_sleep(100);
    }
}


Void CANTestDemoFxn(UArg arg0, UArg arg1)
{
    tCANMsgObject sCAN0TxMessage;
    CANDemo_msgSDODownloadRequest *pSDODRmsg;
    uint8_t msgTxData[8];
    uint8_t msgRxData[8];

    pSDODRmsg = (CANDemo_msgSDODownloadRequest *)msgTxData;
    //
    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    //
    g_sCAN0RxMessage.ui32MsgID = CANDEMO_COIDTYPE_TSDO | (DFU_DEVICE_ID & 0x7F);
    g_sCAN0RxMessage.ui32MsgIDMask = CANDEMO_MSK_MSGTYPE;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_USE_ID_FILTER; //MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object RXOBJECT for receiving messages (this is not the
    //same as the CAN ID which can be any value in this example).
    //
    CANMessageSet(CAN0_BASE, CANDEMO_SDO_RX, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);  // <-------------------------------

    while(1) {
        pSDODRmsg->Index = 0x2000;
        pSDODRmsg->subIndex = 0x00;
        pSDODRmsg->lengthNoData = sizeof(uint32_t) - sizeof(g_ui32TXMsgData);
        pSDODRmsg->expeditedData = g_ui32TXMsgData++;
        pSDODRmsg->clientCommandSpecifier = CANDEMO_SDO_CCS_DOWNLOAD_REQUEST;

        sCAN0TxMessage.ui32MsgID = CANDEMO_COIDTYPE_RSDO | (DFU_DEVICE_ID & CANDEMO_MSK_NODEID);
        sCAN0TxMessage.ui32MsgIDMask = 0;
        sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        sCAN0TxMessage.ui32MsgLen = sizeof(msgTxData);
        sCAN0TxMessage.pui8MsgData = (uint8_t *)msgTxData;

        //        GPIO_write(MRB_LED2, DO_ON);
        CANMessageSet(CAN0_BASE, CANDEMO_SDO_TX, &sCAN0TxMessage, MSG_OBJ_TYPE_TX);  // ------------------------------->



        if (Semaphore_pend(g_CANTestSemHandle, 1000)) {  // <-------------------------------
            //            GPIO_write(MRB_LED1, DO_ON);
            g_sCAN0RxMessage.pui8MsgData = (uint8_t *) msgRxData;

        }else{
            g_ui32HostErrorCount++;
        }

        //
        // Error Handling
        //
        if(g_ui32ErrFlag != 0)
        {
            CANErrorHandler();
//            GPIO_write(Board_LED4,0);
        }

        Task_sleep(1);
    }
}


Void CANTestSMCFxn(UArg arg0, UArg arg1)
{
    uint32_t events;
    tCANMsgObject sCAN0TxMessage;
    CANDemo_msgSDODownloadRequest *pSDODRmsg;
    CANDemo_msgHeartbeat heartbeat;
    uint8_t msgTxData[8];
    uint8_t msgRxData[8];

    pSDODRmsg = (CANDemo_msgSDODownloadRequest *)msgTxData;
    //
    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    //
    g_sCAN0RxMessage.ui32MsgID = CANDEMO_COIDTYPE_TSDO | (DFU_DEVICE_ID & 0x7F);
    g_sCAN0RxMessage.ui32MsgIDMask = CANDEMO_MSK_MSGTYPE;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_USE_ID_FILTER; //MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object RXOBJECT for receiving messages (this is not the
    //same as the CAN ID which can be any value in this example).
    //
    CANMessageSet(CAN0_BASE, CANDEMO_SDO_RX, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);  // <-------------------------------




    while(1) {
        events = Event_pend(g_CANDemoEventHandle, Event_Id_NONE, (CANDEMO_ALL_EVENTS), 2000); //BIOS_WAIT_FOREVER

        if (events & CANDEMO_PERIODIC_EVT) {
            events &= ~CANDEMO_PERIODIC_EVT;

//            sCAN0TxMessage.ui32MsgID = CANDEMO_COIDTYPE_HEARTBEAT | (1 & CANDEMO_MSK_NODEID);
//            sCAN0TxMessage.ui32MsgIDMask = 0;
//            sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
//            sCAN0TxMessage.ui32MsgLen = sizeof(CANDemo_msgHeartbeat);
//            sCAN0TxMessage.pui8MsgData = (uint8_t *)&heartbeat;
//
//            //    heartbeat.unitSerialNumber = INFO_get()->unitSerialNumber;
//
//            heartbeat.nmtState = CANDEMO_NMT_OPERATIONAL;
//            heartbeat.reserved = 0;

//            CANMessageSet(CAN0_BASE, CANDEMO_HeartBeat, &sCAN0TxMessage, MSG_OBJ_TYPE_TX);

            pSDODRmsg->Index = CANDEMO_OD_INDEX_INPUT;
            pSDODRmsg->subIndex = CANDEMO_OD_INPUT_SWITCHES;
            pSDODRmsg->lengthNoData = sizeof(uint32_t) - sizeof(g_ui32TXMsgData);
            pSDODRmsg->expeditedData = g_ui32TXMsgData++;
            pSDODRmsg->clientCommandSpecifier = CANDEMO_SDO_CCS_UPLOAD_REQUEST;

            sCAN0TxMessage.ui32MsgID = CANDEMO_COIDTYPE_RSDO | (DFU_DEVICE_ID & CANDEMO_MSK_NODEID);
            sCAN0TxMessage.ui32MsgIDMask = 0;
            sCAN0TxMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
            sCAN0TxMessage.ui32MsgLen = sizeof(msgTxData);
            sCAN0TxMessage.pui8MsgData = (uint8_t *)msgTxData;

            //        GPIO_write(MRB_LED2, DO_ON);
            CANMessageSet(CAN0_BASE, CANDEMO_SDO_TX, &sCAN0TxMessage, MSG_OBJ_TYPE_TX);  // ------------------------------->

        }


//        if (Semaphore_pend(g_CANTestSemHandle, 1000)) {  // <-------------------------------
//            //            GPIO_write(MRB_LED1, DO_ON);
//            g_sCAN0RxMessage.pui8MsgData = (uint8_t *) msgRxData;
//
//        }else{
//            g_ui32HostErrorCount++;
//        }

        //
        // Error Handling
        //
        if(g_ui32ErrFlag != 0)
        {
            CANErrorHandler();
//            GPIO_write(Board_LED4,0);
        }

        Task_sleep(1);
    }
}



void vCANDemo_clockHandler(UArg arg)
{
    Event_Handle eventHandle;

    ASSERT(arg != NULL);

    if (arg == NULL) {
        return;
    }
    eventHandle = (Event_Handle)arg;

    Event_post(eventHandle, CANDEMO_PERIODIC_EVT);
}



void CANTTest_init()
{
    Task_Params taskParams;
    Semaphore_Params semParams;
    Swi_Params swiParams;
    Clock_Params clockParams;
    Error_Block eb;

    Error_init(&eb);

    InitCAN0();

    g_CANDemoEventHandle = Event_create(NULL, &eb);

    Clock_Params_init(&clockParams);
    clockParams.period = 1000;
    clockParams.startFlag = TRUE;
    clockParams.arg = (UArg)g_CANDemoEventHandle;
    Clock_create(vCANDemo_clockHandler, 100, &clockParams, &eb);


    Task_Params_init(&taskParams);
    taskParams.stackSize = 768;
    taskParams.priority = 11;
//    Task_create((Task_FuncPtr)canTaskFxn, &taskParams, NULL);
//    Task_create((Task_FuncPtr)canReplayFxn, &taskParams, NULL);
    Task_create((Task_FuncPtr)CANTestSMCFxn, &taskParams, NULL);
//    Task_create((Task_FuncPtr)CANTestDemoFxn, &taskParams, NULL);
//    Task_create((Task_FuncPtr)CANTestDUTFxn, &taskParams, NULL);
//    Task_create((Task_FuncPtr)CANTestHostFxn, &taskParams, NULL);

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&g_CANTestSemStruct, 0, &semParams);
    /* Obtain instance handle */
    g_CANTestSemHandle = Semaphore_handle(&g_CANTestSemStruct);


    Swi_Params_init(&swiParams);
    swiParams.arg0 = 1;
    swiParams.arg1 = 0;
    swiParams.priority = 2;
    swiParams.trigger = 0;
    //    Swi_construct(&swi0Struct, (Swi_FuncPtr)swi0Fxn, &swiParams, NULL);
    //    swi0Handle = Swi_handle(&swi0Struct);
    g_CANTestSwi0Handle = Swi_create((Swi_FuncPtr)swi0Fxn, &swiParams, NULL);

}


