/*
 * ExternalFlashStorage.c
 *
 *  Created on: Nov 14, 2018
 *      Author: epenate
 */

#define __SYSTEM_EXTERNALFLASHSTORAGE_GLOBAL
#include "includes.h"

#undef Log_error0
#undef Log_error1
#undef Log_print0
#undef Log_print1
#undef Log_print2
#undef Log_print3
#undef Log_print4

#include <ti/net/http/httpserver.h>
#include <ti/net/http/http.h>
#include <ti/net/http/logging.h>

#include "HTTPServer/urlsimple.h"
#include "HTTPServer/URLHandler/URLHandler.h"


/* SPIFFS configuration parameters */
#define SPIFFS_LOGICAL_BLOCK_SIZE    (16384)
#define SPIFFS_LOGICAL_PAGE_SIZE     (128)
#define SPIFFS_FILE_DESCRIPTOR_SIZE  (44)

/*
 * SPIFFS needs RAM to perform operations on files.  It requires a work buffer
 * which MUST be (2 * LOGICAL_PAGE_SIZE) bytes.
 */
static uint8_t g_EFSspiffsWorkBuffer[SPIFFS_LOGICAL_PAGE_SIZE * 2];

/* The array below will be used by SPIFFS as a file descriptor cache. */
static uint8_t g_EFSspiffsFileDescriptorCache[SPIFFS_FILE_DESCRIPTOR_SIZE * 4];

/* The array below will be used by SPIFFS as a read/write cache. */
static uint8_t g_EFSspiffsReadWriteCache[SPIFFS_LOGICAL_PAGE_SIZE * 2];

#define MESSAGE_LENGTH    (22)
const char g_EFSmessage[MESSAGE_LENGTH] = "Hello from SPIFFS2!!\n";
char g_EFSreadBuffer[MESSAGE_LENGTH];

spiffs g_EFSfs;
SPIFFSNVS_Data g_EFSspiffsnvsData;
spiffs_config  g_EFSfsConfig;

/*
 *  ======== mainThread ========
 */
void *EFS_mainThread(void *arg0)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;

    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");
    GPIO_write(SMC_FLASH_WP, 1);
    GPIO_write(SMC_FLASH_RESET, 1);

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&g_EFSspiffsnvsData, Board_NVSINTERNAL, &g_EFSfs, &g_EFSfsConfig,
        SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(g_SMCDisplay, 0, 0,
            "Error with SPIFFS configuration.\n");

        while (1);
    }

    Display_printf(g_SMCDisplay, 0, 0, "Mounting file system...");

    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
        g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
        g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
    if (status != SPIFFS_OK) {
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(g_SMCDisplay, 0, 0,
                "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&g_EFSfs);
            status = SPIFFS_format(&g_EFSfs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error formatting memory.\n");

                while (1);
            }

            status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error mounting file system.\n");

                while (1);
            }
        }
        else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(g_SMCDisplay, 0, 0,
                "Error mounting file system: %d.\n", status);

            while (1);
        }
    }

    /* Open a file */
    fd = SPIFFS_open(&g_EFSfs, "spiffsFile", SPIFFS_RDWR, 0);
    if (fd < 0) {
        /* File not found; create a new file & write message to it */
        Display_printf(g_SMCDisplay, 0, 0, "Creating spiffsFile...");

        fd = SPIFFS_open(&g_EFSfs, "spiffsFile", SPIFFS_CREAT | SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                "Error creating spiffsFile.\n");

            while (1);
        }

        Display_printf(g_SMCDisplay, 0, 0, "Writing to spiffsFile...");

        if (SPIFFS_write(&g_EFSfs, fd, (void *) &g_EFSmessage, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error writing spiffsFile.\n");

            while (1) ;
        }

        SPIFFS_close(&g_EFSfs, fd);
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "Reading spiffsFile...\n");

        /* spiffsFile exists; read its contents & delete the file */
        if (SPIFFS_read(&g_EFSfs, fd, g_EFSreadBuffer, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error reading spiffsFile.\n");

            while (1) ;
        }

        Display_printf(g_SMCDisplay, 0, 0, "spiffsFile: %s", g_EFSreadBuffer);
        Display_printf(g_SMCDisplay, 0, 0, "Erasing spiffsFile...");

        status = SPIFFS_fremove(&g_EFSfs, fd);
        if (status != SPIFFS_OK) {
            Display_printf(g_SMCDisplay, 0, 0, "Error removing spiffsFile.\n");

            while (1);
        }

        SPIFFS_close(&g_EFSfs, fd);
    }

    SPIFFS_unmount(&g_EFSfs);

    Display_printf(g_SMCDisplay, 0, 0, "Reset the device.");
    Display_printf(g_SMCDisplay, 0, 0,
        "==================================================\n\n");

    return (NULL);
}


