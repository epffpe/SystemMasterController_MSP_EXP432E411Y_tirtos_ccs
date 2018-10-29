/*
 * BSPSerial.c
 *
 *  Created on: Dec 8, 2017
 *      Author: epenate
 */

#define __BSPSERIAL_GLOBAL
#include "includes.h"
#include <ctype.h>
#include <stdio.h>
#include <string>         // std::string
#include <string.h>         // std::string

static UART_Handle g_hBSPSerial_uart[DK_TM4C129X_UARTCOUNT];
static Semaphore_Handle g_hBSPSerial_semTxSerial[DK_TM4C129X_UARTCOUNT];
static Semaphore_Handle g_hBSPSerial_semRxSerial[DK_TM4C129X_UARTCOUNT];
static Semaphore_Handle g_hBSPSerial_semUARTConnected[DK_TM4C129X_UARTCOUNT];
static GateMutexPri_Handle g_hBSPSerial_gateTransfer[DK_TM4C129X_UARTCOUNT];
static GateMutexPri_Handle g_hBSPSerial_gateUARTWait[DK_TM4C129X_UARTCOUNT];

/* Typedefs */
typedef volatile enum {
    UART_STATE_CLOSED = 0,
    UART_STATE_OPENED,
} BSPSerial_UARTState;

/* Static variables and handles */
static volatile BSPSerial_UARTState g_eBSPSerial_gateSerialstate[DK_TM4C129X_UARTCOUNT];

void vBSPSerial_init()
{
    int i32Index;
    UART_Params uartParams;
    Semaphore_Params semParams;
    Error_Block eb;
    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = (unsigned int)115200;

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;

    for (i32Index = SMC_TM4C129X_UART0; i32Index < DK_TM4C129X_UARTCOUNT; i32Index++) {

        g_hBSPSerial_semUARTConnected[i32Index] = Semaphore_create(0, &semParams, &eb);
        if (g_hBSPSerial_semUARTConnected[i32Index] == NULL) {
            System_abort("Can't create UART Connected semaphore");
        }

        g_hBSPSerial_gateUARTWait[i32Index] = GateMutexPri_create(NULL, &eb);
        if (g_hBSPSerial_gateUARTWait[i32Index] == NULL) {
            System_abort("Could not create USB Wait gate");
        }

        g_hBSPSerial_semTxSerial[i32Index] = Semaphore_create(0, &semParams, &eb);
        if (g_hBSPSerial_semTxSerial[i32Index] == NULL) {
            System_abort("Can't create UART TX semaphore");
        }

        g_hBSPSerial_semRxSerial[i32Index] = Semaphore_create(0, &semParams, &eb);
        if (g_hBSPSerial_semRxSerial[i32Index] == NULL) {
            System_abort("Can't create UART RX semaphore");
        }

        g_hBSPSerial_gateTransfer[i32Index] = GateMutexPri_create(NULL, &eb);
        if (g_hBSPSerial_gateTransfer[i32Index] == NULL) {
            System_abort("Can't create gate");
        }

        g_hBSPSerial_uart[i32Index] = UART_open(i32Index, &uartParams);
        if (g_hBSPSerial_uart[i32Index] == NULL) {
            g_eBSPSerial_gateSerialstate[i32Index] = UART_STATE_CLOSED;
            System_abort("Can't open UART");
        }else{
            g_eBSPSerial_gateSerialstate[i32Index] = UART_STATE_OPENED;
            Semaphore_post(g_hBSPSerial_semUARTConnected[i32Index]);
            Semaphore_post(g_hBSPSerial_semTxSerial[i32Index]);
            Semaphore_post(g_hBSPSerial_semRxSerial[i32Index]);
        }
    }
}

unsigned int xBSPSerial_sendData(unsigned int index, const char *pStr, unsigned int length, unsigned int timeout)
{
    unsigned int ui32retValue = 0;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        switch(g_eBSPSerial_gateSerialstate[index]) {
        case UART_STATE_CLOSED:
            bBSPSerial_waitForConnect(index, timeout);
            break;
        case UART_STATE_OPENED:
            if (Semaphore_pend(g_hBSPSerial_semTxSerial[index], timeout)) {
                ui32retValue = UART_write(g_hBSPSerial_uart[index], pStr, length);
                Semaphore_post(g_hBSPSerial_semTxSerial[index]);
            }
            break;
        default:
            break;
        }
    }
    return (ui32retValue);
}



