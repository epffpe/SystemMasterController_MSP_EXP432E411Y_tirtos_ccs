/*
 * ComputerBridge.c
 *
 *  Created on: Jan 12, 2018
 *      Author: epenate
 */


#define __COMPUTERBRIDGE_GLOBAL
#include "includes.h"
#include <ctype.h>
#include <stdio.h>
#include <string>         // std::string
#include <string.h>         // std::string


#define FORTEMANAGER_AMP_ROUTING_MAP_SIZE       8
const uint32_t g_aForteManagerAmpRoutingMap[FORTEMANAGER_AMP_ROUTING_MAP_SIZE] =
{
     Board_Serial0,
     Board_Serial1,
     Board_Serial2,
     Board_Serial3,
     Board_Serial4,
     Board_Serial5,
     Board_Serial6,
     Board_Serial7,
};

Void vForteManagerHandler(UArg arg0, UArg arg1);


Void vForteManagerRXFxn(UArg arg0, UArg arg1)
{
    ALTOFrameMsgObj msgObj;
    ComputerBridge_Params *params;
    uint32_t ui32PCSerialPort;

    Mailbox_Handle mbox;
    Mailbox_Params mboxParams;
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;


    int nbytes;

//    char cmdbuf[ALTO_FRAME_CMD_BUFF_SIZE];
    char cReceived;

    std::size_t indexEOF = 0;
    std::size_t indexHeader = 0;
    std::string receivedPacket;
    std::string endFrame ("\r");
    std::string startFrame ("AA55");
//    cmdbuf[0] = 0;
    unsigned int index;

    ASSERT (arg0 != NULL);

    params = (ComputerBridge_Params *)arg0;
    ui32PCSerialPort = params->ui32PCPortIndex;
    GPIO_write(params->ui32PCSerialDEPin, 1);
    GPIO_write(params->ui32PCSerialREPin, 0);

    index = ui32PCSerialPort;
    ASSERT(index < DK_TM4C129X_UARTCOUNT);


    Error_init(&eb);

    Mailbox_Params_init(&mboxParams);
    mbox = Mailbox_create((SizeT)sizeof(ALTOFrameMsgObj),
                                  (UInt)COMPUTERBRIDGE_MAILBOX_NUM_BUFS,
                                  (Mailbox_Params *)&mboxParams,
                                  (Error_Block *)&eb);
    if (mbox == NULL) {
        System_abort("Mailbox create failed");
    }

    Task_Params_init(&taskParams);
    taskParams.stackSize = COMPUTERBRIDGE_TASK_STACK_SIZE;
    taskParams.priority = COMPUTERBRIDGE_TASK_PRIORITY;
    taskParams.arg0 = (UArg)arg0;
    taskParams.arg1 = (UArg)mbox;
    taskHandle = Task_create((Task_FuncPtr)vForteManagerHandler, &taskParams, &eb);
    if (taskHandle == NULL) {
        System_printf("Failed to create vTestForteManagerFxn Task\n");
        System_flush();
    }

    Semaphore_pend(hSerialSemRxGetHandler(index), BIOS_WAIT_FOREVER);
    while(1) {
        nbytes = UART_read(hSerialGetHandler(index), &cReceived, 1);
        if ((cReceived >= 'a') && (cReceived <= 'z')) {
            cReceived += 'A' - 'a';
        }
        std::string packet(&cReceived,nbytes);

        receivedPacket += packet;
        //                        if (receivedPacket.size() >= 69) {
        //                            memcpy(p64Buffer, (char *)receivedPacket.data(), receivedPacket.size());
        //                        }

        indexHeader = receivedPacket.find(startFrame);
        if ( indexHeader != std::string::npos) {
            if (indexHeader > 0) receivedPacket = receivedPacket.substr(indexHeader);

            indexEOF = receivedPacket.find(endFrame);

            while( indexEOF != std::string::npos){
                //                                indexHeader = receivedPacket.rfind(startFrame, indexEOF);
                indexHeader = receivedPacket.find(startFrame);
                if ( indexHeader != std::string::npos){
                    receivedPacket = receivedPacket.substr(indexHeader + startFrame.size());
                    indexEOF = receivedPacket.find(endFrame);
                    if ( indexEOF != std::string::npos){
                        std::string str = receivedPacket.substr(0, indexEOF);
                        if (str.size() <= 64) {
                            memcpy(msgObj.buff, (char *)str.data(), str.size());
                            msgObj.msgSize = str.size();
                            Mailbox_post(mbox, &msgObj, 1);
                        }
                    }
                }
                receivedPacket = receivedPacket.substr(indexEOF + endFrame.size());
                indexEOF = receivedPacket.find(endFrame);
            }
            if (receivedPacket.size() > 256) {
                indexHeader = receivedPacket.rfind(startFrame);
                if (indexHeader) {
                    receivedPacket = receivedPacket.substr(indexHeader);
                }else{
                    receivedPacket = receivedPacket.substr(receivedPacket.size() - startFrame.size());
                }
            }
        }else {
            if (receivedPacket.size() > startFrame.size()) {
                receivedPacket = receivedPacket.substr(receivedPacket.size() - startFrame.size());
            }
        }
    }
//    Semaphore_post(hSerialSemRxGetHandler(index));
}

