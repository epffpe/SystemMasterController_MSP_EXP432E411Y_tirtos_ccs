/*
 * CANTest.h
 *
 *  Created on: Sep 12, 2019
 *      Author: epenate
 */

#ifndef DEPRECATED_CANTEST_CANTEST_H_
#define DEPRECATED_CANTEST_CANTEST_H_

#define DEPRECATED_CANTEST_CANTEST_TASK_STACK_SIZE        1024
#define DEPRECATED_CANTEST_CANTEST_TASK_PRIORITY          8


#define DUT
//#define TEST_FIXTURE


#ifdef  __DEPRECATED_CANTEST_GLOBAL
    #define __DEPRECATED_CANTEST_EXT
#else
    #define __DEPRECATED_CANTEST_EXT  extern
#endif

#ifdef __cplusplus
extern "C"  {
#endif

__DEPRECATED_CANTEST_EXT
volatile uint32_t g_ui32CANTestFramesSent0, g_ui32CANTestFrameError0;
__DEPRECATED_CANTEST_EXT
volatile uint32_t g_ui32CANTestFramesSent1, g_ui32CANTestFrameError1;

__DEPRECATED_CANTEST_EXT
void vCANTest_init();

#ifdef __cplusplus
}
#endif

#endif /* DEPRECATED_CANTEST_CANTEST_H_ */
