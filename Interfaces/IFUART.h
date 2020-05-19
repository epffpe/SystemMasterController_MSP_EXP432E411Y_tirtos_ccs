/*
 * IFUART.h
 *
 *  Created on: Feb 26, 2018
 *      Author: epenate
 */

#ifndef INTERFACES_IFUART_H_
#define INTERFACES_IFUART_H_


#ifdef __cplusplus
extern "C"  {
#endif

#define IF_UART_SERIAL_DRIVER_ENABLE        1
#define IF_UART_SERIAL_DRIVER_DISABLE       0
#define IF_UART_SERIAL_RECEIVER_ENABLE      0
#define IF_UART_SERIAL_RECEIVER_DISABLE     1


/*!
 *  @brief      UARTTiva Hardware attributes
 *
 *  The baseAddr, intNum, and flowControl fields
 *  are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For TivaWare these definitions are found in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *      - driverlib/uart.h
 *
 *  intPriority is the UART peripheral's interrupt priority, as defined by the
 *  underlying OS.  It is passed unmodified to the underlying OS's interrupt
 *  handler creation code, so you need to refer to the OS documentation
 *  for usage.  For example, for SYS/BIOS applications, refer to the
 *  ti.sysbios.family.arm.m3.Hwi documentation for SYS/BIOS usage of
 *  interrupt priorities.  If the driver uses the ti.drivers.ports interface
 *  instead of making OS calls directly, then the HwiP port handles the
 *  interrupt priority in an OS specific way.  In the case of the SYS/BIOS
 *  port, intPriority is passed unmodified to Hwi_create().
 *
 *  A sample structure is shown below:
 *  @code
 *  unsigned char uartTivaRingBuffer[2][32];
 *
 *  const UARTTiva_HWAttrs uartTivaHWAttrs[] = {
 *      {
 *          .baseAddr = UART0_BASE,
 *          .intNum = INT_UART0,
 *          .intPriority = (~0),
 *          .flowControl = UART_FLOWCONTROL_NONE,
 *          .ringBufPtr  = uartTivaRingBuffer[0],
 *          .ringBufSize = sizeof(uartTivaRingBuffer[0])
 *      },
 *      {
 *          .baseAddr = UART1_BASE,
 *          .intNum = INT_UART1,
 *          .intPriority = (~0),
 *          .flowControl = UART_FLOWCONTROL_NONE,
 *          .ringBufPtr  = uartTivaRingBuffer[1],
 *          .ringBufSize = sizeof(uartTivaRingBuffer[1])
 *      }
 *  };
 *  @endcode
 */
typedef struct IFUART_HWAttrs {
    uint32_t          uartIndex;
    uint32_t          driverEnablePin;
    uint32_t          receiverEnablePin;
    uintptr_t         custom;           /*!< Custom argument used by driver
                                             implementation */
} IFUART_HWAttrs;

/*!
 *  @brief      UARTTiva Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct IFUART_Object {
    /* UART state variable */
    struct {
        bool             opened:1;         /* Has the obj been opened */
//        UART_Mode        readMode:1;       /* Mode for all read calls */
//        UART_Mode        writeMode:1;      /* Mode for all write calls */
//        UART_DataMode    readDataMode:1;   /* Type of data being read */
//        UART_DataMode    writeDataMode:1;  /* Type of data being written */
//        UART_ReturnMode  readReturnMode:1; /* Receive return mode */
//        UART_Echo        readEcho:1;       /* Echo received data back */
//        /*
//         * Flag to determine if a timeout has occurred when the user called
//         * UART_read(). This flag is set by the timeoutClk clock object.
//         */
//        bool             bufTimeout:1;
//        /*
//         * Flag to determine when an ISR needs to perform a callback; in both
//         * UART_MODE_BLOCKING or UART_MODE_CALLBACK
//         */
//        bool             callCallback:1;
//        /*
//         * Flag to determine if the ISR is in control draining the ring buffer
//         * when in UART_MODE_CALLBACK
//         */
//        bool             drainByISR:1;
//        /* Flag to keep the state of the read ring buffer */
//        bool             rxEnabled:1;
        /*
         * Flag to enable the special Interface for the ALTO Forte Manager
         */
        bool            isForteManger:1;

    } state;

//    Semaphore_Struct    mutex;            /* Grants exclusive access to IF */
//    Semaphore_Struct    transferComplete; /* Notify finished IF transfer */
    Clock_Handle       timeoutClk;       /* Clock object to for timeouts */

    IF_TransferMode    transferMode;        /* Blocking or Callback mode */

    IF_Transaction      *currentTransaction; /* Pointer to current IF transaction */
    uint32_t             transactionRxMode; /* Select between RAW, ALTONET, ALTOMULTINET */

    uint8_t            *writeBufIdx;    /* Internal inc. writeBuf index */
    size_t              writeCountIdx;  /* Internal dec. writeCounter */

    uint8_t            *readBufIdx;     /* Internal inc. readBuf index */
    size_t              readCountIdx;   /* Internal dec. readCounter */

    /* IF transaction pointers for IF_MODE_CALLBACK */
    IF_Transaction    *headPtr;        /* Head ptr for queued transactions */
    IF_Transaction    *tailPtr;        /* Tail ptr for queued transactions */

//    bool                isOpen;         /* flag to indicate module is open */

    UART_Handle hBSPSerial_uart;
    Semaphore_Handle hBSPSerial_semTxSerial;
    Semaphore_Handle hBSPSerial_semRxSerial;
    Semaphore_Handle hBSPSerial_semUARTConnected;
    GateMutexPri_Handle hBSPSerial_gateTransfer;
    GateMutexPri_Handle hBSPSerial_gateUARTWait;
    GateMutexPri_Handle hBSPSerial_gateUARTOpen;
} IFUART_Object, *IFUART_Handle;





#ifdef  __INTERFACES_IFUART_GLOBAL
    #define __INTERFACES_IFUART_EXT
#else
    #define __INTERFACES_IFUART_EXT  extern
#endif


__INTERFACES_IFUART_EXT
unsigned int xIFUART_sendData(IF_Handle handle, const char *pStr, unsigned int length, unsigned int timeout);
__INTERFACES_IFUART_EXT
int xIFUART_receiveALTOFrame(IF_Handle handle, char *p64Buffer, unsigned int timeout);
__INTERFACES_IFUART_EXT
int xIFUART_receiveALTOFrameFSMData(IF_Handle handle, char *p64Buffer, unsigned int length, unsigned int timeout);

extern const IF_FxnTable IFUART_fxnTable;


#ifdef __cplusplus
}
#endif



#endif /* INTERFACES_IFUART_H_ */