void vEFS_init()
{
    int32_t        status;

    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    Display_printf(g_SMCDisplay, 0, 0, "Loading Initial Configuration");

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&g_EFSspiffsnvsData, Board_NVSEXTERNAL, &g_EFSfs, &g_EFSfsConfig,
        SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(g_SMCDisplay, 0, 0,
            "Error with SPIFFS configuration.\n");

//        while (1);
    }
}



int xEFS_createDefaultTestFiles()
{
    spiffs_file    fd;
    int retVal = 0;

    /**************************************************************************************************/
    /* File not found; create a new file & write g_IFSmessage to it */
    Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_MEM_TEST_FILE_NAME);

    fd = SPIFFS_open(&g_EFSfs, IFS_MEM_TEST_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
    if (fd < 0) {
        Display_printf(g_SMCDisplay, 0, 0,
                       "Error creating %s.\n", IFS_MEM_TEST_FILE_NAME);
        retVal = 1;
    }else {
        Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_MEM_TEST_FILE_NAME);
        //            Task_sleep((unsigned int)150);
        if (SPIFFS_write(&g_EFSfs, fd, (void *) &g_EFSmessage, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_MEM_TEST_FILE_NAME);
            retVal = 2;
        }else {
            Display_printf(g_SMCDisplay, 0, 0, "Flushing %s...\n", IFS_MEM_TEST_FILE_NAME);
            SPIFFS_fflush(&g_EFSfs, fd);
            Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_MEM_TEST_FILE_NAME);
            /* spiffsFile exists; read its contents & delete the file */
            if (SPIFFS_read(&g_EFSfs, fd, g_EFSreadBuffer, MESSAGE_LENGTH) < 0) {
                Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_MEM_TEST_FILE_NAME);
                retVal = 3;
            }
        }
        Display_printf(g_SMCDisplay, 0, 0, "%s: %s", IFS_MEM_TEST_FILE_NAME, g_EFSreadBuffer);
        SPIFFS_close(&g_EFSfs, fd);
    }

    return retVal;
}

