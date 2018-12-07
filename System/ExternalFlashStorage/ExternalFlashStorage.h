/*
 * ExternalFlashStorage.h
 *
 *  Created on: Nov 14, 2018
 *      Author: epenate
 */

#ifndef SYSTEM_EXTERNALFLASHSTORAGE_EXTERNALFLASHSTORAGE_H_
#define SYSTEM_EXTERNALFLASHSTORAGE_EXTERNALFLASHSTORAGE_H_




#ifdef  __SYSTEM_EXTERNALFLASHSTORAGE_GLOBAL
    #define __SYSTEM_EXTERNALFLASHSTORAGE_EXT
#else
    #define __SYSTEM_EXTERNALFLASHSTORAGE_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif

__SYSTEM_EXTERNALFLASHSTORAGE_EXT
void vEFS_loadStartUpConfiguration(void *arg0);


#ifdef __cplusplus
}
#endif



#endif /* SYSTEM_EXTERNALFLASHSTORAGE_EXTERNALFLASHSTORAGE_H_ */
