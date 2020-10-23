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
__NDK_UDPAVDSSERVER_EXT bool bAVDSUDP_isIPValid();
__NDK_UDPAVDSSERVER_EXT uint32_t xAVDSUDP_getIPAddress();
__NDK_UDPAVDSSERVER_EXT uint16_t xsAVDSUDP_getPortNumber();

#ifdef __cplusplus
}
#endif

#endif /* NDK_UDPAVDSSERVER_H_ */
