/*
 * FlashFileSystemStorage.c
 *
 *  Created on: Jun 23, 2020
 *      Author: epffpe
 */

#define __SYSTEM_FLASHFILESYSTEM_FLASHFILESYSTEMSTORAGE_GLOBAL
#include "includes.h"

#undef htonl
#undef htons
#undef ntohl
#undef ntohs



/* BSD support */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <ti/net/slnetutils.h>

#include <ti/net/http/httpserver.h>
#include <ti/net/http/http.h>
//#include <ti/net/http/logging.h>
#include "HTTPServer/urlsimple.h"
#include "HTTPServer/URLHandler/URLHandler.h"

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();




/* Used to check status and initialization */
static int g_SFFS_count = -1;



int xStorageFFS_init(SFFS_Handle handle);
SFFS_Handle hStorageFFS_open(SFFS_Handle handle);
bool bSFFS_waitForInit(SFFS_Handle handle, unsigned int timeout);
bool bStorageFFS_mount(SFFS_Handle handle, unsigned int timeout);
bool bStorageFFS_unmount(SFFS_Handle handle, unsigned int timeout);
int xStorageFFS_write(SFFS_Handle handle, SFFS_File_Params *pParams, unsigned int timeout);
int xStorageFFS_read(SFFS_Handle handle, SFFS_File_Params *pParams, unsigned int timeout);



SFFS_Object sffsObject[SFFS_COUNT];

const SFFS_FxnTable SFFS_fxnTable =
{
 .closeFxn = NULL,
 .controlFxn = NULL,
 .initFxn = xStorageFFS_init,
 .openFxn  = hStorageFFS_open
};


const SFFS_HWAttrs sffsHWattrs[SFFS_COUNT] =
{
 {
  .nvsIndex = Board_NVSINTERNAL,
  .logicalBlockSize = SFFS_IFS_SPIFFS_LOGICAL_BLOCK_SIZE,
  .logicalPageSize = SFFS_IFS_SPIFFS_LOGICAL_PAGE_SIZE,
 },
 {
  .nvsIndex = Board_NVSEXTERNAL,
  .logicalBlockSize = SFFS_EFS_SPIFFS_LOGICAL_BLOCK_SIZE,
  .logicalPageSize = SFFS_EFS_SPIFFS_LOGICAL_PAGE_SIZE,
 }
};


const SFFS_Config SFFS_config[] =
{
 {
  .fxnTablePtr = &SFFS_fxnTable,
  .hwAttrs = &sffsHWattrs[0],
  .object = &sffsObject[0],
 },
 {
  .fxnTablePtr = &SFFS_fxnTable,
  .hwAttrs = &sffsHWattrs[1],
  .object = &sffsObject[1],
 },
 {NULL, NULL, NULL},
};




int xStorageFFS_init(SFFS_Handle handle)
{
    int32_t        status;
    SFFS_Object *object = (SFFS_Object *)handle->object;
    Error_Block         eb;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    Error_init(&eb);
    object->state.mounted = false;
    object->state.initialized = false;

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&handle->object->spiffsnvsData,
                              handle->hwAttrs->nvsIndex,
                              &handle->object->fs,
                              &handle->object->fsConfig,
                              handle->hwAttrs->logicalBlockSize,
                              handle->hwAttrs->logicalPageSize);
    switch(status) {
    case SPIFFSNVS_STATUS_SUCCESS:
        object->state.initialized = true;
        object->hSFFS_gateOpen = GateMutexPri_create(NULL, &eb);
        if (object->hSFFS_gateOpen == NULL) {
            System_abort("Could not create SFFS Open gate");
        }
        break;
    case SPIFFSNVS_STATUS_INV_NVS_IDX:
    case SPIFFSNVS_STATUS_INV_BLOCK_SIZE:
    case SPIFFSNVS_STATUS_INV_PAGE_SIZE:
    default:
        Display_printf(g_SMCDisplay, 0, 0,
                       "Error with SPIFFS configuration.\n");
        break;
    }
    return status;
}




SFFS_Handle hStorageFFS_open(SFFS_Handle handle)
{
//    int32_t        status;
    unsigned int key;
    Error_Block         eb;
    Semaphore_Params    semParams;

    SFFS_Object *object = (SFFS_Object *)handle->object;
//    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;



    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }


    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(object->hSFFS_gateOpen);

    if(object->state.opened == true) {
        GateMutexPri_leave(object->hSFFS_gateOpen, key);
        return handle;
    }


    Error_init(&eb);


    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    Display_printf(g_SMCDisplay, 0, 0, "Loading Initial Configuration");


    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;

    object->hSFFS_semInitialized = Semaphore_create(0, &semParams, &eb);
    if (object->hSFFS_semInitialized == NULL) {
        System_abort("Can't create initialized semaphore");
    }
    Semaphore_post(object->hSFFS_semInitialized);

    object->hSFFS_gateMount = GateMutexPri_create(NULL, &eb);
    if (object->hSFFS_gateMount == NULL) {
        System_abort("Can't create gate");
    }

    object->hSFFS_gateWait = GateMutexPri_create(NULL, &eb);
    if (object->hSFFS_gateWait == NULL) {
        System_abort("Can't create gate");
    }

    object->state.opened = true;
    GateMutexPri_leave(object->hSFFS_gateOpen, key);
    return handle;
}


