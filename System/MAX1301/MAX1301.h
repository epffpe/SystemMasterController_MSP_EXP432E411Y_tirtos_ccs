/*
 * MAX1301.h
 *
 *  Created on: Jul 16, 2019
 *      Author: epenate
 */

#ifndef SYSTEM_MAX1301_MAX1301_H_
#define SYSTEM_MAX1301_MAX1301_H_


#define MAX1301_TASK_STACK_SIZE         768
#define MAX1301_TASK_PRIORITY           3


#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _SYSTEM_MAX1301_MAX1301_GLOBAL
    #define SYSTEM_MAX1301_MAX1301_EXT
#else
    #define SYSTEM_MAX1301_MAX1301_EXT    extern
#endif

SYSTEM_MAX1301_MAX1301_EXT
float g_fMAX1301Vin_volt;
SYSTEM_MAX1301_MAX1301_EXT
float g_fMAX1301Vout_volt;
SYSTEM_MAX1301_MAX1301_EXT
float g_fMAX1301V5_volt;


SYSTEM_MAX1301_MAX1301_EXT
void vMAX1301_init();


#ifdef __cplusplus
}
#endif



#endif /* SYSTEM_MAX1301_MAX1301_H_ */