int xBSPSerial_receiveDataSimple(unsigned int index, char *pStr, unsigned int length, unsigned int timeout)
{
    int i32retValue = 0;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        switch(g_eBSPSerial_gateSerialstate[index]) {
        case UART_STATE_CLOSED:
            bBSPSerial_waitForConnect(index, timeout);
            break;
        case UART_STATE_OPENED:
            if (Semaphore_pend(g_hBSPSerial_semRxSerial[index], timeout)) {
                i32retValue = UART_read(g_hBSPSerial_uart[index], pStr, length);
                Semaphore_post(g_hBSPSerial_semRxSerial[index]);
            }
            break;
        default:
            break;
        }
    }
    return (i32retValue);
}

int xBSPSerial_receiveData(unsigned int index, char *pStr, unsigned int length, unsigned int timeout)
{
    int i32retValue = 0;
    int i32dataAvailable = 0, i32CounterRead = 0;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        switch(g_eBSPSerial_gateSerialstate[index]) {
        case UART_STATE_CLOSED:
            bBSPSerial_waitForConnect(index, timeout);
            break;
        case UART_STATE_OPENED:
            if (Semaphore_pend(g_hBSPSerial_semRxSerial[index], timeout)) {
                do {
                    UART_control(g_hBSPSerial_uart[index], UART_CMD_GETRXCOUNT, &i32dataAvailable);
                    if (i32dataAvailable) {
                        if ( (length - i32CounterRead) > i32dataAvailable) {
                            i32retValue = UART_read(g_hBSPSerial_uart[index], pStr, i32dataAvailable);
                        }else{
                            i32retValue = UART_read(g_hBSPSerial_uart[index], pStr, (length - i32CounterRead));
                        }
                        pStr += i32retValue;
                        i32CounterRead += i32retValue;
                    }else{
                        Task_sleep(1);
                        if (timeout) timeout--;
                    }
                }while(timeout && (i32CounterRead < length));
                Semaphore_post(g_hBSPSerial_semRxSerial[index]);
                if (i32CounterRead) i32retValue = i32CounterRead;
            }
            break;
        default:
            break;
        }
    }
    return (i32retValue);
}

int xBSPSerial_getRXCount(unsigned int index, unsigned int timeout)
{
    int i32retValue = 0;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        switch(g_eBSPSerial_gateSerialstate[index]) {
        case UART_STATE_CLOSED:
            bBSPSerial_waitForConnect(index, timeout);
            break;
        case UART_STATE_OPENED:
            UART_control(g_hBSPSerial_uart[index], UART_CMD_GETRXCOUNT, &i32retValue);
            break;
        default:
            break;
        }
    }
    return (i32retValue);
}

bool bBSPSerial_waitForConnect(unsigned int index, unsigned int timeout)
{
    bool ret = true;
    unsigned int key;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        /* Need exclusive access to prevent a race condition */
        key = GateMutexPri_enter(g_hBSPSerial_gateUARTWait[index]);

        if (g_eBSPSerial_gateSerialstate[index] == UART_STATE_CLOSED) {
            if (!Semaphore_pend(g_hBSPSerial_semUARTConnected[index], timeout)) {
                ret = false;
            }
        }

        GateMutexPri_leave(g_hBSPSerial_gateUARTWait[index], key);
    }
    return (ret);
}