bool bSFFS_waitForInit(SFFS_Handle handle, unsigned int timeout)
{
    bool ret = true;
    unsigned int key;
    SFFS_Object *object = (SFFS_Object *)handle->object;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(object->hSFFS_gateWait);

    if (!object->state.initialized) {
        if (!Semaphore_pend(object->hSFFS_semInitialized, timeout)) {
            ret = false;
        }
    }
    GateMutexPri_leave(object->hSFFS_gateWait, key);
    return (ret);
}


bool bStorageFFS_mount(SFFS_Handle handle, unsigned int timeout)
{
    int32_t        status;
//    u32_t total, used;
//    int res;
    bool bretValue = false;
    SFFS_Object *object = (SFFS_Object *)handle->object;
//    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return false;
    }

    if(object->state.mounted) {
        return true;
    }



    Display_printf(g_SMCDisplay, 0, 0, "Mounting Internal Flash file system...");

    status = SPIFFS_mount(&object->fs,
                          &object->fsConfig,
                          object->spiffsWorkBuffer,
                          object->spiffsFileDescriptorCache, sizeof(object->spiffsFileDescriptorCache),
                          object->spiffsReadWriteCache, sizeof(object->spiffsReadWriteCache),
                          NULL);
    object->state.mounted = 1;
    bretValue = true;
    if (status != SPIFFS_OK) {
        object->state.mounted = 0;
        bretValue = false;
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(g_SMCDisplay, 0, 0,
                           "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&object->fs);
            status = SPIFFS_format(&object->fs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                               "Error formatting memory.\n");
                //                while (1);
            }
            status = SPIFFS_mount(&object->fs,
                                  &object->fsConfig,
                                  object->spiffsWorkBuffer,
                                  object->spiffsFileDescriptorCache, sizeof(object->spiffsFileDescriptorCache),
                                  object->spiffsReadWriteCache, sizeof(object->spiffsReadWriteCache),
                                  NULL);
            object->state.mounted = 1;
            bretValue = true;
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                               "Error mounting file system.\n");
                object->state.mounted = 0;
                bretValue = false;
            }

        }else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(g_SMCDisplay, 0, 0,
                           "Error mounting file system: %d.\n", status);
            //            while (1);
        }
    }
    return bretValue;
}

bool bStorageFFS_unmount(SFFS_Handle handle, unsigned int timeout)
{
//    int32_t        status;
//    u32_t total, used;
//    int res;
    bool bretValue = false;
    SFFS_Object *object = (SFFS_Object *)handle->object;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return false;
    }

    if(object->state.mounted) {
        SPIFFS_unmount(&object->fs);
        object->state.mounted = 0;
        bretValue = true;
    }
    return bretValue;

}

int xStorageFFS_write(SFFS_Handle handle, SFFS_File_Params *pParams, unsigned int timeout)
{
    int i32retValue = 0;
    unsigned int key;
    spiffs_file    fd;
    SFFS_Object *object = (SFFS_Object *)handle->object;
//    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);
    ASSERT(pParams != NULL);
    ASSERT(pParams->path != NULL);
    ASSERT(pParams->buf != NULL);

    if (handle == NULL) {
        return (NULL);
    }
    if (pParams == NULL) {
        return (NULL);
    }
    if (pParams->path == NULL) {
        return (NULL);
    }
    if (pParams->buf == NULL) {
        return (NULL);
    }

    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout)) {
                    fd = SPIFFS_open(&object->fs,
                                     pParams->path,
                                     pParams->flags | SPIFFS_RDWR,
                                     pParams->mode);
//                    fd = SPIFFS_open(&object->fs,
//                                     pParams->path,
//                                     SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_TRUNC,
//                                     0);
                    if (fd < 0) {
                        Display_printf(g_SMCDisplay, 0, 0, "Error creating %s.\n", pParams->buf);
                        i32retValue = -1;
                    }else{
                        i32retValue = SPIFFS_write(&object->fs, fd,
                                                    pParams->buf, pParams->len);
                        SPIFFS_fflush(&object->fs, fd);
                        SPIFFS_close(&object->fs, fd);
                        if (i32retValue < 0)
                        {
                            Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", pParams->buf);
                        }
                    }

                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

    return i32retValue;
}


int xStorageFFS_read(SFFS_Handle handle, SFFS_File_Params *pParams, unsigned int timeout)
{
    int i32retValue = 0;
    unsigned int key;
    spiffs_file    fd;
    SFFS_Object *object = (SFFS_Object *)handle->object;
//    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);
    ASSERT(pParams != NULL);
    ASSERT(pParams->path != NULL);
    ASSERT(pParams->buf != NULL);

    if (handle == NULL) {
        return (NULL);
    }
    if (pParams == NULL) {
        return (NULL);
    }
    if (pParams->path == NULL) {
        return (NULL);
    }
    if (pParams->buf == NULL) {
        return (NULL);
    }

    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout)) {
                    fd = SPIFFS_open(&object->fs,
                                     pParams->path,
                                     pParams->flags | SPIFFS_RDONLY,
                                     pParams->mode);
//                    fd = SPIFFS_open(&object->fs,
//                                     pParams->path,
//                                     SPIFFS_RDONLY,
//                                     pParams->mode);
                    if (fd < 0) {
                        Display_printf(g_SMCDisplay, 0, 0, "Error creating %s.\n", pParams->buf);
                        i32retValue = -1;
                    }else{
                        i32retValue = SPIFFS_read(&object->fs, fd,
                                                  pParams->buf, pParams->len);
                        SPIFFS_close(&object->fs, fd);

                        if (i32retValue < 0)
                        {
                            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", pParams->buf);
                        }
                    }

                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

    return i32retValue;
}




