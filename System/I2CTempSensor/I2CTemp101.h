/*
 * I2CTemp101.h
 *
 *  Created on: May 20, 2019
 *      Author: epenate
 */

#ifndef SYSTEM_I2CTEMPSENSOR_I2CTEMP101_H_
#define SYSTEM_I2CTEMPSENSOR_I2CTEMP101_H_

#define I2CTEMP101_TASK_STACK_SIZE         768
#define I2CTEMP101_TASK_PRIORITY           2

#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _SYSTEM_I2CTEMPSENSOR_I2CTEMP101_GLOBAL
    #define SYSTEM_I2CTEMPSENSOR_I2CTEMP101_EXT
#else
    #define SYSTEM_I2CTEMPSENSOR_I2CTEMP101_EXT    extern
#endif

SYSTEM_I2CTEMPSENSOR_I2CTEMP101_EXT
int16_t g_i16I2CTempSensor;

SYSTEM_I2CTEMPSENSOR_I2CTEMP101_EXT
void vI2CTemp101_init();

#ifdef __cplusplus
}
#endif




#endif /* SYSTEM_I2CTEMPSENSOR_I2CTEMP101_H_ */
