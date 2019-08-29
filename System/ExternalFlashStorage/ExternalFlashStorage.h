/*
 * ExternalFlashStorage.h
 *
 *  Created on: Nov 14, 2018
 *      Author: epenate
 */

#ifndef SYSTEM_EXTERNALFLASHSTORAGE_EXTERNALFLASHSTORAGE_H_
#define SYSTEM_EXTERNALFLASHSTORAGE_EXTERNALFLASHSTORAGE_H_

#define EFS_MAXIMUM_MEMORY_ALLOCATED        1024


#ifdef  __SYSTEM_EXTERNALFLASHSTORAGE_GLOBAL
    #define __SYSTEM_EXTERNALFLASHSTORAGE_EXT
#else
    #define __SYSTEM_EXTERNALFLASHSTORAGE_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif


__SYSTEM_EXTERNALFLASHSTORAGE_EXT
void vEFS_init();
__SYSTEM_EXTERNALFLASHSTORAGE_EXT
void vEFS_loadStartUpConfigurationTest(void *arg0);
__SYSTEM_EXTERNALFLASHSTORAGE_EXT
void vEFS_loadStartUpConfiguration(void *arg0);
__SYSTEM_EXTERNALFLASHSTORAGE_EXT
void vEFS_setFlashDataFileNameHTTP(int clientfd, int contentLength, char *fileName);
__SYSTEM_EXTERNALFLASHSTORAGE_EXT
void vEFS_getFlashDataFileNameHTTP(int clientfd, char *fileName);


#ifdef __cplusplus
}
#endif



#endif /* SYSTEM_EXTERNALFLASHSTORAGE_EXTERNALFLASHSTORAGE_H_ */