/*********************************************************************************************
 *
 *********************************************************************************************/

void vSFFS_FileParams_init(SFFS_File_Params *pParams, const char *path, void *buf, s32_t len)
{
    pParams->path = path;
    pParams->buf = buf;
    pParams->len = len;
    pParams->mode = 0;
}

void vSFFS_FileParams_initForRead(SFFS_File_Params *pParams, const char *path, void *buf, s32_t len)
{
    vSFFS_FileParams_init(pParams, path, buf, len);
    pParams->flags = SPIFFS_RDONLY;
}


void vSFFS_FileParams_initForWrite(SFFS_File_Params *pParams, const char *path, void *buf, s32_t len)
{
    vSFFS_FileParams_init(pParams, path, buf, len);
    pParams->flags = SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_TRUNC;
    pParams->mode = 0;
}


void vSFFS_init(void)
{
    Display_printf(g_SMCDisplay, 0, 0, "Initializing SFFS Interfaces\n");
    if (g_SFFS_count == -1) {
        /* Call each driver's init function */
        for (g_SFFS_count = 0; SFFS_config[g_SFFS_count].fxnTablePtr != NULL; g_SFFS_count++) {
            if (SFFS_config[g_SFFS_count].fxnTablePtr->initFxn) {
                SFFS_config[g_SFFS_count].fxnTablePtr->initFxn((SFFS_Handle)&(SFFS_config[g_SFFS_count]));
            }
        }
    }
}

void vSFFS_close(SFFS_Handle handle)
{
    if (handle->fxnTablePtr->closeFxn) {
        handle->fxnTablePtr->closeFxn(handle);
    }
}

int xSFFS_control(SFFS_Handle handle, unsigned int cmd, void* arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}


SFFS_Handle hSFFS_open(unsigned int index)
{
    SFFS_Handle handle;

    if (index >= SFFS_COUNT) {
        return (NULL);
    }

    /* Get handle for this driver instance */
    handle = (SFFS_Handle)&(SFFS_config[index]);

    if (handle->fxnTablePtr->openFxn) {
        return (handle->fxnTablePtr->openFxn(handle));
    }
    return 0;
}

int xSFFS_read(SFFS_Handle handle, const char *path, void *buf, s32_t len, unsigned int timeout)
{
    SFFS_File_Params sffsParams;
    vSFFS_FileParams_initForRead(&sffsParams, path, buf, len);
    return xStorageFFS_read(handle, &sffsParams, timeout);
}


int xSFFS_write(SFFS_Handle handle, const char *path, void *buf, s32_t len, unsigned int timeout)
{
    SFFS_File_Params sffsParams;
    vSFFS_FileParams_initForWrite(&sffsParams, path, buf, len);
    return xStorageFFS_write(handle, &sffsParams, timeout);
}



int32_t xSFFS_getStatus(SFFS_Handle handle, unsigned int timeout)
{
    int32_t        status;
    unsigned int key;
//    SFFS_Handle handle;
    SFFS_Object *object;

//    if (index >= SFFS_COUNT) {
//        return (NULL);
//    }

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return -1;
    }

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    /* Get handle for this driver instance */
//    handle = (SFFS_Handle)&(SFFS_config[index]);
    object = (SFFS_Object *)handle->object;


    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {

                status = SPIFFS_mount(&object->fs,
                                      &object->fsConfig,
                                      object->spiffsWorkBuffer,
                                      object->spiffsFileDescriptorCache, sizeof(object->spiffsFileDescriptorCache),
                                      object->spiffsReadWriteCache, sizeof(object->spiffsReadWriteCache),
                                      NULL);
                object->state.mounted = 1;
                if (status != SPIFFS_OK) {
                    object->state.mounted = 0;
                }


                SPIFFS_unmount(&object->fs);
                object->state.mounted = 0;

            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
        status = -1;
    }

    /**
     * Runs a consistency check on given filesystem.
     * @param fs            the file system struct
     */
//    s32_t SPIFFS_check(spiffs *fs);
//    status = SPIFFS_check(&g_IFSfs);
    return status;
}


int32_t xSFFS_format(SFFS_Handle handle, unsigned int timeout)
{
    int32_t        status;
    unsigned int key;
//    SFFS_Handle handle;
    SFFS_Object *object;


//    if (index >= SFFS_COUNT) {
//        return (NULL);
//    }

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return -1;
    }
    /* Get handle for this driver instance */