Void vForteManagerHandler(UArg arg0, UArg arg1)
{
    ALTOFrameMsgObj msgObj;
    char buff[ALTO_FRAME_MSG_SIZE];
    Mailbox_Handle mbox;
    ComputerBridge_Params *params;
    uint32_t ui32PCSerialPort;
    uint32_t ui32DeviceSerialPort;
    uint32_t ui32AmpSerialPort;
    int n;
    int8_t i8bcc;
    IArg key;
    ALTO_Frame_t tALTOFrame;

    ASSERT(arg0 != NULL);
    ASSERT(arg1 != NULL);

    params = (ComputerBridge_Params *)arg0;
    ui32PCSerialPort = params->ui32PCPortIndex;
    ui32DeviceSerialPort = params->ui32DevicePortIndex;

    mbox = (Mailbox_Handle)arg1;

    while(1) {
        Mailbox_pend(mbox, &msgObj, BIOS_WAIT_FOREVER);
        /*
         * If data received then send it to the ALTO Multinet
         * and wait 20ms for the response
         */
        n = msgObj.msgSize;
        xALTOFrame_convert_ASCII_to_binary(msgObj.buff, &tALTOFrame);
        i8bcc = cALTOFrame_BCC_check(&tALTOFrame);
        if (!i8bcc) {
            switch (tALTOFrame.uin4unitType) {
            case ALTO_ALTONet_Amp:
                ui32AmpSerialPort = g_aForteManagerAmpRoutingMap[tALTOFrame.ui4address];
                if (ui32AmpSerialPort < FORTEMANAGER_AMP_ROUTING_MAP_SIZE) {
                    key = xSerialTransferMutex_enter(ui32AmpSerialPort);
                    {
                        xBSPSerial_sendData(ui32AmpSerialPort, "AA55", 4, 1);
                        xBSPSerial_sendData(ui32AmpSerialPort, msgObj.buff, n, 20);
                        xBSPSerial_sendData(ui32AmpSerialPort, "\r\n", 2, 1);
                        n = xBSPSerial_receiveALTOFrame(ui32AmpSerialPort, buff, 20);
                    }
                    vSerialTransferMutex_leave(ui32AmpSerialPort, key);
                    if (n) {
                        /*
                         * If Response received send it back to the PC
                         */
                        key = xSerialTransferMutex_enter(ui32PCSerialPort);
                        {
                            xBSPSerial_sendData(ui32PCSerialPort, "AA55", 4, 30);
                            xBSPSerial_sendData(ui32PCSerialPort, buff, n, 30);
                            xBSPSerial_sendData(ui32PCSerialPort, "\r\n", 2, 30);
                        }
                        vSerialTransferMutex_leave(ui32PCSerialPort, key);
                    }
                }
                break;
            case ALTO_Multinet_RelayBox:
            case ALTO_Multinet_TemperatureBox:
                key = xSerialTransferMutex_enter(ui32DeviceSerialPort);
                {
                    xBSPSerial_sendData(ui32DeviceSerialPort, "AA55", 4, 1);
                    xBSPSerial_sendData(ui32DeviceSerialPort, msgObj.buff, n, 20);
                    xBSPSerial_sendData(ui32DeviceSerialPort, "\r\n", 2, 1);
                    n = xBSPSerial_receiveALTOFrame(ui32DeviceSerialPort, buff, 20);
                }
                vSerialTransferMutex_leave(ui32DeviceSerialPort, key);
                if (n) {
                    /*
                     * If Response received send it back to the PC
                     */
                    key = xSerialTransferMutex_enter(ui32PCSerialPort);
                    {
                        xBSPSerial_sendData(ui32PCSerialPort, "AA55", 4, 30);
                        xBSPSerial_sendData(ui32PCSerialPort, buff, n, 30);
                        xBSPSerial_sendData(ui32PCSerialPort, "\r\n", 2, 30);
                    }
                    vSerialTransferMutex_leave(ui32PCSerialPort, key);
                }
                break;
            default:
                break;
            }
        }else {
            i8bcc = i8bcc;
        }

    }
}

