/*
 * IFUART.c
 *
 *  Created on: Feb 26, 2018
 *      Author: epenate
 */

#define __INTERFACES_IFUART_GLOBAL
#include "includes.h"
#include <ctype.h>
#include <stdio.h>
#include <string>         // std::string
#include <string.h>         // std::string
#include <ti/drivers/uart/UARTMSP432E4.h>


extern const UART_Config UART_config[];
extern const uint_least8_t UART_count;

void         vIFUART_close(IF_Handle handle);
int          xIFUART_control(IF_Handle handle, unsigned int cmd, void *arg);
void         vIFUART_init(IF_Handle handle);
IF_Handle    hIFUART_open(IF_Handle handle, IF_Params *params);
bool         bIFUART_transfer(IF_Handle handle, IF_Transaction *transaction);




unsigned int xIFUART_sendData(IF_Handle handle, const char *pStr, unsigned int length, unsigned int timeout);
int xIFUART_receiveDataSimple(IF_Handle handle, char *pStr, unsigned int length, unsigned int timeout);
int xIFUART_receiveData(IF_Handle handle, char *pStr, unsigned int length, unsigned int timeout);
int xIFUART_getRXCount(IF_Handle handle, unsigned int timeout);
bool bIFUART_waitForConnect(IF_Handle handle, unsigned int timeout);

int xIFUART_receiveALTOFrame(IF_Handle handle, char *p64Buffer, unsigned int timeout);

static Void vDriverEnableTimeout(UArg arg);


const IF_FxnTable IFUART_fxnTable =
{
 vIFUART_close,
 xIFUART_control,
 vIFUART_init,
 hIFUART_open,
 bIFUART_transfer
};

const char g_aIFUART_SerialMap[] = {3, 5, 4, 0, 2, 6, 1, 7};

void vIFUART_close(IF_Handle handle)
{

}

int xIFUART_control(IF_Handle handle, unsigned int cmd, void *arg)
{
    return 0;
}


void vIFUART_init(IF_Handle handle)
{
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;
    Error_Block eb;

    Error_init(&eb);

    Display_printf(g_SMCDisplay, 0, 0, (char *)"Initializing Serial Port (%d)\n", g_aIFUART_SerialMap[hwAttrs->uartIndex]);

    object->state.opened = false;

    object->hBSPSerial_gateUARTOpen = GateMutexPri_create(NULL, &eb);
    if (object->hBSPSerial_gateUARTOpen == NULL) {
        System_abort("Could not create USB Wait gate");
    }
}

IF_Handle hIFUART_open(IF_Handle handle, IF_Params *params)
{
    unsigned int key;
    Error_Block eb;
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;
    union {
        Semaphore_Params        semParams;
        UART_Params             uartParams;
        Clock_Params            clockParams;
    } paramsUnion;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(object->hBSPSerial_gateUARTOpen);

//    key = Hwi_disable();
    if(object->state.opened == true) {
//        Hwi_restore(key);
        GateMutexPri_leave(object->hBSPSerial_gateUARTOpen, key);
//        Log_warning1("UART:(%p) already in use.", hwAttrs->baseAddr);
        return handle;
    }
//    object->state.opened = true;

//    Hwi_restore(key);
    Error_init(&eb);


    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&paramsUnion.semParams);
    paramsUnion.semParams.mode = Semaphore_Mode_BINARY;

    object->hBSPSerial_semUARTConnected = Semaphore_create(0, &paramsUnion.semParams, &eb);
    if (object->hBSPSerial_semUARTConnected == NULL) {
        System_abort("Can't create UART Connected semaphore");
    }

    object->hBSPSerial_gateUARTWait = GateMutexPri_create(NULL, &eb);
    if (object->hBSPSerial_gateUARTWait == NULL) {
        System_abort("Could not create USB Wait gate");
    }

    object->hBSPSerial_semTxSerial = Semaphore_create(0, &paramsUnion.semParams, &eb);
    if (object->hBSPSerial_semTxSerial == NULL) {
        System_abort("Can't create UART TX semaphore");
    }

    object->hBSPSerial_semRxSerial = Semaphore_create(0, &paramsUnion.semParams, &eb);
    if (object->hBSPSerial_semRxSerial == NULL) {
        System_abort("Can't create UART RX semaphore");
    }

    object->hBSPSerial_gateTransfer = GateMutexPri_create(NULL, &eb);
    if (object->hBSPSerial_gateTransfer == NULL) {
        System_abort("Can't create gate");
    }