//    handle = (SFFS_Handle)&(SFFS_config[index]);
    object = (SFFS_Object *)handle->object;

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);



    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {

                SPIFFS_unmount(&object->fs);
                status = SPIFFS_format(&object->fs);
                if (status != SPIFFS_OK) {
                    Display_printf(g_SMCDisplay, 0, 0,
                                   "Error formatting memory.\n");
                }
                status = SPIFFS_mount(&object->fs,
                                      &object->fsConfig,
                                      object->spiffsWorkBuffer,
                                      object->spiffsFileDescriptorCache, sizeof(object->spiffsFileDescriptorCache),
                                      object->spiffsReadWriteCache, sizeof(object->spiffsReadWriteCache),
                                      NULL);
                object->state.mounted = 1;

                if (status != SPIFFS_OK) {
                    Display_printf(g_SMCDisplay, 0, 0,
                                   "Error mounting file system.\n");
                    object->state.mounted = 0;
                }

                SPIFFS_unmount(&object->fs);
                object->state.mounted = 0;
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
        status = -1;
    }

    return status;
}

int xSFFS_readDevicesConfiguration(SFFS_Handle handle, unsigned int timeout)
{
    int i32retValue = 0;
    unsigned int key;
    spiffs_file    fd;
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;
    Error_Block eb;
    Device_Params deviceParams;
    int res;
    IFS_deviceInfoFile_t devInfo;

    SFFS_Object *object = (SFFS_Object *)handle->object;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    Error_init(&eb);


    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout)) {

                    SPIFFS_opendir(&object->fs, "/", &d);
                    while ((pe = SPIFFS_readdir(&d, pe))) {
                        Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i", pe->name, pe->obj_id, pe->size);
                        //        if (0 == strncmp(EFS_STARTUP_CONF_FILE_NAME, (char *)pe->name, strlen(EFS_STARTUP_CONF_FILE_NAME))) {
                        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                            //            Display_printf(g_SMCDisplay, 0, 0, "* Device found");
                            if (pe->size >= sizeof(IFS_deviceInfoFile_t)) {
                                /*********************************************************************/
                                /* Do note that if the any file is modified (except for fully removing)
                                 * within the dirent iterator, the iterator may become invalid.
                                 */
                                /*********************************************************************/
                                fd = SPIFFS_open_by_dirent(&object->fs, pe, SPIFFS_RDWR, 0);
                                if (fd < 0) {
                                    Display_printf(g_SMCDisplay, 0, 0, "Error SPIFFS_open_by_dirent %i.\n", SPIFFS_errno(&object->fs));
                                    i32retValue = -1;
                                }else{
                                    if (SPIFFS_read(&object->fs, fd, (void *)&devInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
                                        Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
                                        i32retValue = -2;
                                    }
                                    res = SPIFFS_close(&object->fs, fd);
                                    if (res < 0) {
                                        Display_printf(g_SMCDisplay, 0, 0, "errno %i\n", SPIFFS_errno(&object->fs));
                                        i32retValue = -3;
                                    }
                                    if (devInfo.params.deviceType < DEVICE_TYPE_COUNT) {
                                        Display_printf(g_SMCDisplay, 0, 0,
                                                       "-> Loading Device: %s\n"
                                                       "   Type: %s\n"
                                                       "   DeviceId: %d"
                                                       , devInfo.description,
                                                       g_Device_nameByType[devInfo.params.deviceType],
                                                       devInfo.params.deviceID);
                                        //                    Display_printf(g_SMCDisplay, 0, 0, "   Type: %s", g_Device_nameByType[devInfo.params.deviceType]);
                                        //                    Display_printf(g_SMCDisplay, 0, 0, "   DeviceId: %d", devInfo.params.deviceID);
                                        switch(devInfo.params.deviceType) {
                                        case DEVICE_TYPE_ALTO_AMP:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vALTOAmpDevice_Params_init(&deviceParams, devInfo.params.deviceID);
                                            deviceParams.arg0 = devInfo.params.arg0;
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_ALTO_MULTINET:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vALTOMultinetDevice_Params_init(&deviceParams, devInfo.params.deviceID);
                                            deviceParams.arg0 = devInfo.params.arg0;
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_ALTO_FORTE_MANAGER:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vForteManagerDevice_Params_init(&deviceParams, devInfo.params.deviceID, (uint32_t)devInfo.params.arg0);
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_TF_UART_TEST:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vTFUartTestDevice_Params_init(&deviceParams, devInfo.params.deviceID, (uint32_t)devInfo.params.arg0);
                                            if (devInfo.params.timeout) deviceParams.timeout = devInfo.params.timeout;
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_DUT_UART_TEST:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vDUTUartTestDevice_Params_init(&deviceParams, devInfo.params.deviceID, (uint32_t)devInfo.params.arg0);
                                        //    deviceParams.period = 1000;
                                        //    deviceParams.timeout = 100;
                                            if (devInfo.params.period) deviceParams.period = devInfo.params.period;
                                            if (devInfo.params.timeout) deviceParams.timeout = devInfo.params.timeout;
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_AVDS:
                                            vAVDSDevice_Params_init(&deviceParams, devInfo.params.deviceID);
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_AVDS485:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vAVDS485Device_Params_init(&deviceParams, devInfo.params.deviceID);
                                            deviceParams.arg0 = devInfo.params.arg0;
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_ROSEN:
                                            vRosenDevice_Params_init(&deviceParams, devInfo.params.deviceID);
                                            xDevice_add(&deviceParams, &eb);
                                            break;
                                        case DEVICE_TYPE_ROSEN485:
                                            Display_printf(g_SMCDisplay, 0, 0, "   Serial Port: %d", (uint32_t)devInfo.params.arg0);
                                            vRosen485Device_Params_init(&deviceParams, devInfo.params.deviceID);
                                            deviceParams.arg0 = devInfo.params.arg0;
                                            xDevice_add(&deviceParams, &eb);
                                            break;



                                        case DEVICE_TYPE_USB_REMOTE_CONTROL_BINARY:
//                                            vUSBRCBinaryDevice_Params_init(&deviceParams, devInfo.params.deviceID);
//                                            xDevice_add(&deviceParams, &eb);
//                                            break;
                                        case DEVICE_TYPE_USB_REMOTE_CONTROL_CONSOLE:
//                                            vUSBConsoleDevice_Params_init(&deviceParams, devInfo.params.deviceID);
//                                            xDevice_add(&deviceParams, &eb);
//                                            break;
                                        case DEVICE_TYPE_TCP_REMOTE_CONTROLLER_BINARY:
                                        case DEVICE_TYPE_TCP_REMOTE_CONTROLLER_ASCII:
                                        case DEVICE_TYPE_GENERIC:
                                        default:
                                            Display_printf(g_SMCDisplay, 0, 0, "***Couldn't load device");
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                    }
                    SPIFFS_closedir(&d);

                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

    return i32retValue;
}


void vSFFS_getFlashDeviceListEthernet(SFFS_Handle handle, int clientfd, unsigned int timeout)
{
    spiffs_file    fd;
    uint32_t bufferSize;
    int res;
    uint32_t counter = 0;
    spiffs_DIR d, d2;
    struct spiffs_dirent e, e2;
    struct spiffs_dirent *pe = &e, *pe2 = &e2;
    Error_Block eb;
    IFS_deviceInfoFile_t devInfo;


    unsigned int key;
    SFFS_Object *object;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }

    object = (SFFS_Object *)handle->object;

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

    Error_init(&eb);


    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout))
                {

                    SPIFFS_opendir(&object->fs, "/", &d);
                    while ((pe = SPIFFS_readdir(&d, pe))) {
                        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                            if (pe->size >= sizeof(IFS_deviceInfoFile_t)) {
                                counter++;
                            }
                        }
                    }
                    SPIFFS_closedir(&d);


                    bufferSize = sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_SystemControl_RAM_deviceList_payload_t) + counter * sizeof(TCPBin_CMD_SystemControl_devicesList_t);
                    char *pBuffer = Memory_alloc(NULL, bufferSize, 0, &eb);

                    if (pBuffer !=NULL) {

                        TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)pBuffer;
                        pFrame->type = TCP_CMD_System_getFlashDeviceListResponse | 0x80000000;
                        pFrame->retDeviceID = TCPRCBINDEVICE_ID;
                        pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
                        pFrame->retParamID = 4;

                        TCPBin_CMD_SystemControl_RAM_deviceList_payload_t *pFramePayload = (TCPBin_CMD_SystemControl_RAM_deviceList_payload_t *)pFrame->payload;
                        pFramePayload->numbOfDevices = counter;

                        TCPBin_CMD_SystemControl_devicesList_t *pDeviceListElem = (TCPBin_CMD_SystemControl_devicesList_t *)pFramePayload->payload;


                        /*
                         * Fill information
                         */

                        SPIFFS_opendir(&object->fs, "/", &d2);
                        while ((pe2 = SPIFFS_readdir(&d2, pe2))) {
                            //                Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i", pe->name, pe->obj_id, pe->size);

                            if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe2->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                                if (pe2->size >= sizeof(IFS_deviceInfoFile_t)) {
                                    /*********************************************************************/
                                    /* Do note that if the any file is modified (except for fully removing)
                                     * within the dirent iterator, the iterator may become invalid.
                                     */
                                    /*********************************************************************/


                                    fd = SPIFFS_open_by_dirent(&object->fs, pe2, SPIFFS_RDWR, 0);
                                    if (fd >= 0) {
                                        if (SPIFFS_read(&object->fs, fd, (void *)&devInfo, sizeof(IFS_deviceInfoFile_t)) >= 0) {
                                            res = SPIFFS_close(&object->fs, fd);
                                            if (res == 0) {
                                                pDeviceListElem->deviceID = devInfo.params.deviceID;
                                                pDeviceListElem->deviceType = devInfo.params.deviceType;
                                                memcpy(pDeviceListElem->fileName, (char *)pe2->name, IFS_FILE_NAME_LENGTH);
                                                memcpy(pDeviceListElem->description, devInfo.description, IFS_FILE_DESCRIPTION_LENGTH);

                                            }
                                        }
                                    }
                                    pDeviceListElem++;

                                }
                            }


                        }
                        SPIFFS_closedir(&d2);

                        send(clientfd, pBuffer, bufferSize, 0);

                        Memory_free(NULL, pBuffer, bufferSize);

                    }


                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

}