Void vTestForteManagerFxn(UArg arg0, UArg arg1)
{
    char buff[128];
    char txbuff[70];
    int n;
    IArg key;
    ComputerBridge_Params *params;
    uint32_t ui32PCSerialPort;
    uint32_t ui32DeviceSerialPort;

    ASSERT (arg0 != NULL);

    params = (ComputerBridge_Params *)arg0;
    ui32PCSerialPort = params->ui32PCPortIndex;
    ui32DeviceSerialPort = params->ui32DevicePortIndex;


    GPIO_write(params->ui32PCSerialDEPin, 1);
    GPIO_write(params->ui32PCSerialREPin, 0);

    while(1) {
        /*
         * Read data from PC
         */
        key = xSerialTransferMutex_enter(ui32PCSerialPort);
        {
            n = xBSPSerial_receiveALTOFrame(ui32PCSerialPort, txbuff, BIOS_WAIT_FOREVER);
        }
        vSerialTransferMutex_leave(ui32PCSerialPort, key);

        if (n) {
            /*
             * If data received then send it to the ALTO Multinet
             * and wait 20ms for the response
             */
            key = xSerialTransferMutex_enter(ui32DeviceSerialPort);
            {
                xBSPSerial_sendData(ui32DeviceSerialPort, "AA55", 4, 1);
                xBSPSerial_sendData(ui32DeviceSerialPort, txbuff, n, 20);
                xBSPSerial_sendData(ui32DeviceSerialPort, "\r\n", 2, 1);
                n = xBSPSerial_receiveALTOFrame(ui32DeviceSerialPort, buff, 20);
            }
            vSerialTransferMutex_leave(ui32DeviceSerialPort, key);
            if (n) {
                /*
                 * If Response received send it back to the PC
                 */
                key = xSerialTransferMutex_enter(ui32PCSerialPort);
                {
                    xBSPSerial_sendData(ui32PCSerialPort, "AA55", 4, 30);
                    xBSPSerial_sendData(ui32PCSerialPort, buff, n, 30);
                    xBSPSerial_sendData(ui32PCSerialPort, "\r\n", 2, 30);
                }
                vSerialTransferMutex_leave(ui32PCSerialPort, key);
//                if (n != 64) {
//                    n = n;
//                }
            }
        }
    }
}




void vComputerBridge_Params_init(ComputerBridge_Params *params) {
    params->ui32PCPortIndex = Board_Serial0;
    params->ui32DevicePortIndex = Board_SerialMultinet;
    params->ui32PCSerialDEPin = SMC_SERIAL0_DE;
    params->ui32PCSerialREPin = SMC_SERIAL0_RE;
}

Task_Handle xComputerBridge_init(ComputerBridge_Params *params)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;

    ASSERT(params != NULL);

    /* Make sure Error_Block is initialized */
    Error_init(&eb);


    Task_Params_init(&taskParams);
    taskParams.stackSize = COMPUTERBRIDGE_TASK_RX_STACK_SIZE;
    taskParams.priority = COMPUTERBRIDGE_RX_TASK_PRIORITY;
    taskParams.arg0 = (UArg)params;
    taskHandle = Task_create((Task_FuncPtr)vForteManagerRXFxn, &taskParams, &eb);
//    if (taskHandle == NULL) {
//        System_printf("Failed to create vTestForteManagerFxn Task\n");
//        System_flush();
//    }

    return taskHandle;
}


