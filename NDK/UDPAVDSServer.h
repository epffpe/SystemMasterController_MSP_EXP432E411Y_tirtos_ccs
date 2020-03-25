/*
 * UDPAVDSServer.h
 *
 *  Created on: Mar 25, 2020
 *      Author: epf
 */

#ifndef NDK_UDPAVDSSERVER_H_
#define NDK_UDPAVDSSERVER_H_


typedef struct {
    uint32_t ipAddress;
    uint16_t portNumber;
    bool    isValid;
}tsUDPAVDSInfoMsg;





#ifdef  __NDK_UDPAVDSSERVER_GLOBAL
    #define __NDK_UDPAVDSSERVER_EXT
#else
    #define __NDK_UDPAVDSSERVER_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif


__NDK_UDPAVDSSERVER_EXT void *pvUDPAVDSFinder_taskFxn(void *arg0);

#ifdef __cplusplus
}
#endif

#endif /* NDK_UDPAVDSSERVER_H_ */
