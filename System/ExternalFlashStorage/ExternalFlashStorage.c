/*
 * ExternalFlashStorage.c
 *
 *  Created on: Nov 14, 2018
 *      Author: epenate
 */

#define __SYSTEM_EXTERNALFLASHSTORAGE_GLOBAL
#include "includes.h"



/* SPIFFS configuration parameters */
#define SPIFFS_LOGICAL_BLOCK_SIZE    (16384)
#define SPIFFS_LOGICAL_PAGE_SIZE     (128)
#define SPIFFS_FILE_DESCRIPTOR_SIZE  (44)

/*
 * SPIFFS needs RAM to perform operations on files.  It requires a work buffer
 * which MUST be (2 * LOGICAL_PAGE_SIZE) bytes.
 */
static uint8_t spiffsWorkBuffer[SPIFFS_LOGICAL_PAGE_SIZE * 2];

/* The array below will be used by SPIFFS as a file descriptor cache. */
static uint8_t spiffsFileDescriptorCache[SPIFFS_FILE_DESCRIPTOR_SIZE * 4];

/* The array below will be used by SPIFFS as a read/write cache. */
static uint8_t spiffsReadWriteCache[SPIFFS_LOGICAL_PAGE_SIZE * 2];

#define MESSAGE_LENGTH    (22)
const char message[MESSAGE_LENGTH] = "Hello from SPIFFS2!!\n";
char readBuffer[MESSAGE_LENGTH];

spiffs fs;
SPIFFSNVS_Data spiffsnvsData;

/*
 *  ======== mainThread ========
 */
void *EFS_mainThread(void *arg0)
{
    spiffs_file    fd;
    spiffs_config  fsConfig;
    int32_t        status;

    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");
    GPIO_write(SMC_FLASH_WP, 1);
    GPIO_write(SMC_FLASH_RESET, 1);

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&spiffsnvsData, Board_NVSINTERNAL, &fs, &fsConfig,
        SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(g_SMCDisplay, 0, 0,
            "Error with SPIFFS configuration.\n");

        while (1);
    }

    Display_printf(g_SMCDisplay, 0, 0, "Mounting file system...");

    status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
        spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
        spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
    if (status != SPIFFS_OK) {
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(g_SMCDisplay, 0, 0,
                "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&fs);
            status = SPIFFS_format(&fs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error formatting memory.\n");

                while (1);
            }

            status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
                spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
                spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
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
    fd = SPIFFS_open(&fs, "spiffsFile", SPIFFS_RDWR, 0);
    if (fd < 0) {
        /* File not found; create a new file & write message to it */
        Display_printf(g_SMCDisplay, 0, 0, "Creating spiffsFile...");

        fd = SPIFFS_open(&fs, "spiffsFile", SPIFFS_CREAT | SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                "Error creating spiffsFile.\n");

            while (1);
        }

        Display_printf(g_SMCDisplay, 0, 0, "Writing to spiffsFile...");

        if (SPIFFS_write(&fs, fd, (void *) &message, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error writing spiffsFile.\n");

            while (1) ;
        }

        SPIFFS_close(&fs, fd);
    }
    else {
        Display_printf(g_SMCDisplay, 0, 0, "Reading spiffsFile...\n");

        /* spiffsFile exists; read its contents & delete the file */
        if (SPIFFS_read(&fs, fd, readBuffer, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error reading spiffsFile.\n");

            while (1) ;
        }

        Display_printf(g_SMCDisplay, 0, 0, "spiffsFile: %s", readBuffer);
        Display_printf(g_SMCDisplay, 0, 0, "Erasing spiffsFile...");

        status = SPIFFS_fremove(&fs, fd);
        if (status != SPIFFS_OK) {
            Display_printf(g_SMCDisplay, 0, 0, "Error removing spiffsFile.\n");

            while (1);
        }

        SPIFFS_close(&fs, fd);
    }

    SPIFFS_unmount(&fs);

    Display_printf(g_SMCDisplay, 0, 0, "Reset the device.");
    Display_printf(g_SMCDisplay, 0, 0,
        "==================================================\n\n");

    return (NULL);
}


void vEFS_loadStartUpConfiguration(void *arg0)
{
    spiffs_file    fd;
    spiffs_config  fsConfig;
    int32_t        status;

    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;

    u32_t total, used;
    int res;

    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    Display_printf(g_SMCDisplay, 0, 0, "Loading Initial Configuration");

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&spiffsnvsData, Board_NVSEXTERNAL, &fs, &fsConfig,
        SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(g_SMCDisplay, 0, 0,
            "Error with SPIFFS configuration.\n");

        while (1);
    }

    Display_printf(g_SMCDisplay, 0, 0, "Mounting Internal Flash file system...");

    status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
        spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
        spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
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

            SPIFFS_unmount(&fs);
            status = SPIFFS_format(&fs);
            if (status != SPIFFS_OK) {
                Display_printf(g_SMCDisplay, 0, 0,
                    "Error formatting memory.\n");

                while (1);
            }

            status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
                spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
                spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
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
    fd = SPIFFS_open(&fs, IFS_STARTUP_CONF_FILE_NAME, SPIFFS_RDWR, 0);
    if (fd < 0) {
        /* File not found; create a new file & write message to it */
        Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_STARTUP_CONF_FILE_NAME);

        fd = SPIFFS_open(&fs, IFS_STARTUP_CONF_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                "Error creating %s.\n", IFS_STARTUP_CONF_FILE_NAME);

            while (1);
        }

        Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_STARTUP_CONF_FILE_NAME);

        if (SPIFFS_write(&fs, fd, (void *) &message, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_CONF_FILE_NAME);

            while (1) ;
        }

