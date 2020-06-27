/*
 * FlashFileSystemStorage.h
 *
 *  Created on: Jun 23, 2020
 *      Author: epffpe
 */

#ifndef SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_H_
#define SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_H_


/* SPIFFS configuration parameters */
#define SFFS_SPIFFS_LOGICAL_BLOCK_SIZE    (16384)
#define SFFS_SPIFFS_LOGICAL_PAGE_SIZE     (128)
#define SFFS_SPIFFS_FILE_DESCRIPTOR_SIZE  (44)



typedef struct {
    uint32_t                nvsIndex;
    uint32_t                logicalBlockSize;
    uint32_t                logicalPageSize;

}SFFS_HWAttrs;


typedef struct {
    spiffs                  fs;
    SPIFFSNVS_Data          spiffsnvsData;
    spiffs_config           fsConfig;
    struct {
        bool            initialized:1;
        bool            mounted:1;

    } state;
    Semaphore_Handle hSFFS_semInitialized;
    GateMutexPri_Handle hSFFS_gateOpen;
    GateMutexPri_Handle hSFFS_gateWait;
    GateMutexPri_Handle hSFFS_gateMount;

    /*
     * SPIFFS needs RAM to perform operations on files.  It requires a work buffer
     * which MUST be (2 * LOGICAL_PAGE_SIZE) bytes.
     */
    static uint8_t spiffsWorkBuffer[SFFS_SPIFFS_LOGICAL_PAGE_SIZE * 2];

    /* The array below will be used by SPIFFS as a file descriptor cache. */
    static uint8_t spiffsFileDescriptorCache[SFFS_SPIFFS_FILE_DESCRIPTOR_SIZE * 4];

    /* The array below will be used by SPIFFS as a read/write cache. */
    static uint8_t spiffsReadWriteCache[SFFS_SPIFFS_LOGICAL_PAGE_SIZE * 2];

}SFFS_Object;



typedef struct {
    SFFS_HWAttrs    *hwAttrs;
    SFFS_Object     *object;
}SFFS_Config;


typedef SFFS_Config      *SFFS_Handle;



typedef struct {
    Device_Params params;
    char description[IFS_FILE_DESCRIPTION_LENGTH];
    char data[];
}SFFS_deviceInfoFile_t;


#ifdef  __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_GLOBAL
    #define __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
#else
    #define __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
SFFS_Config g_IFSConfig;
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
SFFS_Config g_EFSConfig;





__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
SFFS_Handle hSFFS_open(SFFS_Handle handle);


#ifdef __cplusplus
}
#endif






#endif /* SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_H_ */
