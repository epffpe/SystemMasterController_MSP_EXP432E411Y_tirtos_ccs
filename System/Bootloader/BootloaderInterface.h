/*
 * BootloaderInterface.h
 *
 *  Created on: May 22, 2020
 *      Author: epffpe
 */

#ifndef SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_H_
#define SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_H_



#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  _SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_GLOBAL
    #define _SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_EXT
#else
    #define _SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_EXT    extern
#endif

_SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_EXT
void JumpToBootLoader(void);


#ifdef __cplusplus
}
#endif



#endif /* SYSTEM_BOOTLOADER_BOOTLOADERINTERFACE_H_ */
