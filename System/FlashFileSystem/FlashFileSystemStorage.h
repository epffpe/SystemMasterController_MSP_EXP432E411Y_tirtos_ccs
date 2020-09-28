/*
 * FlashFileSystemStorage.h
 *
 *  Created on: Jun 23, 2020
 *      Author: epffpe
 */

#ifndef SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_H_
#define SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_H_



/* SPIFFS configuration parameters */
#define SFFS_IFS_SPIFFS_LOGICAL_BLOCK_SIZE    (16384)
#define SFFS_IFS_SPIFFS_LOGICAL_PAGE_SIZE     (128)
#define SFFS_IFS_SPIFFS_FILE_DESCRIPTOR_SIZE  (44)

/* SPIFFS configuration parameters */
#define SFFS_EFS_SPIFFS_LOGICAL_BLOCK_SIZE    (16384)
#define SFFS_EFS_SPIFFS_LOGICAL_PAGE_SIZE     (128)
#define SFFS_EFS_SPIFFS_FILE_DESCRIPTOR_SIZE  (44)


#ifndef SFFS_SPIFFS_LOGICAL_PAGE_SIZE
#define SFFS_SPIFFS_LOGICAL_PAGE_SIZE           SFFS_IFS_SPIFFS_LOGICAL_PAGE_SIZE
#endif
#ifndef SFFS_SPIFFS_FILE_DESCRIPTOR_SIZE
#define SFFS_SPIFFS_FILE_DESCRIPTOR_SIZE        SFFS_IFS_SPIFFS_FILE_DESCRIPTOR_SIZE
#endif



typedef enum teSFFSInterfacesName {
    SFFS_Internal = 0,
    SFFS_External,

    SFFS_COUNT
} teSFFSInterfacesName;









typedef struct {
    Device_Params params;
    char description[IFS_FILE_DESCRIPTION_LENGTH];
    char data[];
}SFFS_deviceInfoFile_t;

typedef struct {
    const char *path;
    spiffs_flags flags;
    spiffs_mode mode;
    void *buf;
    s32_t len;
}SFFS_File_Params;



typedef struct SFFS_HWAttrs {
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
        bool            opened:1;
    } state;
    Semaphore_Handle hSFFS_semInitialized;
    GateMutexPri_Handle hSFFS_gateOpen;
    GateMutexPri_Handle hSFFS_gateWait;
    GateMutexPri_Handle hSFFS_gateMount;

    /*
     * SPIFFS needs RAM to perform operations on files.  It requires a work buffer
     * which MUST be (2 * LOGICAL_PAGE_SIZE) bytes.
     */
    uint8_t spiffsWorkBuffer[SFFS_SPIFFS_LOGICAL_PAGE_SIZE * 2];

    /* The array below will be used by SPIFFS as a file descriptor cache. */
    uint8_t spiffsFileDescriptorCache[SFFS_SPIFFS_FILE_DESCRIPTOR_SIZE * 4];

    /* The array below will be used by SPIFFS as a read/write cache. */
    uint8_t spiffsReadWriteCache[SFFS_SPIFFS_LOGICAL_PAGE_SIZE * 2];

}SFFS_Object;

typedef struct SFFS_Config *SFFS_Handle;


typedef void        (*SFFS_CloseFxn)    (SFFS_Handle handle);


typedef int         (*SFFS_ControlFxn)  (SFFS_Handle handle,
                                        unsigned int cmd,
                                        void *arg);
typedef int        (*SFFS_InitFxn)     (SFFS_Handle handle);

typedef SFFS_Handle  (*SFFS_OpenFxn)     (SFFS_Handle handle);

typedef struct SFFS_FxnTable {
    /*! Function to close the specified peripheral */
    SFFS_CloseFxn        closeFxn;

    /*! Function to implementation specific control function */
    SFFS_ControlFxn      controlFxn;

    /*! Function to initialize the given data object */
    SFFS_InitFxn         initFxn;

    /*! Function to open the specified peripheral */
    SFFS_OpenFxn         openFxn;

} SFFS_FxnTable;


typedef struct SFFS_Config{
    SFFS_FxnTable const *fxnTablePtr;
    SFFS_HWAttrs  const *hwAttrs;
    SFFS_Object     *object;
}SFFS_Config;



typedef struct {
    void *arg0;
}SFFS_Params;



#ifdef  __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_GLOBAL
    #define __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
#else
    #define __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern const SFFS_Config SFFS_config[];

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
Task_Handle hSFFS_formatHeartBeat_init();


__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_init(void);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_FileParams_init(SFFS_File_Params *pParams, const char *path, void *buf, s32_t len);


__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_FileParams_initForRead(SFFS_File_Params *pParams, const char *path, void *buf, s32_t len);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_FileParams_initForWrite(SFFS_File_Params *pParams, const char *path, void *buf, s32_t len);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_close(SFFS_Handle handle);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_control(SFFS_Handle handle, unsigned int cmd, void* arg);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
SFFS_Handle hSFFS_open(unsigned int index);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_read(SFFS_Handle handle, const char *path, void *buf, s32_t len, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_write(SFFS_Handle handle, const char *path, void *buf, s32_t len, unsigned int timeout);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xStorageFFS_write(SFFS_Handle handle, SFFS_File_Params *pParams, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xStorageFFS_read(SFFS_Handle handle, SFFS_File_Params *pParams, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_readDevicesConfiguration(SFFS_Handle handle, unsigned int timeout);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int32_t xSFFS_getStatus(SFFS_Handle handle, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int32_t xSFFS_format(SFFS_Handle handle, unsigned int timeout);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_getFlashDeviceListEthernet(SFFS_Handle handle, int clientfd, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_getFlashReadFileNameEthernet(SFFS_Handle handle, int clientfd, char *fileName, unsigned int timeout);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_setFlashDataFileNameEthernet(SFFS_Handle handle, int clientfd, char *payload, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_removeFileNameEthernet(SFFS_Handle handle, int clientfd, char *payload, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
void vSFFS_getFlashConfigurationFileEthernet(SFFS_Handle handle, int clientfd, unsigned int timeout);

__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_getFlashDataFileNameHTTP(SFFS_Handle handle, int clientfd, char *fileName, unsigned int timeout);
__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_setFlashDataFileNameHTTP(SFFS_Handle handle, int clientfd, char *fileName, int contentLength, unsigned int timeout);


__SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_EXT
int xSFFS_lockMemoryForReboot(SFFS_Handle handle, unsigned int timeout);


#ifdef __cplusplus
}
#endif






#endif /* SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_H_ */
