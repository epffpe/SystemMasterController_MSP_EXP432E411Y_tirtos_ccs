/*
 * InternalFlashStorage.c
 *
 *  Created on: Nov 15, 2018
 *      Author: epenate
 */


#define __SYSTEM_INTERNALFLASHSTORAGE_GLOBAL
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


/* SPIFFS configuration parameters */
#define IFS_SPIFFS_LOGICAL_BLOCK_SIZE    (16384)
#define IFS_SPIFFS_LOGICAL_PAGE_SIZE     (128)
#define IFS_SPIFFS_FILE_DESCRIPTOR_SIZE  (44)

/*
 * SPIFFS needs RAM to perform operations on files.  It requires a work buffer
 * which MUST be (2 * LOGICAL_PAGE_SIZE) bytes.
 */
static uint8_t g_IFSspiffsWorkBuffer[IFS_SPIFFS_LOGICAL_PAGE_SIZE * 2];

/* The array below will be used by SPIFFS as a file descriptor cache. */
static uint8_t g_IFSspiffsFileDescriptorCache[IFS_SPIFFS_FILE_DESCRIPTOR_SIZE * 4];

/* The array below will be used by SPIFFS as a read/write cache. */
static uint8_t g_IFSspiffsReadWriteCache[IFS_SPIFFS_LOGICAL_PAGE_SIZE * 2];

spiffs_config  g_fsConfig;

#define IFS_MESSAGE_LENGTH    (22)
const char g_IFSmessage[IFS_MESSAGE_LENGTH] = "Hello from SPIFFS2!!\n";
char g_IFSreadBuffer[IFS_MESSAGE_LENGTH];

spiffs g_IFSfs;
SPIFFSNVS_Data g_IFSspiffsnvsData;

int xIFS_createDefaultFiles();
int xIFS_findDevicesAtStartUp();
int xIFS_createDefaultTestFiles();

void vIFS_init()
{
    int32_t        status;

    Display_printf(g_SMCDisplay, 0, 0,
            "==================================================");

    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    Display_printf(g_SMCDisplay, 0, 0, "Loading Initial Configuration");

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
    status = SPIFFSNVS_config(&g_IFSspiffsnvsData, Board_NVSINTERNAL, &g_IFSfs, &g_fsConfig,
        IFS_SPIFFS_LOGICAL_BLOCK_SIZE, IFS_SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(g_SMCDisplay, 0, 0,
            "Error with SPIFFS configuration.\n");
//        while (1);
    }
}


void vIFS_loadStartUpConfiguration(void *arg0)
{
    int32_t        status;
    u32_t total, used;
    int res;


    Display_printf(g_SMCDisplay, 0, 0, "Mounting Internal Flash file system...");

    status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                          g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                          g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
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

            //            xIFS_createDefaultFiles();
#if !(defined(TEST_FIXTURE) || defined(DUT))
            xIFS_createDefaultFiles();
#endif
            xIFS_createDefaultTestFiles();
        }else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(g_SMCDisplay, 0, 0,
                           "Error mounting file system: %d.\n", status);
            //            while (1);
        }
    }

    if (status == SPIFFS_OK) {

        xIFS_findDevicesAtStartUp();

        /***********************************************************************/
        Display_printf(g_SMCDisplay, 0, 0, "SPIFFS_stat...");
        spiffs_stat s;
        res = SPIFFS_stat(&g_IFSfs, IFS_STARTUP_CONF_FILE_NAME, &s);
        Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i\n", s.name, s.obj_id, s.size);
        /***********************************************************************/

        res = SPIFFS_info(&g_IFSfs, &total, &used);
        Display_printf(g_SMCDisplay, 0, 0, "File System total:%i used:%i\n", total, used);
        res = res;
    }

    Display_printf(g_SMCDisplay, 0, 0, "Unmounting the fs.");
    SPIFFS_unmount(&g_IFSfs);

    Display_printf(g_SMCDisplay, 0, 0,
                   "==================================================\n\n");

}

