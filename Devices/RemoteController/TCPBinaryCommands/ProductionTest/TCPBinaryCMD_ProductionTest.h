/*
 * TCPBinaryCMD_ProductionTest.h
 *
 *  Created on: Aug 27, 2019
 *      Author: epenate
 */

#ifndef DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_H_
#define DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_H_



#ifdef  __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_GLOBAL
    #define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
#else
    #define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif

__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
void vTCPRCBin_ProductionTest_getCounters(int clientfd, char *payload, int32_t size);
__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
void vTCPRCBin_ProductionTest_resetCounters(int clientfd, char *payload, int32_t size);
__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
void vTCPRCBin_ProductionTest_setGPIOAsInput(int clientfd, char *payload, int32_t size);
__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
void vTCPRCBin_ProductionTest_setGPIOAsOutput(int clientfd, char *payload, int32_t size);
__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
void vTCPRCBin_ProductionTest_setGPIOOutputValue(int clientfd, char *payload, int32_t size);
__DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_EXT
void vTCPRCBin_ProductionTest_getGPIOInputValue(int clientfd, char *payload, int32_t size);




#ifdef __cplusplus
}
#endif



#endif /* DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_PRODUCTIONTEST_TCPBINARYCMD_PRODUCTIONTEST_H_ */
