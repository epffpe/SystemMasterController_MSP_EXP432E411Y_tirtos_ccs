/*
 * SMC_Interfaces.h
 *
 *  Created on: Mar 6, 2018
 *      Author: epenate
 */

#ifndef INTERFACES_SMC_INTERFACES_H_
#define INTERFACES_SMC_INTERFACES_H_




#ifdef __cplusplus
extern "C"  {
#endif



#ifdef  __INTERFACES_SMC_INTERFACES_GLOBAL
    #define __INTERFACES_SMC_INTERFACES_EXT
#else
    #define __INTERFACES_SMC_INTERFACES_EXT  extern
#endif


/*!
 *  @def    DK_TM4C129X_SDSPIName
 *  @brief  Enum of SDSPI names on the DK_TM4C129X dev board
 */
typedef enum teInterfacesName {
    IF_SERIAL_0 = 0,
    IF_SERIAL_1,
    IF_SERIAL_2,
    IF_SERIAL_3,
    IF_SERIAL_4,
    IF_SERIAL_5 = 5,
    IF_RS232    = 5,
    IF_DEBUG    = 5,
    IF_SERIAL_6,
    IF_MULTINET = 6,
    IF_SERIAL_7,
    IF_UART_MAX10 = 7,

    IF_COUNT
} teInterfacesName;




extern const IF_Config IF_config[];


#ifdef __cplusplus
}
#endif





#endif /* INTERFACES_SMC_INTERFACES_H_ */