void vSFFS_getFlashReadFileNameEthernet(SFFS_Handle handle, int clientfd, char *fileName, unsigned int timeout)
{
    spiffs_file    fd;
    uint32_t bufferSize;
    int res;
    spiffs_stat s;
    Error_Block eb;

    unsigned int key;
    SFFS_Object *object;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }

    object = (SFFS_Object *)handle->object;

    //    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

    Error_init(&eb);


    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout))
                {

                    //        Display_printf(g_SMCDisplay, 0, 0, "SPIFFS_stat... %s", fileName);

                    res = SPIFFS_stat(&object->fs, fileName, &s);
                    if (res >= 0) {
                        Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i\n", s.name, s.obj_id, s.size);

                        bufferSize = sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_SystemControl_FlashFileData_payload_t) + s.size;
                        char *pBuffer = Memory_alloc(NULL, bufferSize, 0, &eb);

                        if (pBuffer !=NULL) {

                            TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)pBuffer;
                            pFrame->type = TCP_CMD_System_getFlashFileDataResponse | 0x80000000;
                            pFrame->retDeviceID = TCPRCBINDEVICE_ID;
                            pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
                            pFrame->retParamID = 4;

                            TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFramePayload = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)pFrame->payload;
                            pFramePayload->fileSize = s.size;
                            if (strlen((const char *)s.name) < IFS_FILE_NAME_LENGTH) {
                                strcpy((char *)pFramePayload->fileName, (char *)s.name);
                            }else {
                                memcpy(pFramePayload->fileName, s.name, IFS_FILE_NAME_LENGTH);
                            }
                            /*
                             * Fill information
                             */

                            fd = SPIFFS_open(&object->fs, fileName, SPIFFS_RDONLY, 0);
                            if (fd >= 0) {
                                //                    Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", fileName);

                                /* spiffsFile exists; read its contents & delete the file */
                                if (SPIFFS_read(&object->fs, fd, pFramePayload->payload, s.size) > 0) {
                                    //                        Display_printf(g_SMCDisplay, 0, 0, "--> %s", pFramePayload->payload);
                                }

                                //                    Display_printf(g_SMCDisplay, 0, 0, "closing %s...", fileName);
                                SPIFFS_close(&object->fs, fd);

                                send(clientfd, pBuffer, bufferSize, 0);
                            }


                            Memory_free(NULL, pBuffer, bufferSize);

                        }
                    }else{
                        Display_printf(g_SMCDisplay, 0, 0, "**Error reading file: %s**\n", fileName);
                    }

                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

}