//    UART_Params_init(&paramsUnion.uartParams);
//    paramsUnion.uartParams.writeDataMode = UART_DATA_BINARY;
//    paramsUnion.uartParams.readDataMode = UART_DATA_BINARY;
//    paramsUnion.uartParams.readReturnMode = UART_RETURN_FULL;
//    paramsUnion.uartParams.readEcho = UART_ECHO_OFF;
//    paramsUnion.uartParams.baudRate = (unsigned int)115200;
    paramsUnion.uartParams = params->uartParams;

    object->hBSPSerial_uart = UART_open(hwAttrs->uartIndex, &paramsUnion.uartParams);
    if (object->hBSPSerial_uart == NULL) {

        UART_Handle handle = NULL;

        if (hwAttrs->uartIndex < UART_count) {
            /* If params are NULL use defaults */
//            if (params == NULL) {
//                params = (UART_Params *) &UART_defaultParams;
//            }

            /* Get handle for this driver instance */
            handle = (UART_Handle)&(UART_config[hwAttrs->uartIndex]);
            UARTMSP432E4_Object        *uObject = (UARTMSP432E4_Object *)handle->object;
            if (uObject->state.opened) {
                object->hBSPSerial_uart = handle;
            }else {
                object->state.opened = false;
                System_abort("Can't open UART");
            }
        }

    }else{
        Semaphore_post(object->hBSPSerial_semUARTConnected);
        Semaphore_post(object->hBSPSerial_semTxSerial);
        Semaphore_post(object->hBSPSerial_semRxSerial);
        object->state.opened = true;

        Display_printf(g_SMCDisplay, 0, 0, (char *)"Serial Port (%d) oppened\n", g_aIFUART_SerialMap[hwAttrs->uartIndex]);

        /* Clock to disable driverEnablePin */
        if (hwAttrs->driverEnablePin && (hwAttrs->driverEnablePin < Board_GPIOCount)) {
            Clock_Params_init(&paramsUnion.clockParams);
            paramsUnion.clockParams.period = 0;
            paramsUnion.clockParams.startFlag = FALSE;
            paramsUnion.clockParams.arg = (UArg)handle;
            object->timeoutClk = Clock_create(vDriverEnableTimeout,
                                              2 + (10 * 16 * 1000)/ params->uartParams.baudRate, // 10bits/byte * 16bytes(fifo) / baudrate / 1ms
                                              &paramsUnion.clockParams,
                                              &eb);
            //        Clock_destruct(&object->timeoutClk);
        }
    }

    GateMutexPri_leave(object->hBSPSerial_gateUARTOpen, key);
    return handle;
}