int xIFS_createDefaultFiles()
{
    spiffs_file    fd;
    IFS_deviceInfoFile_t devInfo;
    int retVal = 0;

    devInfo.params.arg0 = NULL;
    devInfo.params.arg1 = NULL;
    /**************************************************************************************************/
    Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_STARTUP_DEVICE_0_FILE_NAME);

    fd = SPIFFS_open(&g_IFSfs, IFS_STARTUP_DEVICE_0_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
    if (fd < 0) {
        Display_printf(g_SMCDisplay, 0, 0, "Error creating %s.\n", IFS_STARTUP_DEVICE_0_FILE_NAME);
        return -1;
    }

    Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_STARTUP_DEVICE_0_FILE_NAME);

    devInfo.params.deviceType = DEVICE_TYPE_ALTO_MULTINET;
    devInfo.params.deviceID = 32;
    devInfo.params.arg0 = (void *)IF_SERIAL_6;
    strcpy (devInfo.description,"ALTO Multinet");

    if (SPIFFS_write(&g_IFSfs, fd, (void *) &devInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
        Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_DEVICE_0_FILE_NAME);
        return -1;
    }

    SPIFFS_close(&g_IFSfs, fd);

    /**************************************************************************************************/
//    Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_STARTUP_DEVICE_1_FILE_NAME);

//    fd = SPIFFS_open(&g_IFSfs, IFS_STARTUP_DEVICE_1_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
//    if (fd < 0) {
//        Display_printf(g_SMCDisplay, 0, 0, "Error creating %s.\n", IFS_STARTUP_DEVICE_1_FILE_NAME);
//        return -1;
//    }
//
//    Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_STARTUP_DEVICE_1_FILE_NAME);
//
//    devInfo.params.deviceType = DEVICE_TYPE_ALTO_AMP;
//    devInfo.params.deviceID = 33;
//    devInfo.params.arg0 = (void *)IF_SERIAL_3;
//    strcpy (devInfo.description,"ALTO Amp 1");
//
//    if (SPIFFS_write(&g_IFSfs, fd, (void *) &devInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
//        Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_DEVICE_1_FILE_NAME);
//        return -1;
//    }
//
//    SPIFFS_close(&g_IFSfs, fd);

    /**************************************************************************************************/
    Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_STARTUP_DEVICE_2_FILE_NAME);

    fd = SPIFFS_open(&g_IFSfs, IFS_STARTUP_DEVICE_2_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
    if (fd < 0) {
        Display_printf(g_SMCDisplay, 0, 0, "Error creating %s.\n", IFS_STARTUP_DEVICE_2_FILE_NAME);
        return -1;
    }

    Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_STARTUP_DEVICE_2_FILE_NAME);

    devInfo.params.deviceType = DEVICE_TYPE_ALTO_AMP;
    devInfo.params.deviceID = 34;
    devInfo.params.arg0 = (void *)IF_SERIAL_0;
    strcpy (devInfo.description,"ALTO Amp 2");

    if (SPIFFS_write(&g_IFSfs, fd, (void *) &devInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
        Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_DEVICE_2_FILE_NAME);
        return -1;
    }

    SPIFFS_close(&g_IFSfs, fd);

    return retVal;
}



int xIFS_createDefaultTestFiles()
{
    spiffs_file    fd;
    int retVal = 0;

    /**************************************************************************************************/
    /* File not found; create a new file & write g_IFSmessage to it */
    Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_MEM_TEST_FILE_NAME);

    fd = SPIFFS_open(&g_IFSfs, IFS_MEM_TEST_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
    if (fd < 0) {
        Display_printf(g_SMCDisplay, 0, 0,
                       "Error creating %s.\n", IFS_MEM_TEST_FILE_NAME);
        retVal = 1;
    }else {
        Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_MEM_TEST_FILE_NAME);
        //            Task_sleep((unsigned int)150);
        if (SPIFFS_write(&g_IFSfs, fd, (void *) &g_IFSmessage, IFS_MESSAGE_LENGTH) < 0) {
            Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_MEM_TEST_FILE_NAME);
            retVal = 2;
        }
        SPIFFS_fflush(&g_IFSfs, fd);
        Display_printf(g_SMCDisplay, 0, 0, "%s: %s", IFS_MEM_TEST_FILE_NAME, g_IFSreadBuffer);
        SPIFFS_close(&g_IFSfs, fd);
    }

    return retVal;
}