void vEFS_loadStartUpConfigurationTest(void *arg0)
{
    spiffs_file    fd;
    int32_t        status;
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;
    u32_t total, used;
    int res;
#define EFS_READ_BUFFER_SIZE    128
    char buffRead[EFS_READ_BUFFER_SIZE];
    uint32_t fileSize;



    Display_printf(g_SMCDisplay, 0, 0, "Mounting External Flash file system...");

    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
        g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
        g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
//    status = SPIFFS_ERR_NOT_A_FS;
    if (status != SPIFFS_OK) {
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(g_SMCDisplay, 0, 0,
                "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&g_EFSfs);
            status = SPIFFS_format(&g_EFSfs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error formatting memory.\n");
//                while (1);
            }

            status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error mounting file system.\n");
//                while (1);
            }

            xEFS_createDefaultTestFiles();
        }
        else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(g_SMCDisplay, 0, 0,
                "Error mounting file system: %d.\n", status);
//            while (1);
        }
    }

    if (status == SPIFFS_OK) {
        /* Open a file */
//        fd = SPIFFS_open(&g_EFSfs, IFS_STARTUP_CONF_FILE_NAME, SPIFFS_RDWR, 0);
//        if (fd < 0) {
//            /* File not found; create a new file & write message to it */
//            Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_STARTUP_CONF_FILE_NAME);
//
//            fd = SPIFFS_open(&g_EFSfs, IFS_STARTUP_CONF_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
//            if (fd < 0) {
//                Display_printf(g_SMCDisplay, 0, 0,
//                               "Error creating %s.\n", IFS_STARTUP_CONF_FILE_NAME);
////                while (1);
//            }else{
//                Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_STARTUP_CONF_FILE_NAME);
//                Task_sleep((unsigned int)150);
//                if (SPIFFS_write(&g_EFSfs, fd, (void *) &g_EFSmessage, MESSAGE_LENGTH) < 0) {
//                    Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_CONF_FILE_NAME);
//                    //                while (1) ;
//                }
//                SPIFFS_close(&g_EFSfs, fd);
//            }
//        }
//        else {
//            Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_STARTUP_CONF_FILE_NAME);
//            /* spiffsFile exists; read its contents & delete the file */
//            if (SPIFFS_read(&g_EFSfs, fd, g_EFSreadBuffer, MESSAGE_LENGTH) < 0) {
//                Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
////                while (1) ;
//            }else{
//                Display_printf(g_SMCDisplay, 0, 0, "%s: %s", IFS_STARTUP_CONF_FILE_NAME, g_EFSreadBuffer);
//            }
//            Display_printf(g_SMCDisplay, 0, 0, "closing %s...", IFS_STARTUP_CONF_FILE_NAME);
//            SPIFFS_close(&g_EFSfs, fd);
//        }


        Display_printf(g_SMCDisplay, 0, 0, "\nReading files in directory /");
        SPIFFS_opendir(&g_EFSfs, "/", &d);
        while ((pe = SPIFFS_readdir(&d, pe))) {
            Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i", pe->name, pe->obj_id, pe->size);
            //        if (0 == strncmp(EFS_STARTUP_CONF_FILE_NAME, (char *)pe->name, strlen(EFS_STARTUP_CONF_FILE_NAME))) {
            if (0 == strncmp("start", (char *)pe->name, 5)) {
                Display_printf(g_SMCDisplay, 0, 0, "Found %s.\n", IFS_STARTUP_CONF_FILE_NAME);
            }
            /*********************************************************************/
            /* Do note that if the any file is modified (except for fully removing)
             * within the dirent iterator, the iterator may become invalid.
             */
            /*********************************************************************/
            fd = SPIFFS_open_by_dirent(&g_EFSfs, pe, SPIFFS_RDWR, 0);
            if (fd < 0) {
                Display_printf(g_SMCDisplay, 0, 0, "Error SPIFFS_open_by_dirent %i.\n", SPIFFS_errno(&g_EFSfs));
//                while (1) ;
            }else {
                for (res = 0; res < EFS_READ_BUFFER_SIZE; res++) {
                    buffRead[res] = 0;
                }
                fileSize = pe->size;
                int contentRead;
                Display_printf(g_SMCDisplay, 0, 0, "<--------------------------------->\n->");
                while (fileSize > 0) {
                    if (fileSize < EFS_READ_BUFFER_SIZE) {
                        contentRead = SPIFFS_read(&g_EFSfs, fd, buffRead, fileSize);
                        if (contentRead < 0) {
                            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
                            break;
                        }
                        Display_printf(g_SMCDisplay, 0, 0, "%s", buffRead);
                        fileSize -= contentRead;
                    }else {
                        contentRead = SPIFFS_read(&g_EFSfs, fd, buffRead, EFS_READ_BUFFER_SIZE);
                        if (contentRead < 0) {
                            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
                            break;
                        }
                        Display_printf(g_SMCDisplay, 0, 0, "%s", buffRead);
                        fileSize -= contentRead;
                        for (res = 0; res < EFS_READ_BUFFER_SIZE; res++) {
                            buffRead[res] = 0;
                        }
                    }

                }
                Display_printf(g_SMCDisplay, 0, 0, "<--------------------------------->\n");

                res = SPIFFS_close(&g_EFSfs, fd);
                if (res < 0) {
                    Display_printf(g_SMCDisplay, 0, 0, "errno %i\n", SPIFFS_errno(&g_EFSfs));
//                    while (1) ;
                }
            }
        }
        SPIFFS_closedir(&d);



        fd = SPIFFS_open(&g_EFSfs, IFS_STARTUP_CONF_FILE_NAME, SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                           "Error creating %s.\n", IFS_STARTUP_CONF_FILE_NAME);

//            while (1);
        }else {
            Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_STARTUP_CONF_FILE_NAME);
            /* spiffsFile exists; read its contents & delete the file */
            while (SPIFFS_read(&g_EFSfs, fd, g_EFSreadBuffer, MESSAGE_LENGTH) > 0) {
                Display_printf(g_SMCDisplay, 0, 0, "--> %s", g_EFSreadBuffer);
            }
            Display_printf(g_SMCDisplay, 0, 0, "closing %s...", IFS_STARTUP_CONF_FILE_NAME);
            SPIFFS_close(&g_EFSfs, fd);
        }
        /***********************************************************************/
        Display_printf(g_SMCDisplay, 0, 0, "SPIFFS_stat...");
        spiffs_stat s;
        res = SPIFFS_stat(&g_EFSfs, IFS_STARTUP_CONF_FILE_NAME, &s);
        Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i\n", s.name, s.obj_id, s.size);
        /***********************************************************************/

        res = SPIFFS_info(&g_EFSfs, &total, &used);
        Display_printf(g_SMCDisplay, 0, 0, "File System total:%i used:%i\n", total, used);

    }
    Display_printf(g_SMCDisplay, 0, 0, "Unmounting the fs.");
    SPIFFS_unmount(&g_EFSfs);

    Display_printf(g_SMCDisplay, 0, 0,
                   "==================================================\n\n");

}


