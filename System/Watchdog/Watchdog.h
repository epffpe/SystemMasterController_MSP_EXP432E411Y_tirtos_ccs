/*
 * Watchdog.h
 *
 *  Created on: Oct 1, 2020
 *      Author: epffpe
 */

#ifndef SYSTEM_WATCHDOG_WATCHDOG_H_
#define SYSTEM_WATCHDOG_WATCHDOG_H_



#define SYSTEM_WATCHDOG_TASK_STACK_SIZE         768
#define SYSTEM_WATCHDOG_TASK_PRIORITY           10


#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _SYSTEM_WATCHDOG_WATCHDOG_GLOBAL
    #define SYSTEM_WATCHDOG_WATCHDOG_EXT
#else
    #define SYSTEM_WATCHDOG_WATCHDOG_EXT    extern
#endif

void vWDG_init();

#ifdef __cplusplus
}
#endif



#endif /* SYSTEM_WATCHDOG_WATCHDOG_H_ */