int xIFS_findDevicesAtStartUp()
{
    spiffs_file    fd;
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;
    Error_Block eb;
    Device_Params deviceParams;
    int res;
    IFS_deviceInfoFile_t devInfo;
    int retVal = 0;
    Error_init(&eb);

    Display_printf(g_SMCDisplay, 0, 0, "\nReading files in directory /");

    SPIFFS_opendir(&g_IFSfs, "/", &d);
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
                fd = SPIFFS_open_by_dirent(&g_IFSfs, pe, SPIFFS_RDWR, 0);
                if (fd < 0) {
                    Display_printf(g_SMCDisplay, 0, 0, "Error SPIFFS_open_by_dirent %i.\n", SPIFFS_errno(&g_IFSfs));
                    //                    while (1);
                }else{
                    if (SPIFFS_read(&g_IFSfs, fd, (void *)&devInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
                        Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_STARTUP_CONF_FILE_NAME);
                        //                        while (1);
                    }
                    res = SPIFFS_close(&g_IFSfs, fd);
                    if (res < 0) {
                        Display_printf(g_SMCDisplay, 0, 0, "errno %i\n", SPIFFS_errno(&g_IFSfs));
                        //                        while (1);
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

//    vALTOMultinetDevice_Params_init(&deviceParams, 32);
//    xDevice_add(&deviceParams, &eb);
//
//    vALTOAmpDevice_Params_init(&deviceParams, 33);
//    deviceParams.arg0 = (void *)IF_SERIAL_3;
//    xDevice_add(&deviceParams, &eb);
//
//    vALTOAmpDevice_Params_init(&deviceParams, 34);
//    deviceParams.arg0 = (void *)IF_SERIAL_4;
//    xDevice_add(&deviceParams, &eb);


//    CANTTest_init();

//    vForteManagerDevice_Params_init(&deviceParams, 35, IF_SERIAL_0);
//    xDevice_add(&deviceParams, &eb);

    return retVal;
}


void vIFS_getFlashDeviceListEthernet(int clientfd)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
    uint32_t bufferSize;
    int res;
    uint32_t counter = 0;
    spiffs_DIR d, d2;
    struct spiffs_dirent e, e2;
    struct spiffs_dirent *pe = &e, *pe2 = &e2;
    Error_Block eb;
    IFS_deviceInfoFile_t devInfo;


    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

    Error_init(&eb);

//    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures Board_NVSINTERNAL, Board_NVSEXTERNAL*/
//    status = SPIFFSNVS_config(&g_IFSspiffsnvsData, Board_NVSINTERNAL, &g_IFSfs, &fsConfig,
//        IFS_SPIFFS_LOGICAL_BLOCK_SIZE, IFS_SPIFFS_LOGICAL_PAGE_SIZE);
//    if (status == SPIFFSNVS_STATUS_SUCCESS) {
        status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                              g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                              g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
        if (status == SPIFFS_OK) {

            SPIFFS_opendir(&g_IFSfs, "/", &d);
            while ((pe = SPIFFS_readdir(&d, pe))) {
                if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                    if (pe->size >= sizeof(IFS_deviceInfoFile_t)) {
                        counter++;
                    }
                }
            }
            SPIFFS_closedir(&d);

//            SPIFFS_unmount(&g_IFSfs);
//        }
//
//        status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
//                              g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
//                              g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
//        if (status == SPIFFS_OK) {
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

                SPIFFS_opendir(&g_IFSfs, "/", &d2);
                while ((pe2 = SPIFFS_readdir(&d2, pe2))) {
                    //                Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i", pe->name, pe->obj_id, pe->size);

                    if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe2->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                        if (pe2->size >= sizeof(IFS_deviceInfoFile_t)) {
                            /*********************************************************************/
                            /* Do note that if the any file is modified (except for fully removing)
                             * within the dirent iterator, the iterator may become invalid.
                             */
                            /*********************************************************************/


                            fd = SPIFFS_open_by_dirent(&g_IFSfs, pe2, SPIFFS_RDWR, 0);
                            if (fd >= 0) {
                                if (SPIFFS_read(&g_IFSfs, fd, (void *)&devInfo, sizeof(IFS_deviceInfoFile_t)) >= 0) {
                                    res = SPIFFS_close(&g_IFSfs, fd);
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
            SPIFFS_unmount(&g_IFSfs);
        }

//    }

}


void vIFS_getFlashReadFileNameEthernet(int clientfd, char *fileName)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
    uint32_t bufferSize;
    int res;
    spiffs_stat s;
    Error_Block eb;


//    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

    Error_init(&eb);


    status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                          g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                          g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
    if (status == SPIFFS_OK) {

//        Display_printf(g_SMCDisplay, 0, 0, "SPIFFS_stat... %s", fileName);

        res = SPIFFS_stat(&g_IFSfs, fileName, &s);
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

                fd = SPIFFS_open(&g_IFSfs, fileName, SPIFFS_RDONLY, 0);
                if (fd >= 0) {
//                    Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", fileName);

                    /* spiffsFile exists; read its contents & delete the file */
                    if (SPIFFS_read(&g_IFSfs, fd, pFramePayload->payload, s.size) > 0) {
//                        Display_printf(g_SMCDisplay, 0, 0, "--> %s", pFramePayload->payload);
                    }

//                    Display_printf(g_SMCDisplay, 0, 0, "closing %s...", fileName);
                    SPIFFS_close(&g_IFSfs, fd);

                    send(clientfd, pBuffer, bufferSize, 0);
                }


                Memory_free(NULL, pBuffer, bufferSize);

            }
        }else{
            Display_printf(g_SMCDisplay, 0, 0, "**Error reading file: %s**\n", fileName);
        }
        SPIFFS_unmount(&g_IFSfs);
    }


}



void vIFS_setFlashDataFileNameEthernet(int clientfd, char *payload)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
//    uint32_t bufferSize;
    char *fileName;
//    IFS_deviceInfoFile_t devInfo;
    Error_Block eb;


    TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFPayload = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)payload;
    IFS_deviceInfoFile_t *pDeviceInfo = (IFS_deviceInfoFile_t *)pFPayload->payload;
    fileName = pFPayload->fileName;
    fileName[IFS_FILE_NAME_LENGTH - 1] = 0;

    Error_init(&eb);

    if (pFPayload->fileSize >= sizeof(IFS_deviceInfoFile_t)) {
        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, fileName, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
            status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                                  g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                                  g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
            if (status == SPIFFS_OK) {

                fd = SPIFFS_open(&g_IFSfs, fileName, SPIFFS_CREAT | SPIFFS_RDWR, 0);
                if (fd >= 0) {


                    if (SPIFFS_write(&g_IFSfs, fd, (void *) pDeviceInfo, sizeof(IFS_deviceInfoFile_t)) < 0) {
                        Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_STARTUP_DEVICE_1_FILE_NAME);

                        //                return;
                    }

                    SPIFFS_close(&g_IFSfs, fd);

                    //                send(clientfd, pBuffer, bufferSize, 0);

                }


                SPIFFS_unmount(&g_IFSfs);
            }
        }
    }


}