void vEFS_loadStartUpConfiguration(void *arg0)
{
//    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;

//    spiffs_DIR d;
//    struct spiffs_dirent e;
//    struct spiffs_dirent *pe = &e;

    u32_t total, used;
    int res;

    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    Display_printf(g_SMCDisplay, 0, 0, "Loading Initial Configuration");

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&g_EFSspiffsnvsData, Board_NVSEXTERNAL, &g_EFSfs, &g_EFSfsConfig,
        SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(g_SMCDisplay, 0, 0,
            "Error with SPIFFS configuration.\n");

        while (1);
    }

    Display_printf(g_SMCDisplay, 0, 0, "Mounting External Flash file system...");

    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
        g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
        g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
//    status = SPIFFS_ERR_NOT_A_FS;
    if (status != SPIFFS_OK) {
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(g_SMCDisplay, 0, 0,
                "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&g_EFSfs);
            status = SPIFFS_format(&g_EFSfs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error formatting memory.\n");

                while (1);
            }

            status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error mounting file system.\n");

                while (1);
            }
        }
        else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(g_SMCDisplay, 0, 0,
                "Error mounting file system: %d.\n", status);

            while (1);
        }
    }

    /***********************************************************************/
    Display_printf(g_SMCDisplay, 0, 0, "SPIFFS_stat...");
    spiffs_stat s;
    res = SPIFFS_stat(&g_EFSfs, IFS_STARTUP_CONF_FILE_NAME, &s);
    Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i\n", s.name, s.obj_id, s.size);
    /***********************************************************************/

    res = SPIFFS_info(&g_EFSfs, &total, &used);
    Display_printf(g_SMCDisplay, 0, 0, "File System total:%i used:%i\n", total, used);
    res = res;

    Display_printf(g_SMCDisplay, 0, 0, "Unmounting the fs.");
    SPIFFS_unmount(&g_EFSfs);

    Display_printf(g_SMCDisplay, 0, 0,
        "==================================================\n\n");

}

void vEFS_setFlashDataFileNameHTTP(int clientfd, int contentLength, char *fileName)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
//    uint32_t bufferSize;
//    char *fileName;
//    IFS_deviceInfoFile_t devInfo;
    Error_Block eb;

    Error_init(&eb);

//    TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFPayload = 0;
//    IFS_deviceInfoFile_t *pDeviceInfo = (IFS_deviceInfoFile_t *)pFPayload->payload;
//    fileName = pFPayload->fileName;