int xBSPSerial_receiveALTOFrame(unsigned int index, char *p64Buffer, unsigned int timeout)
{
    int i32retValue = 0;
    int nbytes, i32Index;
    int i32dataAvailable = 0;
    char cmdbuf[ALTO_FRAME_CMD_BUFF_SIZE];
    bool bIsFrame = false;
    std::size_t indexEOF = 0;
    std::size_t indexHeader = 0;
    std::string receivedPacket;
    std::string endFrame ("\r");
    std::string startFrame ("AA55");
    cmdbuf[0] = 0;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        switch(g_eBSPSerial_gateSerialstate[index]) {
        case UART_STATE_CLOSED:
            bBSPSerial_waitForConnect(index, timeout);
            break;
        case UART_STATE_OPENED:
            if (Semaphore_pend(g_hBSPSerial_semRxSerial[index], timeout)) {
                do {
                    UART_control(g_hBSPSerial_uart[index], UART_CMD_GETRXCOUNT, &i32dataAvailable);
                    if (i32dataAvailable) {
                        if ( ALTO_FRAME_CMD_BUFF_SIZE > i32dataAvailable) {
                            nbytes = UART_read(g_hBSPSerial_uart[index], cmdbuf, i32dataAvailable);
                        }else{
                            nbytes = UART_read(g_hBSPSerial_uart[index], cmdbuf, ALTO_FRAME_CMD_BUFF_SIZE);
                        }
                        /*
                         * Convert to Capital letters
                         */
                        for (i32Index = 0; i32Index < nbytes; i32Index++) {
                            if ((cmdbuf[i32Index] >= 'a') && (cmdbuf[i32Index] <= 'z')) {
                                cmdbuf[i32Index] += 'A' - 'a';
                            }
                        }
                        std::string packet(cmdbuf,nbytes);

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
                                            memcpy(p64Buffer, (char *)str.data(), str.size());
                                            i32retValue = str.size();
                                            bIsFrame = true;
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
                    }else{
                        Task_sleep(1);
                        if (timeout && (timeout != BIOS_WAIT_FOREVER)) timeout--;
                    }
                }while(timeout && !bIsFrame);
                Semaphore_post(g_hBSPSerial_semRxSerial[index]);

            }
            break;
        default:
            break;
        }
    }
    return (i32retValue);
}

IArg xSerialTransferMutex_enter( unsigned int index )
{
    IArg key;
    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        /* Acquire lock */
        key = GateMutexPri_enter(g_hBSPSerial_gateTransfer[index]);
    }
    return key;
}

void vSerialTransferMutex_leave( unsigned int index, IArg key )
{
    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    if (index < DK_TM4C129X_UARTCOUNT) {
        /* Release lock */
        GateMutexPri_leave(g_hBSPSerial_gateTransfer[index], key);
    }
}

UART_Handle hSerialGetHandler( unsigned int index )
{
    ASSERT(index < DK_TM4C129X_UARTCOUNT);
    return g_hBSPSerial_uart[index];
}

Semaphore_Handle hSerialSemRxGetHandler( unsigned int index )
{
    ASSERT(index < DK_TM4C129X_UARTCOUNT);
    return g_hBSPSerial_semRxSerial[index];
}

UART_Handle hSerialChangeSpeed( unsigned int index, unsigned int speed )
{
    UART_Params uartParams;

    ASSERT(index < DK_TM4C129X_UARTCOUNT);

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = speed;

    if (index < DK_TM4C129X_UARTCOUNT) {
        switch(g_eBSPSerial_gateSerialstate[index]) {
        case UART_STATE_OPENED:
            Semaphore_pend(g_hBSPSerial_semUARTConnected[index], BIOS_WAIT_FOREVER);
            Semaphore_pend(g_hBSPSerial_semTxSerial[index], BIOS_WAIT_FOREVER);
            Semaphore_pend(g_hBSPSerial_semRxSerial[index], BIOS_WAIT_FOREVER);
            g_eBSPSerial_gateSerialstate[index] = UART_STATE_CLOSED;
        case UART_STATE_CLOSED:
            UART_close(g_hBSPSerial_uart[index]);
            g_hBSPSerial_uart[index] = UART_open(index, &uartParams);
            if (g_hBSPSerial_uart[index] == NULL) {
                g_eBSPSerial_gateSerialstate[index] = UART_STATE_CLOSED;
                System_abort("Can't open UART");
            }else {
                g_eBSPSerial_gateSerialstate[index] = UART_STATE_OPENED;
                Semaphore_post(g_hBSPSerial_semUARTConnected[index]);
                Semaphore_post(g_hBSPSerial_semTxSerial[index]);
                Semaphore_post(g_hBSPSerial_semRxSerial[index]);
            }
            break;
        default:
            break;
        }
    }
    return g_hBSPSerial_uart[index];
}

