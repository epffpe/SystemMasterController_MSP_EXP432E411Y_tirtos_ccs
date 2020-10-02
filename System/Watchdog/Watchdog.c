/*
 * Watchdog.c
 *
 *  Created on: Oct 1, 2020
 *      Author: epffpe
 */

#define _SYSTEM_WATCHDOG_WATCHDOG_GLOBAL
#include "includes.h"

#define SLEEP_MS        300
#define TIMEOUT_MS      1000

/*
 *  ======== watchdogCallback ========
 */
void vWDG_watchdogCallback(uintptr_t watchdogHandle)
{
    /*
     * If the Watchdog Non-Maskable Interrupt (NMI) is called,
     * loop until the device resets. Some devices will invoke
     * this callback upon watchdog expiration while others will
     * reset. See the device specific watchdog driver documentation
     * for your device.
     */
    while (1) {}
}


/*
 *  ======== mainThread ========
 */
Void vWDG_taskFxn(UArg arg0, UArg arg1)
{
    Watchdog_Handle watchdogHandle;
    Watchdog_Params params;
    uint32_t        reloadValue;


    /* Open a Watchdog driver instance */
    Watchdog_Params_init(&params);
    params.callbackFxn = (Watchdog_Callback) vWDG_watchdogCallback;
    params.debugStallMode = Watchdog_DEBUG_STALL_ON;
    params.resetMode = Watchdog_RESET_ON;

    watchdogHandle = Watchdog_open(Board_WATCHDOG0, &params);
    if (watchdogHandle == NULL) {
        /* Error opening Watchdog */
        Display_printf(g_SMCDisplay, 0, 0, "Couldn't open the Watchdog");
        return;
    }

    /*
     * The watchdog reload value is initialized during the
     * Watchdog_open() call. The reload value can also be
     * set dynamically during runtime.
     *
     * Converts TIMEOUT_MS to watchdog clock ticks.
     * This API is not applicable for all devices.
     * See the device specific watchdog driver documentation
     * for your device.
     */
    reloadValue = Watchdog_convertMsToTicks(watchdogHandle, TIMEOUT_MS);

    /*
     * A value of zero (0) indicates the converted value exceeds 32 bits
     * OR that the API is not applicable for this specific device.
     */
    if (reloadValue != 0) {
        Watchdog_setReload(watchdogHandle, reloadValue);
    }


    while (1) {
        /* Sleep for SLEEP_US before clearing the watchdog */
        Task_sleep((unsigned int)SLEEP_MS);
        Watchdog_clear(watchdogHandle);
    }
}


void vWDG_init()
{
    Task_Params taskParams;
//    Task_Handle taskHandle;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);
    Display_printf(g_SMCDisplay, 0, 0, "Initializing vWDG_taskFxn\n");
    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.priority = SYSTEM_WATCHDOG_TASK_PRIORITY;
    taskParams.stackSize = SYSTEM_WATCHDOG_TASK_STACK_SIZE;
    Task_create((Task_FuncPtr)vWDG_taskFxn, &taskParams, NULL);
}