//    if (strlen(fileName) >= IFS_FILE_NAME_LENGTH ) fileName[IFS_FILE_NAME_LENGTH - 1] = 0;

    Display_printf(g_SMCDisplay, 0, 0, "Received %d bytes\n", contentLength);
    if (contentLength > 0) {
//        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, fileName, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
            status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                                  g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                                  g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
            if (status == SPIFFS_OK) {

                fd = SPIFFS_open(&g_EFSfs, fileName, SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_TRUNC, 0); //SPIFFS_APPEND | SPIFFS_TRUNC
                if (fd >= 0) {

                    /*
                     * Revisar Algoritmo. Mientras queden datos, hay q reservar memoria, leer socket y guardar en flash. Cerrar memoria.
                     */
                    char *buf;
                    ssize_t nbytes = 0;

                    while(contentLength > 0) {

                        if (contentLength < EFS_MAXIMUM_MEMORY_ALLOCATED) {
                            buf = Memory_alloc(NULL, contentLength, 0, &eb);
                            if (buf != NULL) {
                                nbytes = recv(clientfd, buf, contentLength, 0);
                                if (nbytes > 0) {
                                    ((uint8_t *)buf)[nbytes] = 0;
                                    Display_printf(g_SMCDisplay, 0, 0, "%s\n", (uint8_t *)buf);
                                    contentLength -= nbytes;
                                }
                                else {
                                    Memory_free(NULL, buf, contentLength);
                                    break;
                                }


                                if (SPIFFS_write(&g_EFSfs, fd, (void *) buf, nbytes) < 0) {
                                    Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", fileName);
                                    //                return;
                                }
                                Memory_free(NULL, buf, contentLength);

                            }
                        }else {
                            buf = Memory_alloc(NULL, EFS_MAXIMUM_MEMORY_ALLOCATED, 0, &eb);
                            if (buf != NULL) {
                                nbytes = recv(clientfd, buf, EFS_MAXIMUM_MEMORY_ALLOCATED, 0);
                                if (nbytes > 0) {
                                    ((uint8_t *)buf)[nbytes] = 0;
                                    Display_printf(g_SMCDisplay, 0, 0, "%s\n", (uint8_t *)buf);
                                    contentLength -= nbytes;
                                }
                                else {
                                    Memory_free(NULL, buf, EFS_MAXIMUM_MEMORY_ALLOCATED);
                                    break;
                                }


                                if (SPIFFS_write(&g_EFSfs, fd, (void *) buf, nbytes) < 0) {
                                    Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", fileName);
                                    //                return;
                                }
                                Memory_free(NULL, buf, EFS_MAXIMUM_MEMORY_ALLOCATED);

                            }
                        }
                    }

                    SPIFFS_close(&g_EFSfs, fd);

                    //                send(clientfd, pBuffer, bufferSize, 0);

                }


                SPIFFS_unmount(&g_EFSfs);
            }
//        }
    }

}


void vEFS_getFlashDataFileNameHTTP(int clientfd, char *fileName)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
//    uint32_t bufferSize;
//    char *fileName;
//    IFS_deviceInfoFile_t devInfo;
//    int contentLength
    spiffs_stat s;
    Error_Block eb;

    Error_init(&eb);

//    TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFPayload = 0;
//    IFS_deviceInfoFile_t *pDeviceInfo = (IFS_deviceInfoFile_t *)pFPayload->payload;
//    fileName = pFPayload->fileName;