//        Display_printf(g_SMCDisplay, 0, 0, "Flushing %s...\n", EFS_STARTUP_CONF_FILE_NAME);
//        SPIFFS_fflush(&fs, fd);
//
//        Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", EFS_STARTUP_CONF_FILE_NAME);
//
//        /* spiffsFile exists; read its contents & delete the file */
//        if (SPIFFS_read(&fs, fd, readBuffer, MESSAGE_LENGTH) < 0) {
//            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", EFS_STARTUP_CONF_FILE_NAME);
//
//            while (1) ;
//        }
//
//        Display_printf(g_SMCDisplay, 0, 0, "%s: %s", EFS_STARTUP_CONF_FILE_NAME, readBuffer);

        SPIFFS_close(&fs, fd);
    }
    else {


        Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_STARTUP_CONF_FILE_NAME);

        /* spiffsFile exists; read its contents & delete the file */
        if (SPIFFS_read(&fs, fd, readBuffer, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);

            while (1) ;
        }

        Display_printf(g_SMCDisplay, 0, 0, "%s: %s", IFS_STARTUP_CONF_FILE_NAME, readBuffer);


//        Display_printf(g_SMCDisplay, 0, 0, "Erasing %s...", EFS_STARTUP_CONF_FILE_NAME);
//
//        status = SPIFFS_fremove(&fs, fd);
//        if (status != SPIFFS_OK) {
//            Display_printf(g_SMCDisplay, 0, 0, "Error removing spiffsFile.\n");
//
//            while (1);
//        }

        Display_printf(g_SMCDisplay, 0, 0, "closing %s...", IFS_STARTUP_CONF_FILE_NAME);
        SPIFFS_close(&fs, fd);
    }

    Display_printf(g_SMCDisplay, 0, 0, "\nReading files in directory /");


    SPIFFS_opendir(&fs, "/", &d);
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


        fd = SPIFFS_open_by_dirent(&fs, pe, SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error SPIFFS_open_by_dirent %i.\n", SPIFFS_errno(&fs));
            while (1) ;
        }

        if (SPIFFS_read(&fs, fd, readBuffer, MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
            while (1) ;
        }
        Display_printf(g_SMCDisplay, 0, 0, "-> %s", readBuffer);

        res = SPIFFS_close(&fs, fd);
        if (res < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "errno %i\n", SPIFFS_errno(&fs));
            while (1) ;
        }
    }
    SPIFFS_closedir(&d);


    /***********************************************************************/
//    Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", EFS_STARTUP_CONF_FILE_NAME);
//
////    fd = SPIFFS_open(&fs, EFS_STARTUP_CONF_FILE_NAME, SPIFFS_APPEND | SPIFFS_RDWR, 0);
////    fd = SPIFFS_open(&fs, EFS_STARTUP_CONF_FILE_NAME, SPIFFS_TRUNC | SPIFFS_RDWR, 0);
//    fd = SPIFFS_open(&fs, EFS_STARTUP_CONF_FILE_NAME, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
//    if (fd < 0) {
//        Display_printf(g_SMCDisplay, 0, 0,
//                       "Error openning %s.\n", EFS_STARTUP_CONF_FILE_NAME);
//
//        while (1);
//    }
//
//    Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", EFS_STARTUP_CONF_FILE_NAME);
//
//    if (SPIFFS_write(&fs, fd, (void *) &message, MESSAGE_LENGTH) < 0) {
//        Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", EFS_STARTUP_CONF_FILE_NAME);
//
//        while (1) ;
//    }
//    SPIFFS_close(&fs, fd);



    fd = SPIFFS_open(&fs, IFS_STARTUP_CONF_FILE_NAME, SPIFFS_RDWR, 0);
    if (fd < 0) {
        Display_printf(g_SMCDisplay, 0, 0,
                       "Error creating %s.\n", IFS_STARTUP_CONF_FILE_NAME);

        while (1);
    }

    Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_STARTUP_CONF_FILE_NAME);

    /* spiffsFile exists; read its contents & delete the file */
    while (SPIFFS_read(&fs, fd, readBuffer, MESSAGE_LENGTH) > 0) {
        Display_printf(g_SMCDisplay, 0, 0, "--> %s", readBuffer);
    }


    Display_printf(g_SMCDisplay, 0, 0, "closing %s...", IFS_STARTUP_CONF_FILE_NAME);
    SPIFFS_close(&fs, fd);
    /***********************************************************************/
    Display_printf(g_SMCDisplay, 0, 0, "SPIFFS_stat...");
    spiffs_stat s;
    res = SPIFFS_stat(&fs, IFS_STARTUP_CONF_FILE_NAME, &s);
    Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i\n", s.name, s.obj_id, s.size);
    /***********************************************************************/

    res = SPIFFS_info(&fs, &total, &used);
    Display_printf(g_SMCDisplay, 0, 0, "File System total:%i used:%i\n", total, used);

    Display_printf(g_SMCDisplay, 0, 0, "Unmounting the fs.");
    SPIFFS_unmount(&fs);

    Display_printf(g_SMCDisplay, 0, 0,
        "==================================================\n\n");

}