void vSFFS_setFlashDataFileNameEthernet(SFFS_Handle handle, int clientfd, char *payload, unsigned int timeout)
{
    spiffs_file    fd;
    char *fileName;
    Error_Block eb;

    unsigned int key;
    SFFS_Object *object;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }
    object = (SFFS_Object *)handle->object;



    TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFPayload = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)payload;
    IFS_deviceInfoFile_t *pDeviceInfo = (IFS_deviceInfoFile_t *)pFPayload->payload;
    fileName = pFPayload->fileName;
    fileName[IFS_FILE_NAME_LENGTH - 1] = 0;

    Error_init(&eb);



    if (pFPayload->fileSize >= sizeof(IFS_deviceInfoFile_t)) {
        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, fileName, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {

            if (object->state.initialized) {
                if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

                    key = GateMutexPri_enter(object->hSFFS_gateMount);
                    {
                        if (bStorageFFS_mount(handle, timeout))
                        {
                            fd = SPIFFS_open(&object->fs, fileName, SPIFFS_CREAT | SPIFFS_RDWR, 0);
                            if (fd >= 0) {


                                if (SPIFFS_write(&object->fs, fd, (void *) pDeviceInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
                                    Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_DEVICE_1_FILE_NAME);
                                }

                                SPIFFS_close(&object->fs, fd);

                                //                send(clientfd, pBuffer, bufferSize, 0);

                            }
                            bStorageFFS_unmount(handle, timeout);
                        }
                    }
                    GateMutexPri_leave(object->hSFFS_gateMount, key);

                    Semaphore_post(object->hSFFS_semInitialized);
                }
            }else {
                bSFFS_waitForInit(handle, timeout);
            }
        }
    }
}



void vSFFS_removeFileNameEthernet(SFFS_Handle handle, int clientfd, char *payload, unsigned int timeout)
{
    int res;
    char *fileName;
    Error_Block eb;

    unsigned int key;
    SFFS_Object *object;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }
    object = (SFFS_Object *)handle->object;


    TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFPayload = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)payload;
    fileName = pFPayload->fileName;
    fileName[IFS_FILE_NAME_LENGTH - 1] = 0;

    Error_init(&eb);

    if (pFPayload->fileSize == 0) {
        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, fileName, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {

            if (object->state.initialized) {
                if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

                    key = GateMutexPri_enter(object->hSFFS_gateMount);
                    {
                        if (bStorageFFS_mount(handle, timeout))
                        {

                            res = SPIFFS_remove(&object->fs, fileName);
                            if (res < 0) {
                                Display_printf(g_SMCDisplay, 0, 0, "errno %i\n", SPIFFS_errno(&object->fs));
                            }else {
                                //                send(clientfd, pBuffer, bufferSize, 0);
                            }

                            bStorageFFS_unmount(handle, timeout);
                        }
                    }
                    GateMutexPri_leave(object->hSFFS_gateMount, key);

                    Semaphore_post(object->hSFFS_semInitialized);
                }
            }else {
                bSFFS_waitForInit(handle, timeout);
            }
        }
    }
}


