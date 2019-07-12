/*
 * SystemAnalogMonitor.h
 *
 *  Created on: Nov 5, 2018
 *      Author: epenate
 */

#ifndef SYSTEM_SYSTEMANALOGMONITOR_H_
#define SYSTEM_SYSTEMANALOGMONITOR_H_


#define SYSTEMANALOGMONITOR_TASK_STACK_SIZE         1024
#define SYSTEMANALOGMONITOR_TASK_PRIORITY           3


typedef struct {
    uint16_t internalTempSensor;
    uint16_t logicCur;
    uint16_t perCur;
    uint16_t v5Main;
    uint16_t v5Per;
    uint16_t v28;
    uint16_t mcuCur;
} SAM_Channels;



#ifdef  __SYSTEM_SYSTEMANALOGMONITOR_GLOBAL
    #define __SYSTEM_SYSTEMANALOGMONITOR_EXT
#else
    #define __SYSTEM_SYSTEMANALOGMONITOR_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif

__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgCPUTemperature;
__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgLogicCur;
__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgPerCur;
__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgV5Main;
__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgV5Per;
__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgV28;
__SYSTEM_SYSTEMANALOGMONITOR_EXT
float g_fSAMavgMCUCur;


__SYSTEM_SYSTEMANALOGMONITOR_EXT
void vSAM_init();


#ifdef __cplusplus
}
#endif



#endif /* SYSTEM_SYSTEMANALOGMONITOR_H_ */