bool bIFUART_transfer(IF_Handle handle, IF_Transaction *transaction)
{
    bool                    ret = false;
    unsigned int            key;
    unsigned int            ui32retValue = 0;
    IFUART_Object         *object = (IFUART_Object *)handle->object;
//    IFUART_HWAttrs        *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;

    ASSERT (handle != NULL);
    ASSERT (object != NULL);
    ASSERT (hwAttrs != NULL);
    ASSERT (transaction != NULL);

    if (handle == NULL) {
        return (false);
    }

    /* Check if anything needs to be written or read */
    if ((!transaction->writeCount) && (!transaction->readCount)) {
        /* Nothing to write or read */
        return (ret);
    }

    switch (transaction->transferType) {
    case IF_TRANSFER_TYPE_ALTO_FORTE_MANAGER:
        ret = true;
        key = GateMutexPri_enter(object->hBSPSerial_gateTransfer);
        {
            switch(transaction->transactionRxProtocol) {
            case IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET:
            case IF_TRANSACTION_RX_PROTOCOL_ALTO_NET:
                if (transaction->readCount >= IF_ALTO_SERIAL_FRAME_SIZE) {
                    ui32retValue = xIFUART_receiveALTOFrame(handle,
                                                            (char *)transaction->readBuf,
                                                            transaction->readTimeout);
                    if (ui32retValue != transaction->readCount) ret = false;
                    transaction->readCount = ui32retValue;
                }else {
                    ret = false;
                }
                break;
            default:
                break;
            }
        }
        GateMutexPri_leave(object->hBSPSerial_gateTransfer, key);
        break;
    default:
        ret = true;
        key = GateMutexPri_enter(object->hBSPSerial_gateTransfer);
        {
            if (transaction->writeCount) {
                ui32retValue = xIFUART_sendData(handle,
                                                (const char *)transaction->writeBuf,
                                                transaction->writeCount,
                                                transaction->writeTimeout);
                if (ui32retValue != transaction->writeCount) ret = false;
                transaction->writeCount = ui32retValue;
            }

            if (transaction->readCount) {
                switch(transaction->transactionRxProtocol) {
                case IF_TRANSACTION_RX_PROTOCOL_NONE:
                    if (transaction->readCount) {
                        ui32retValue = xIFUART_receiveDataSimple(handle,
                                                                 (char *)transaction->readBuf,
                                                                 transaction->readCount,
                                                                 transaction->readTimeout);
                        if (ui32retValue != transaction->readCount) ret = false;
                        transaction->readCount = ui32retValue;
                    }
                    break;
                case IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET:
                case IF_TRANSACTION_RX_PROTOCOL_ALTO_NET:
                    if (transaction->readCount >= IF_ALTO_SERIAL_FRAME_SIZE) {
                        ui32retValue = xIFUART_receiveALTOFrame(handle,
                                                                (char *)transaction->readBuf,
                                                                transaction->readTimeout);
                        if (ui32retValue != transaction->readCount) ret = false;
                        transaction->readCount = ui32retValue;
                    }else {
                        ret = false;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        GateMutexPri_leave(object->hBSPSerial_gateTransfer, key);
        break;
    }
    return ret;
}


unsigned int xIFUART_sendData(IF_Handle handle, const char *pStr, unsigned int length, unsigned int timeout)
{
    unsigned int ui32retValue = 0;
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);
    ASSERT(pStr != NULL);

    if (handle == NULL) {
        return (NULL);
    }
    if (pStr == NULL) {
        return (NULL);
    }

    if (object->state.opened) {
        if (Semaphore_pend(object->hBSPSerial_semTxSerial, timeout)) {
            if (hwAttrs->driverEnablePin && (hwAttrs->driverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->driverEnablePin, IF_UART_SERIAL_DRIVER_ENABLE);
            }

            ui32retValue = UART_write(object->hBSPSerial_uart, pStr, length);

            Semaphore_post(object->hBSPSerial_semTxSerial);
            if (hwAttrs->driverEnablePin && (hwAttrs->driverEnablePin < Board_GPIOCount)) {
                Clock_start(object->timeoutClk);
//                Clock_start(Clock_handle(&object->timeoutClk));
                //            Clock_stop(Clock_handle(&object->timeoutClk));
            }
        }
    }else {
        bIFUART_waitForConnect(handle, timeout);
    }
    return (ui32retValue);
}

int xIFUART_receiveDataSimple(IF_Handle handle, char *pStr, unsigned int length, unsigned int timeout)
{
    int i32retValue = 0;
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    if (object->state.opened) {
        if (Semaphore_pend(object->hBSPSerial_semRxSerial, timeout)) {
            if (hwAttrs->receiverEnablePin && (hwAttrs->receiverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->receiverEnablePin, IF_UART_SERIAL_RECEIVER_ENABLE);
            }
            i32retValue = UART_read(object->hBSPSerial_uart, pStr, length);
            if (hwAttrs->receiverEnablePin && (hwAttrs->receiverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->receiverEnablePin, IF_UART_SERIAL_RECEIVER_DISABLE);
            }
            Semaphore_post(object->hBSPSerial_semRxSerial);
        }
    }else {
        bIFUART_waitForConnect(handle, timeout);
    }
    return (i32retValue);
}

int xIFUART_receiveData(IF_Handle handle, char *pStr, unsigned int length, unsigned int timeout)
{
    int i32retValue = 0;
    int i32dataAvailable = 0, i32CounterRead = 0;
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;


    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    if (object->state.opened) {
        if (Semaphore_pend(object->hBSPSerial_semRxSerial, timeout)) {
            if (hwAttrs->receiverEnablePin && (hwAttrs->receiverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->receiverEnablePin, IF_UART_SERIAL_RECEIVER_ENABLE);
            }
            do {
                UART_control(object->hBSPSerial_uart, UART_CMD_GETRXCOUNT, &i32dataAvailable);
                if (i32dataAvailable) {
                    if ( (length - i32CounterRead) > i32dataAvailable) {
                        i32retValue = UART_read(object->hBSPSerial_uart, pStr, i32dataAvailable);
                    }else{
                        i32retValue = UART_read(object->hBSPSerial_uart, pStr, (length - i32CounterRead));
                    }
                    pStr += i32retValue;
                    i32CounterRead += i32retValue;
                }else{
                    Task_sleep(1);
                    if (timeout) timeout--;
                }
            }while(timeout && (i32CounterRead < length));
            if (hwAttrs->receiverEnablePin && (hwAttrs->receiverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->receiverEnablePin, IF_UART_SERIAL_RECEIVER_DISABLE);
            }
            Semaphore_post(object->hBSPSerial_semRxSerial);
            if (i32CounterRead) i32retValue = i32CounterRead;
        }
    }else {
        bIFUART_waitForConnect(handle, timeout);
    }
    return (i32retValue);
}

int xIFUART_getRXCount(IF_Handle handle, unsigned int timeout)
{
    int i32retValue = 0;
    IFUART_Object *object = (IFUART_Object *)handle->object;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }
    if (object->state.opened) {
        UART_control(object->hBSPSerial_uart, UART_CMD_GETRXCOUNT, &i32retValue);
    }else {
        bIFUART_waitForConnect(handle, timeout);
    }
    return (i32retValue);
}


bool bIFUART_waitForConnect(IF_Handle handle, unsigned int timeout)
{
    bool ret = true;
    unsigned int key;
    IFUART_Object *object = (IFUART_Object *)handle->object;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    /* Need exclusive access to prevent a race condition */
    key = GateMutexPri_enter(object->hBSPSerial_gateUARTWait);

    if (!object->state.opened) {
        if (!Semaphore_pend(object->hBSPSerial_semUARTConnected, timeout)) {
            ret = false;
        }
    }
    GateMutexPri_leave(object->hBSPSerial_gateUARTWait, key);
    return (ret);
}

IF_Handle hIFUART_changeSpeed( IF_Handle handle, unsigned int speed )
{
    UART_Params uartParams;
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = speed;

    if (object->state.opened) {
        Semaphore_pend(object->hBSPSerial_semUARTConnected, BIOS_WAIT_FOREVER);
        Semaphore_pend(object->hBSPSerial_semTxSerial, BIOS_WAIT_FOREVER);
        Semaphore_pend(object->hBSPSerial_semRxSerial, BIOS_WAIT_FOREVER);
        object->state.opened = false;
    }
    UART_close(object->hBSPSerial_uart);
    object->hBSPSerial_uart = UART_open(hwAttrs->uartIndex, &uartParams);
    if (object->hBSPSerial_uart == NULL) {
        object->state.opened = false;
        System_abort("Can't open UART");
    }else {
        object->state.opened = true;
        Semaphore_post(object->hBSPSerial_semUARTConnected);
        Semaphore_post(object->hBSPSerial_semTxSerial);
        Semaphore_post(object->hBSPSerial_semRxSerial);
    }
    return handle;
}

#ifndef ALTO_FRAME_CMD_BUFF_SIZE
#define ALTO_FRAME_CMD_BUFF_SIZE        128
#endif
int xIFUART_receiveALTOFrame(IF_Handle handle, char *p64Buffer, unsigned int timeout)
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
    IFUART_Object *object = (IFUART_Object *)handle->object;
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)handle->hwAttrs;

    ASSERT(handle != NULL);

    if (handle == NULL) {
        return (NULL);
    }

    cmdbuf[0] = 0;

    if (object->state.opened) {
        if (Semaphore_pend(object->hBSPSerial_semRxSerial, timeout)) {
            if (hwAttrs->receiverEnablePin && (hwAttrs->receiverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->receiverEnablePin, IF_UART_SERIAL_RECEIVER_ENABLE);
            }
            do {
                UART_control(object->hBSPSerial_uart, UART_CMD_GETRXCOUNT, &i32dataAvailable);
                if (i32dataAvailable) {
                    if ( ALTO_FRAME_CMD_BUFF_SIZE > i32dataAvailable) {
                        nbytes = UART_read(object->hBSPSerial_uart, cmdbuf, i32dataAvailable);
                    }else{
                        nbytes = UART_read(object->hBSPSerial_uart, cmdbuf, ALTO_FRAME_CMD_BUFF_SIZE);
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
                                    if (str.size() <= IF_ALTO_SERIAL_FRAME_SIZE) {
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
            if (hwAttrs->receiverEnablePin && (hwAttrs->receiverEnablePin < Board_GPIOCount)) {
                GPIO_write(hwAttrs->receiverEnablePin, IF_UART_SERIAL_RECEIVER_DISABLE);
            }
            Semaphore_post(object->hBSPSerial_semRxSerial);

        }
    }else {
        bIFUART_waitForConnect(handle, timeout);
    }


    return (i32retValue);
}


/*
 *  ======== readBlockingTimeout ========
 */
static Void vDriverEnableTimeout(UArg arg)
{
    IFUART_HWAttrs *hwAttrs = (IFUART_HWAttrs *)((UART_Handle)arg)->hwAttrs;

    if (hwAttrs->driverEnablePin && (hwAttrs->driverEnablePin < Board_GPIOCount)) {
        GPIO_write(hwAttrs->driverEnablePin, IF_UART_SERIAL_DRIVER_DISABLE);
    }
}
