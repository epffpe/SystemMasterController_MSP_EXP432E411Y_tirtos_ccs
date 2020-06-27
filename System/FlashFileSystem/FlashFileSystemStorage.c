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

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();


int xSFFS_init(SFFS_Handle handle)
{
    int32_t        status;
    SFFS_Object *object = (SFFS_Object *)handle->object;
    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

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




SFFS_Handle hSFFS_open(SFFS_Handle handle)
{
    int32_t        status;
    unsigned int key;
    Error_Block         eb;
    Semaphore_Params    semParams;

    SFFS_Object *object = (SFFS_Object *)handle->object;
    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;



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


bool bSFFS_mount(SFFS_Handle handle, unsigned int timeout)
{
    int32_t        status;
    u32_t total, used;
    int res;
    bool bretValue = false;
    SFFS_Object *object = (SFFS_Object *)handle->object;
    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;


    Display_printf(g_SMCDisplay, 0, 0, "Mounting Internal Flash file system...");

    status = SPIFFS_mount(&object->fs,
                          &object->fsConfig,
                          object->spiffsWorkBuffer,
                          object->spiffsFileDescriptorCache, sizeof(object->spiffsFileDescriptorCache),
                          object->spiffsReadWriteCache, sizeof(object->spiffsReadWriteCache),
                          NULL);
    if (status != SPIFFS_OK) {
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(g_SMCDisplay, 0, 0,
                           "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&g_IFSfs);
            status = SPIFFS_format(&g_IFSfs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                               "Error formatting memory.\n");
                //                while (1);
            }

            status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                                  g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                                  g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                               "Error mounting file system.\n");
                //                while (1);
            }

        }else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(g_SMCDisplay, 0, 0,
                           "Error mounting file system: %d.\n", status);
            //            while (1);
        }
    }
}

bool bSFFS_unmount(SFFS_Handle handle, unsigned int timeout)
{

}

int xSFFS_write(SFFS_Handle handle, char *pBuffer, unsigned int length, unsigned int timeout)
{
    unsigned int ui32retValue = 0;
    SFFS_Object *object = (SFFS_Object *)handle->object;
    SFFS_HWAttrs *hwAttrs = (SFFS_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);
    ASSERT(pBuffer != NULL);

    if (handle == NULL) {
        return (NULL);
    }
    if (pBuffer == NULL) {
        return (NULL);
    }

    if (object->state.initialized) {
        if (Semaphore_pend(object->hSFFS_semInitialized, timeout)) {
            if (bSFFS_mount(handle, timeout)) {


                bSFFS_unmount(handle, timeout);
            }

            Semaphore_post(object->hSFFS_semInitialized);
        }
    }else {
        bSFFS_waitForInit(handle, timeout);
    }

    return ui32retValue;
}




