/*
 * InternalFlashStorage.h
 *
 *  Created on: Nov 15, 2018
 *      Author: epenate
 */

#ifndef SYSTEM_INTERNALFLASHSTORAGE_INTERNALFLASHSTORAGE_H_
#define SYSTEM_INTERNALFLASHSTORAGE_INTERNALFLASHSTORAGE_H_

#define IFS_STARTUP_CONF_FILE_NAME          "startUpConfigFile"
#define IFS_MEM_TEST_FILE_NAME              "memTestFile"
#define IFS_STARTUP_DEVICE_0_FILE_NAME      "devices/dev000"
#define IFS_STARTUP_DEVICE_1_FILE_NAME      "devices/dev001"
#define IFS_STARTUP_DEVICE_2_FILE_NAME      "devices/dev002"

#define IFS_DEVICES_FOLDER_NAME      "devices/"

#define IFS_FILE_DESCRIPTION_LENGTH          (64)


typedef struct {
    Device_Params params;
    char description[IFS_FILE_DESCRIPTION_LENGTH];
    char data[];
}IFS_deviceInfoFile_t;


#ifdef  __SYSTEM_INTERNALFLASHSTORAGE_GLOBAL
    #define __SYSTEM_INTERNALFLASHSTORAGE_EXT
#else
    #define __SYSTEM_INTERNALFLASHSTORAGE_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_init();

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_loadStartUpConfiguration(void *arg0);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_getFlashDeviceListEthernet(int clientfd);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_getFlashReadFileNameEthernet(int clientfd, char *fileName);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_setFlashDataFileNameEthernet(int clientfd, char *payload);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_removeFileNameEthernet(int clientfd, char *payload);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
void vIFS_getFlashConfigurationFileEthernet(int clientfd);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
uint32_t vIFS_testMemoryEthernet(int clientfd);

__SYSTEM_INTERNALFLASHSTORAGE_EXT
int32_t vIFS_getStatus();

__SYSTEM_INTERNALFLASHSTORAGE_EXT
int32_t vIFS_format();

#ifdef __cplusplus
}
#endif




#endif /* SYSTEM_INTERNALFLASHSTORAGE_INTERNALFLASHSTORAGE_H_ */