void vIFS_removeFileNameEthernet(int clientfd, char *payload)
{
//    spiffs_config  fsConfig;
    int32_t        status;
    int res;
//    uint32_t bufferSize;
    char *fileName;
//    IFS_deviceInfoFile_t devInfo;
    Error_Block eb;


    TCPBin_CMD_SystemControl_FlashFileData_payload_t *pFPayload = (TCPBin_CMD_SystemControl_FlashFileData_payload_t *)payload;
    fileName = pFPayload->fileName;
    fileName[IFS_FILE_NAME_LENGTH - 1] = 0;

    Error_init(&eb);

    if (pFPayload->fileSize == 0) {
        if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, fileName, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
            status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                                  g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                                  g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
            if (status == SPIFFS_OK) {

                res = SPIFFS_remove(&g_IFSfs, fileName);
                if (res < 0) {
                    Display_printf(g_SMCDisplay, 0, 0, "errno %i\n", SPIFFS_errno(&g_IFSfs));
                }else {
                    //                send(clientfd, pBuffer, bufferSize, 0);
                }

                SPIFFS_unmount(&g_IFSfs);
            }
        }
    }


}

void vIFS_getFlashConfigurationFileEthernet(int clientfd)
{
    spiffs_file    fd;
//    spiffs_config  fsConfig;
    int32_t        status;
    uint32_t bufferSize;
    int res;
    uint32_t counter = 0, index;
    spiffs_DIR d, d2;
    struct spiffs_dirent e, e2;
    struct spiffs_dirent *pe = &e, *pe2 = &e2;
    Error_Block eb;
    IFS_deviceInfoFile_t devInfo;


//    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

    Error_init(&eb);


    status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                          g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                          g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
    if (status == SPIFFS_OK) {

        SPIFFS_opendir(&g_IFSfs, "/", &d);
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

            SPIFFS_opendir(&g_IFSfs, "/", &d2);
            while ((pe2 = SPIFFS_readdir(&d2, pe2))) {
                //                Display_printf(g_SMCDisplay, 0, 0, "%s [%04x] size:%i", pe->name, pe->obj_id, pe->size);

                if (0 == strncmp(IFS_DEVICES_FOLDER_NAME, (char *)pe2->name, sizeof(IFS_DEVICES_FOLDER_NAME) - 1)) {
                    if (pe2->size >= sizeof(IFS_deviceInfoFile_t)) {
                        /*********************************************************************/
                        /* Do note that if the any file is modified (except for fully removing)
                         * within the dirent iterator, the iterator may become invalid.
                         */
                        /*********************************************************************/


                        fd = SPIFFS_open_by_dirent(&g_IFSfs, pe2, SPIFFS_RDWR, 0);
                        if (fd >= 0) {
                            if (SPIFFS_read(&g_IFSfs, fd, (void *)&devInfo, sizeof(IFS_deviceInfoFile_t)) >= 0) {
                                res = SPIFFS_close(&g_IFSfs, fd);
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
        SPIFFS_unmount(&g_IFSfs);
    }

}

uint32_t vIFS_testMemoryEthernet(int clientfd)
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

    status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                          g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                          g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
    if (status == SPIFFS_OK) {

        /* Open a file */

        /* File not found; create a new file & write g_IFSmessage to it */
        Display_printf(g_SMCDisplay, 0, 0, "Creating %s...", IFS_MEM_TEST_FILE_NAME);

        fd = SPIFFS_open(&g_IFSfs, IFS_MEM_TEST_FILE_NAME, SPIFFS_CREAT | SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(g_SMCDisplay, 0, 0,
                           "Error creating %s.\n", IFS_MEM_TEST_FILE_NAME);
            retVal = 1;
        }else {

//            Display_printf(g_SMCDisplay, 0, 0, "Writing to %s...", IFS_MEM_TEST_FILE_NAME);
////            Task_sleep((unsigned int)150);
//            if (SPIFFS_write(&g_IFSfs, fd, (void *) &g_IFSmessage, IFS_MESSAGE_LENGTH) < 0) {
//                Display_printf(g_SMCDisplay, 0, 0, "Error writing %s.\n", IFS_MEM_TEST_FILE_NAME);
//                retVal = 2;
//            }else {
//                Display_printf(g_SMCDisplay, 0, 0, "Flushing %s...\n", IFS_MEM_TEST_FILE_NAME);
//                SPIFFS_fflush(&g_IFSfs, fd);
                Display_printf(g_SMCDisplay, 0, 0, "Reading %s...\n", IFS_MEM_TEST_FILE_NAME);
                /* spiffsFile exists; read its contents & delete the file */
                if (SPIFFS_read(&g_IFSfs, fd, g_IFSreadBuffer, IFS_MESSAGE_LENGTH) < 0) {
                    Display_printf(g_SMCDisplay, 0, 0, "Error reading %s.\n", IFS_MEM_TEST_FILE_NAME);
                    retVal = 3;
//                }
            }
            Display_printf(g_SMCDisplay, 0, 0, "%s: %s", IFS_MEM_TEST_FILE_NAME, g_IFSreadBuffer);
            SPIFFS_close(&g_IFSfs, fd);

        }
        SPIFFS_unmount(&g_IFSfs);
    }
    return retVal;
}


int32_t vIFS_getStatus()
{
    int32_t        status;
    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);
    status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
                          g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
                          g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);

    SPIFFS_unmount(&g_IFSfs);
    /**
     * Runs a consistency check on given filesystem.
     * @param fs            the file system struct
     */
//    s32_t SPIFFS_check(spiffs *fs);
//    status = SPIFFS_check(&g_IFSfs);
    return status;
}

int32_t vIFS_format()
{
    int32_t        status;
    Int prevPri;
    Display_printf(g_SMCDisplay, 0, 0, "%s:", __func__);

//    if (SPIFFS_mounted(&g_IFSfs))

    SPIFFS_unmount(&g_IFSfs);

    prevPri = Task_setPri( Task_self(), MAX_SYSTEM_PRIORITY + 1);
    status = SPIFFS_format(&g_IFSfs);
    prevPri = Task_setPri( Task_self(), prevPri);
    if (status != SPIFFS_OK) {
        Display_printf(g_SMCDisplay, 0, 0, "Error formatting memory.\n");
    }

    Display_printf(g_SMCDisplay, 0, 0, "Mounting Internal Flash file system...");
    status = SPIFFS_mount(&g_IFSfs, &g_fsConfig, g_IFSspiffsWorkBuffer,
        g_IFSspiffsFileDescriptorCache, sizeof(g_IFSspiffsFileDescriptorCache),
        g_IFSspiffsReadWriteCache, sizeof(g_IFSspiffsReadWriteCache), NULL);
    if (status == SPIFFS_OK) {
        xIFS_createDefaultTestFiles();
    }
    Display_printf(g_SMCDisplay, 0, 0, "Unmounting the fs.");
    SPIFFS_unmount(&g_IFSfs);
    return status;
}



