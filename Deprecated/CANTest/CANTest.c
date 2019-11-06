/*
 * CANTest.c
 *
 *  Created on: Sep 12, 2019
 *      Author: epenate
 */


#define __DEPRECATED_CANTEST_GLOBAL
#include "includes.h"




/*
 * CAN Test
 */


//static CAN_Handle g_hCANTestCANHandler0;
//static CAN_Handle g_hCANTestCANHandler1;



Void vCANTest_canTXTestFxn(UArg arg0, UArg arg1)
{
    CAN_Handle canHandler = (CAN_Handle)arg1;
    CAN_Frame canFrame[2] = {0};
    uint32_t *pui32Data1 = (uint32_t *)canFrame[1].data;
    uint32_t *pui32Data0 = (uint32_t *)canFrame[0].data;
    uint32_t volatile *pui32Error, *pui32FrameSent;

    if (arg0 == NULL) {
        pui32Error = &g_ui32CANTestFrameError0;
        pui32FrameSent = &g_ui32CANTestFramesSent0;
        Task_sleep((unsigned int)120);
    }else{
        pui32Error = &g_ui32CANTestFrameError1;
        pui32FrameSent = &g_ui32CANTestFramesSent1;
        Task_sleep((unsigned int)150);
    }

    while (1) {
        Task_sleep((unsigned int)100);
#ifdef TEST_FIXTURE
        canFrame[0].id = 1;  // TF is sending
        canFrame[1].id = 4;  // TF is sending
#endif
#ifdef DUT
        canFrame[0].id = 2; // DUT is sending
        canFrame[1].id = 2; // DUT is sending
#endif
        canFrame[0].err = 0;
        canFrame[0].rtr = 0;
        canFrame[0].eff = 0;
        canFrame[0].dlc = 4;
//        canFrame[0].data[0] = framesSent++;
        *pui32Data0 = (*pui32FrameSent)++;

        canFrame[1].err = 0;
        canFrame[1].rtr = 0;
        canFrame[1].eff = 0;
        canFrame[1].dlc = 8;
//        *pui32Data1 = *pui32Error;
        pui32Data1 = (uint32_t *)canFrame[1].data;
        *(pui32Data1++) = g_ui32CANTestFrameError0;
        *pui32Data1 = g_ui32CANTestFrameError1;
        CAN_write(canHandler, &canFrame, sizeof(canFrame));
//        Display_print1(g_SMCDisplay, 0, 0, "Button pressed. Frame ID sent: 0x%3x", canFrame.id);
    }
}

Void vCANTest_canRXTestFxn(UArg arg0, UArg arg1)
{
    CAN_Params canParams;
    uint32_t *pui32Data;
    uint32_t volatile *pui32Error, *pui32FrameSent;
    int_fast32_t result;
    CAN_Handle canHandler;

    Display_printf(g_SMCDisplay, 0, 0, "canRXTestFxn task started\n");

    g_ui32CANTestFramesSent0 = g_ui32CANTestFrameError0 = 0;

    /*
     * Open CAN instance.
     * CAN_Params filterID and filterMask determine what messages to receive.
     */
    CAN_Params_init(&canParams);
#ifdef TEST_FIXTURE
    canParams.filterID = 0x002; //TF reading addres
    canParams.readTimeout = 200;

#endif
#ifdef DUT
    canParams.filterID = 0x001; //DUT reading addres
#endif

    canParams.filterMask = 0x003;
    if (arg0 == NULL) {
        canHandler = CAN_open(Board_CAN0, &canParams);
        pui32Error = &g_ui32CANTestFrameError0;
        pui32FrameSent = &g_ui32CANTestFramesSent0;
    }else{
        canHandler = CAN_open(Board_CAN1, &canParams);
        pui32Error = &g_ui32CANTestFrameError1;
        pui32FrameSent = &g_ui32CANTestFramesSent1;
    }

#ifdef TEST_FIXTURE
    Task_Params taskParams;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing canTXTestFxn\n");
    Task_Params_init(&taskParams);
    taskParams.priority = 3;
    taskParams.stackSize = 1024;
    taskParams.arg0 = arg0;
    taskParams.arg1 = (UArg)canHandler;
    Task_create((Task_FuncPtr)vCANTest_canTXTestFxn, &taskParams, NULL);
#endif

    /*
     * Toggles Board_GPIO_LED0 every time a CAN frame is received.
     * Also prints out the received CAN frame ID to UART display.
     * CAN_Params filterID, filterMask control how often CAN_read is completed.
     */
    CAN_Frame canFrame = {0};
    while (1) {
        result = CAN_read(canHandler, &canFrame, sizeof(canFrame));
        DOSet(DIO_LED_D20, DOGet(DIO_LED_D20));
        pui32Data = (uint32_t *)canFrame.data;
#ifdef DUT
        canFrame.id = 2; // DUT is sending
        canFrame.err = 0;
        canFrame.rtr = 0;
        canFrame.eff = 0;
        canFrame.dlc = 4;
//        canFrame.data[0] = canFrame.data[0] + 1;
        (*pui32Data)++;

        CAN_write(canHandler, &canFrame, sizeof(canFrame));
#endif

#ifdef TEST_FIXTURE
        if (result == sizeof(canFrame)) {
            if (*pui32Data != *pui32FrameSent) (*pui32Error)++;
        }else {
            (*pui32Error)++;
        }
#endif
    }
}

void vCANTest_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing canRXTestFxn\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = 1024;
    taskParams.priority = 8;
    taskParams.arg0 = 0;
    Task_create((Task_FuncPtr)vCANTest_canRXTestFxn, &taskParams, NULL);
    taskParams.arg0 = 1;
    Task_create((Task_FuncPtr)vCANTest_canRXTestFxn, &taskParams, NULL);
}
