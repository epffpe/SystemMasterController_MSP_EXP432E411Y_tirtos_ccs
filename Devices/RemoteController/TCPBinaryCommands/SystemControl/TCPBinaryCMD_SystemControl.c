/*
 * TCPBinaryCMD_SystemControl.c
 *
 *  Created on: Nov 27, 2018
 *      Author: epenate
 */

#define __DEVICES_REMOTECONTROLLER_TCPBINARYCOMMANDS_SYSTEMCONTROL_TCPBINARYCMD_SYSTEMCONTROL_GLOBAL
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


void vTCPRCBin_SystemControl_getOverAllStatus(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    Memory_Stats memStats;
    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_SystemControl_monitorStatus_payload_t)];

    TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)buffer;

    Memory_getStats(NULL, &memStats);

    pFrame->type = TCP_CMD_System_getMonitorStatusResponse | 0x80000000;
    pFrame->retDeviceID = TCPRCBINDEVICE_ID;
    pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
    pFrame->retParamID = 1;

    TCPBin_CMD_SystemControl_monitorStatus_payload_t *pFramePayload = (TCPBin_CMD_SystemControl_monitorStatus_payload_t *)pFrame->payload;


    pFramePayload->f28Voltage = 28.0;
    pFramePayload->f5MainVoltage = 5.0;
    pFramePayload->f5PerVoltage = 0.0;
    pFramePayload->fLogicCurrent = 0.0;
    pFramePayload->fPerCurrent = 0.0;
    pFramePayload->fMCUCurrent = 0.0;
    pFramePayload->fDAC = 0.0;
    pFramePayload->fADCCH[0] = 0.0;
    pFramePayload->fADCCH[1] = 0.0;
    pFramePayload->fADCCH[2] = 0.0;
    pFramePayload->fMCUTemp = 0.0;
    pFramePayload->fBoardTemp = 0.0;
    pFramePayload->ui32HeapTotalSize = memStats.totalSize;
    pFramePayload->ui32HeapTotalFreeSize = memStats.totalFreeSize;
    pFramePayload->ui32HeapLargestFreeSize = memStats.largestFreeSize;

    pFramePayload->bGpio0 = DIGet(DIO_GPI_0);
    pFramePayload->bGpio1 = DIGet(DIO_GPI_1);
    pFramePayload->bGpio2 = DIGet(DIO_GPI_2);
    pFramePayload->bGpio3 = DIGet(DIO_GPI_3);
    pFramePayload->bGpio4 = DIGet(DIO_GPI_4);
    pFramePayload->bGpio5 = DIGet(DIO_GPI_5);
    pFramePayload->bGpioReserved = 0;

    pFramePayload->bLed0 = 0;
    pFramePayload->bLed1 = 0;
    pFramePayload->bLed2 = 0;
    pFramePayload->bLed3 = DOGet(DIO_LED_D6) ? 0:1;
    pFramePayload->bLed4 = DOGet(DIO_LED_D20) ? 0:1;
    pFramePayload->bLedReserved = 0;

    pFramePayload->b5VOutEnabled = DOGet(DIO_5V_OUT_EN);
    pFramePayload->b5VOutStatus = DIGet(DIO_5V_OUT_STAT);
    pFramePayload->bUSBOutEnabled = DOGet(DIO_USB0EPEN);
    pFramePayload->bSWStatus = DIGet(DIO_INTERNAL_5V_SW_STATUS);
    pFramePayload->bDebugMode = DOGet(DIO_UART_DEBUG);
    pFramePayload->bSerial5Enabled = DOGet(DIO_SERIAL5_EN_);
    pFramePayload->bPowerPeripheralEnabled = DOGet(DIO_PWR_PERIPHERAL_EN) ? 0:1;


    bytesSent = send(clientfd, buffer, sizeof(buffer), 0);
    bytesSent = bytesSent;
}


// Could it be called Running devices on the command description

void vTCPRCBin_SystemControl_getRAMDeviceList(int clientfd, char *payload, int32_t size)
{
    int bytesSent;
    uint32_t counter = 0;
    uint32_t bufferSize;
    DevicesList_t *pDeviceList;
    Queue_Elem *elem;
    Error_Block eb;

//    char buffer[sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_5VPowerStatus_payload_t)];
//    TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *ptPayload = (TCP_CMD_ALTOMultinet_setSwitchOnOff_payload_t *)payload;

    Error_init(&eb);

    for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
        counter++;
    }

    bufferSize = sizeof(TCPBin_CMD_retFrame_t) + sizeof(TCPBin_CMD_SystemControl_RAM_deviceList_payload_t) + counter * sizeof(TCPBin_CMD_SystemControl_devicesList_t);
    char *pBuffer = Memory_alloc(NULL, bufferSize, 0, &eb);

    if (pBuffer !=NULL) {

        TCPBin_CMD_retFrame_t *pFrame = (TCPBin_CMD_retFrame_t *)pBuffer;
        pFrame->type = TCP_CMD_DiscreteIO_set5VOutputPowerResponse | 0x80000000;
        pFrame->retDeviceID = TCPRCBINDEVICE_ID;
        pFrame->retSvcUUID = SERVICE_TCPBIN_REMOTECONTROL_SYSTEMCONTROL_CLASS_RETURN_UUID;
        pFrame->retParamID = 4;

        TCPBin_CMD_SystemControl_RAM_deviceList_payload_t *pFramePayload = (TCPBin_CMD_SystemControl_RAM_deviceList_payload_t *)pFrame->payload;
        pFramePayload->numbOfDevices = counter;

        TCPBin_CMD_SystemControl_devicesList_t *pDeviceListElem = (TCPBin_CMD_SystemControl_devicesList_t *)pFramePayload->payload;

        for (elem = Queue_head(g_hDevicesListQ); elem != (Queue_Elem *)g_hDevicesListQ; elem = Queue_next(elem)) {
            pDeviceList = (DevicesList_t *)elem;
            pDeviceListElem->deviceID = pDeviceList->deviceID;
            pDeviceListElem->deviceType = pDeviceList->deviceType;
            pDeviceListElem->description[0] = NULL;
            pDeviceListElem++;
        }

        bytesSent = send(clientfd, pBuffer, bufferSize, 0);
        bytesSent = bytesSent;
        Memory_free(NULL, pBuffer, bufferSize);
    }
}

void vTCPRCBin_SystemControl_getFlashDeviceList(int clientfd, char *payload, int32_t size)
{

}