int xSFFS_lockMemoryForReboot(SFFS_Handle handle, unsigned int timeout)
{
    int i32retValue = 0;
    unsigned int key;
    SFFS_Object *object;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    object = (SFFS_Object *)handle->object;

    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                i32retValue = 1;
            }
//            GateMutexPri_leave(object->hSFFS_gateMount, key);
//
//            Semaphore_post(object->hSFFS_semInitialized);
            key = key;
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
        i32retValue = -1;
    }

    return i32retValue;
}



void vSFFS_getFlashConfigurationFileEthernet(SFFS_Handle handle, int clientfd, unsigned int timeout)
{
    spiffs_file    fd;
    uint32_t bufferSize;
    int res;
    uint32_t counter = 0, index;
    spiffs_DIR d, d2;
    struct spiffs_dirent e, e2;
    struct spiffs_dirent *pe = &e, *pe2 = &e2;
    IFS_deviceInfoFile_t devInfo;
    Error_Block eb;

    unsigned int key;
    SFFS_Object *object;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return;
    }
    object = (SFFS_Object *)handle->object;

    Error_init(&eb);


    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout))
                {

                    SPIFFS_opendir(&object->fs, "/", &d);
                    while ((pe = SPIFFS_readdir(&d, pe))) {
                        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                            if (pe->size >= sizeof(IFS_deviceInfoFile_t)) {
                                counter++;
                            }
                        }
                    }
                    SPIFFS_closedir(&d);

                    bufferSize = sizeof(TCPBin_CMD_retFrame_t) +
                            sizeof(TCPBin_CMD_SystemControl_ConfigurationFileFixed_payload_t) +
                            sizeof(TCPBin_CMD_SystemControl_RAM_deviceList_payload_t) +
                            counter * (sizeof(TCPBin_CMD_SystemControl_FlashFileData_payload_t) + sizeof(IFS_deviceInfoFile_t));

                    char *pBuffer = Memory_alloc(NULL, bufferSize, 0, &eb);

                    if (pBuffer !=NULL) {
                        TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)pBuffer;
                        pFrame->type = TCP_CMD_System_getConfigurationFileResponse | 0x80000000;
                        pFrame->retDeviceID = TCPRCBINDEVICE_ID;
                        pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_DISCRETEIO_CLASS_RETURN_UUID;
                        pFrame->retParamID = 4;

                        TCPBin_CMD_SystemControl_ConfigurationFileFixed_payload_t *pFramePayload = (TCPBin_CMD_SystemControl_ConfigurationFileFixed_payload_t *)pFrame->payload;

                        for (index = 0; index < IFS_NUMBER_OF_CONFIG_GPIO; index++) {
                            pFramePayload->gpioConfig[index].index = index;
                            pFramePayload->gpioConfig[index].pinConfig = g_sEEPROMDIOCfgData.dioCfg[index];
                            switch (pFramePayload->gpioConfig[index].pinConfig & GPIO_CFG_IO_MASK & 0x00f70000) {
                            case GPIO_CFG_IN_NOPULL:
                            case GPIO_CFG_IN_PU:
                            case GPIO_CFG_IN_PD:
                                pFramePayload->gpioConfig[index].DIVal = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIVal;
                                pFramePayload->gpioConfig[index].DIDebounceDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIDebounceDly;
                                pFramePayload->gpioConfig[index].DIRptStartDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIRptStartDly;
                                pFramePayload->gpioConfig[index].DIRptDly = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIRptDly;
                                pFramePayload->gpioConfig[index].DIIn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIIn;
                                pFramePayload->gpioConfig[index].DIBypassEn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIBypassEn;
                                pFramePayload->gpioConfig[index].DIModeSel = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIModeSel;
                                pFramePayload->gpioConfig[index].DIDebounceEn = DITbl[g_TCPRCBin_DiscreteIO_DIMapTable[index]].DIDebounceEn;
                                break;
                            case GPIO_CFG_OUT_STD:
                            case GPIO_CFG_OUT_OD_NOPULL:
                            case GPIO_CFG_OUT_OD_PU:
                            case GPIO_CFG_OUT_OD_PD:
                                pFramePayload->gpioConfig[index].DOA = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOA;
                                pFramePayload->gpioConfig[index].DOB = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOB;
                                pFramePayload->gpioConfig[index].DOBCtr = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBCtr;
                                pFramePayload->gpioConfig[index].DOSyncCtrMax = xDOGetSyncCtrMax();
                                pFramePayload->gpioConfig[index].DOOut = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOOut;
                                pFramePayload->gpioConfig[index].DOCtrl = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOCtrl;
                                pFramePayload->gpioConfig[index].DOBypass = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBypass;
                                pFramePayload->gpioConfig[index].DOBypassEn = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBypassEn;
                                pFramePayload->gpioConfig[index].DOModeSel = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOModeSel;
                                pFramePayload->gpioConfig[index].DOBlinkEnSel = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOBlinkEnSel;
                                pFramePayload->gpioConfig[index].DOInv = DOTbl[g_TCPRCBin_DiscreteIO_DOMapTable[index]].DOInv;
                                break;
                            default:
                                break;
                            }
                        }


                        TCPBin_CMD_SystemControl_RAM_deviceList_payload_t *pDynamicFramePayload = (TCPBin_CMD_SystemControl_RAM_deviceList_payload_t *)pFramePayload->dynamicPayload;
                        pDynamicFramePayload->numbOfDevices = counter;

                        //            TCPBin_CMD_SystemControl_devicesList_t *pDeviceListElem = (TCPBin_CMD_SystemControl_devicesList_t *)pDynamicFramePayload->payload;
                        TCPBin_CMD_SystemControl_FlashFileData_payload_t *pDeviceListElem = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)pDynamicFramePayload->payload;
                        IFS_deviceInfoFile_t *pDevInfo;

                        /*
                         * Fill information
                         */

                        SPIFFS_opendir(&object->fs, "/", &d2);
                        while ((pe2 = SPIFFS_readdir(&d2, pe2))) {
                            //                Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i", pe->name, pe->obj_id, pe->size);

                            if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe2->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                                if (pe2->size >= sizeof(IFS_deviceInfoFile_t)) {
                                    /*********************************************************************/
                                    /* Do note that if the any file is modified (except for fully removing)
                                     * within the dirent iterator, the iterator may become invalid.
                                     */
                                    /*********************************************************************/


                                    fd = SPIFFS_open_by_dirent(&object->fs, pe2, SPIFFS_RDONLY, 0);
                                    if (fd >= 0) {
                                        if (SPIFFS_read(&object->fs, fd, (void *)&devInfo, sizeof(IFS_deviceInfoFile_t)) >= 0) {
                                            res = SPIFFS_close(&object->fs, fd);
                                            if (res == 0) {
                                                pDeviceListElem->fileSize = pe2->size;
                                                memcpy(pDeviceListElem->fileName, (char *)pe2->name, IFS_FILE_NAME_LENGTH);

                                                pDevInfo = (IFS_deviceInfoFile_t *)pDeviceListElem->payload;

                                                memcpy(pDevInfo->description, devInfo.description, IFS_FILE_DESCRIPTION_LENGTH);
                                                pDevInfo->params = devInfo.params;

                                                //                                    pDeviceListElem->deviceType = devInfo.params.deviceType;

                                            }
                                        }
                                    }
                                    pDeviceListElem = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)pDevInfo->data;

                                }
                            }


                        }
                        SPIFFS_closedir(&d2);

                        send(clientfd, pBuffer, bufferSize, 0);

                        Memory_free(NULL, pBuffer, bufferSize);

                    }

                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

}



