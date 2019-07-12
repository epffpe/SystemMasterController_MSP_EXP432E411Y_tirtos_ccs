/*
 * SPIDAC.h
 *
 *  Created on: Jul 12, 2019
 *      Author: epenate
 */

#ifndef SYSTEM_SPIDAC_SPIDAC_H_
#define SYSTEM_SPIDAC_SPIDAC_H_


#define SPIDAC101_TASK_STACK_SIZE         768
#define SPIDAC101_TASK_PRIORITY           3


#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _SYSTEM_SPIDAC_SPIDAC_GLOBAL
    #define SYSTEM_SPIDAC_SPIDAC_EXT
#else
    #define SYSTEM_SPIDAC_SPIDAC_EXT    extern
#endif

SYSTEM_SPIDAC_SPIDAC_EXT
uint16_t g_ui16SPIDAC101_dacVal;


SYSTEM_SPIDAC_SPIDAC_EXT
uint32_t g_ui32SPIDAC101_R46Val;
SYSTEM_SPIDAC_SPIDAC_EXT
uint32_t g_ui32SPIDAC101_R47Val;


SYSTEM_SPIDAC_SPIDAC_EXT
void vSPIDAC101_init();


#ifdef __cplusplus
}
#endif




#endif /* SYSTEM_SPIDAC_SPIDAC_H_ */