//    if (strlen(fileName) >= IFS_FILE_NAME_LENGTH ) fileName[IFS_FILE_NAME_LENGTH - 1] = 0;

    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                          g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                          g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
    if (status == SPIFFS_OK) {

        fd = SPIFFS_open(&g_EFSfs, fileName, SPIFFS_RDONLY, 0);
        if (fd >= 0) {

            SPIFFS_stat(&g_EFSfs, fileName, &s);
            Display_printf(g_SMCDisplay, 0, 0, "Sending -> %s [%04x] size:%i\n", s.name, s.obj_id, s.size);


            int fileSize = s.size;

            if (fileSize > 0) {
                HTTPServer_sendSimpleResponse(clientfd, HTTP_SC_OK, "text/html", fileSize, NULL);
                char *buffRead = Memory_alloc(NULL, EFS_MAXIMUM_MEMORY_ALLOCATED, 0, &eb);
                if (buffRead != NULL) {
                    int contentRead;
                    Display_printf(g_SMCDisplay, 0, 0, "<--------------------------------->\n->");
                    while (fileSize > 0) {
                        if (fileSize < EFS_READ_BUFFER_SIZE) {
                            contentRead = SPIFFS_read(&g_EFSfs, fd, buffRead, fileSize);
                        }else {
                            contentRead = SPIFFS_read(&g_EFSfs, fd, buffRead, EFS_READ_BUFFER_SIZE);
                        }
                        if (contentRead < 0) {
                            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
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


            SPIFFS_close(&g_EFSfs, fd);

            //                send(clientfd, pBuffer, bufferSize, 0);

        }


        SPIFFS_unmount(&g_EFSfs);
    }

}


uint32_t vEFS_testMemoryEthernet(int clientfd)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
//    uint32_t bufferSize;
    uint32_t retVal;
//    int res;
//    char *fileName = "memoryTest";
//    spiffs_stat s;
    Error_Block eb;


//    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

    Error_init(&eb);

    retVal = 0;

    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                          g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                          g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
    if (status == SPIFFS_OK) {

        /* Open a file */

        /* File not found; create a new file & write g_IFSmessage to it */
        Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_MEM_TEST_FILE_NAME);

        fd = SPIFFS_open(&g_EFSfs, IFS_MEM_TEST_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                           "Error creating %s.\n", IFS_MEM_TEST_FILE_NAME);
            retVal = 1;
        }else {

            Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_MEM_TEST_FILE_NAME);
//            Task_sleep((unsigned int)150);
            if (SPIFFS_write(&g_EFSfs, fd, (void *) &g_EFSmessage, MESSAGE_LENGTH) < 0) {
                Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_MEM_TEST_FILE_NAME);
                retVal = 2;
            }else {
                Display_printf(g_SMCDisplay, 0, 0, "Flushing %s...\n", IFS_MEM_TEST_FILE_NAME);
                SPIFFS_fflush(&g_EFSfs, fd);
                Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_MEM_TEST_FILE_NAME);
                /* spiffsFile exists; read its contents & delete the file */
                if (SPIFFS_read(&g_EFSfs, fd, g_EFSreadBuffer, MESSAGE_LENGTH) < 0) {
                    Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_MEM_TEST_FILE_NAME);
                    retVal = 3;
                }
            }
            Display_printf(g_SMCDisplay, 0, 0, "%s: %s", IFS_MEM_TEST_FILE_NAME, g_EFSreadBuffer);
            SPIFFS_close(&g_EFSfs, fd);

        }
        SPIFFS_unmount(&g_EFSfs);
    }
    return retVal;
}


int32_t vEFS_getStatus()
{
    int32_t        status;
    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
                          g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
                          g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);

    SPIFFS_unmount(&g_EFSfs);
    return status;
}

int32_t vEFS_format()
{
    int32_t        status;
    Int prevPri;
    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

//    if (SPIFFS_mounted(&g_IFSfs))

    SPIFFS_unmount(&g_EFSfs);

    prevPri = Task_setPri( Task_self(), 11);
    status = SPIFFS_format(&g_EFSfs);
    prevPri = Task_setPri( Task_self(), prevPri);
    if (status != SPIFFS_OK) {
        Display_printf(g_SMCDisplay, 0, 0, "Error formatting memory.\n");
    }

    Display_printf(g_SMCDisplay, 0, 0, "Mounting External Flash file system...");

    status = SPIFFS_mount(&g_EFSfs, &g_EFSfsConfig, g_EFSspiffsWorkBuffer,
        g_EFSspiffsFileDescriptorCache, sizeof(g_EFSspiffsFileDescriptorCache),
        g_EFSspiffsReadWriteCache, sizeof(g_EFSspiffsReadWriteCache), NULL);
    if (status != SPIFFS_OK) {
        xEFS_createDefaultTestFiles();
    }

    Display_printf(g_SMCDisplay, 0, 0, "Unmounting the fs.");
    SPIFFS_unmount(&g_EFSfs);
    return status;
}