int xSFFS_getFlashDataFileNameHTTP(SFFS_Handle handle, int clientfd, char *fileName, unsigned int timeout)
{
    spiffs_file     fd;
    spiffs_stat     s;
    int             fileSize;
    int             retVal = 0;
    int contentRead;
    const char *contentType   = "text/plain"; /* default, but can be overridden */
    Error_Block eb;

    unsigned int key;
    SFFS_Object *object;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return -1;
    }
    object = (SFFS_Object *)handle->object;

    Error_init(&eb);


    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {

            key = GateMutexPri_enter(object->hSFFS_gateMount);
            {
                if (bStorageFFS_mount(handle, timeout))
                {

                    fd = SPIFFS_open(&object->fs, fileName, SPIFFS_RDONLY, 0);
                    if (fd >= 0) {

                        SPIFFS_stat(&object->fs, fileName, &s);
                        Display_printf(g_SMCDisplay, 0, 0, "Sending -> %s [%04x] size:%i\n", s.name, s.obj_id, s.size);


                        fileSize = s.size;
                        contentType = getContentType(fileName);

                        if (fileSize > 0) {
                            HTTPServer_sendSimpleResponse(clientfd, HTTP_SC_OK, contentType, fileSize, NULL);
                            char *buffRead = Memory_alloc(NULL, EFS_MAXIMUM_MEMORY_ALLOCATED, 0, &eb);
                            if (buffRead != NULL) {
                                Display_printf(g_SMCDisplay, 0, 0, "<--------------------------------->\n->");
                                while (fileSize > 0) {
                                    if (fileSize < EFS_MAXIMUM_MEMORY_ALLOCATED) {
                                        contentRead = SPIFFS_read(&object->fs, fd, buffRead, fileSize);
                                    }else {
                                        contentRead = SPIFFS_read(&object->fs, fd, buffRead, EFS_MAXIMUM_MEMORY_ALLOCATED);
                                    }
                                    if (contentRead < 0) {
                                        Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", fileName);
                                        break;
                                    }
                                    send(clientfd, buffRead, contentRead, 0);
                                    buffRead[contentRead] = NULL;
                                    Display_printf(g_SMCDisplay, 0, 0, "%s", buffRead);
                                    fileSize -= contentRead;

                                }
                                Display_printf(g_SMCDisplay, 0, 0, "<--------------------------------->\n");

                            }
                            Memory_free(NULL, buffRead, EFS_MAXIMUM_MEMORY_ALLOCATED);
                        }else {

                        }


                        SPIFFS_close(&object->fs, fd);

                        //                send(clientfd, pBuffer, bufferSize, 0);

                    }


                    bStorageFFS_unmount(handle, timeout);
                }
            }
            GateMutexPri_leave(object->hSFFS_gateMount, key);

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

    return retVal;
}







