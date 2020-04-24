/*
 * Interfaces.h
 *
 *  Created on: Feb 23, 2018
 *      Author: epenate
 */

#ifndef INTERFACES_INTERFACES_H_
#define INTERFACES_INTERFACES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#define IF_ALTO_SERIAL_FRAME_SIZE           64


#ifdef __cplusplus
extern "C"  {
#endif




#ifdef  __INTERFACES_GLOBAL
    #define __INTERFACES_EXT
#else
    #define __INTERFACES_EXT  extern
#endif


typedef enum {
    IF_TRANSFER_TYPE_NONE = 0,
    IF_TRANSFER_TYPE_ALTO_FORTE_MANAGER,
}teIF_TransferType;

typedef enum {
    IF_TRANSACTION_RX_PROTOCOL_NONE = 0,
    IF_TRANSACTION_RX_PROTOCOL_ALTO_MULTINET,
    IF_TRANSACTION_RX_PROTOCOL_ALTO_NET,
    IF_TRANSACTION_RX_PROTOCOL_ROSEN485,
    IF_TRANSACTION_RX_PROTOCOL_AVDS485,
}teIF_TransactionRxProtocol;

/*!
 *  @brief  I2C transfer mode
 *
 *  I2C_MODE_BLOCKING block task execution while a I2C transfer is in progress
 *  I2C_MODE_CALLBACK does not block task execution; but calls a callback
 *  function when the I2C transfer has completed
 */
typedef enum IF_TransferMode {
    IF_MODE_BLOCKING,  /*!< I2C_transfer blocks execution*/
    IF_MODE_CALLBACK   /*!< I2C_transfer queues transactions and does not block */
} IF_TransferMode;


/*!
 *  @brief      A handle that is returned from a IF_open() call.
 */
typedef struct IF_Config      *IF_Handle;

/*!
 *  @brief      Status codes that are set by the interface driver.
 */
typedef enum IF_Status {
    IF_TRANSFER_COMPLETED = 0,
    IF_TRANSFER_STARTED,
    IF_TRANSFER_CANCELED,
    IF_TRANSFER_FAILED,
    IF_TRANSFER_CSN_DEASSERT
} IF_Status;

typedef enum {
    IF_Params_Type_Uart = 0,
}teIF_ParamsType;

/*!
 *  @brief  IF Parameters
 *
 *  IF parameters are used to with the IF_open() call. Default values for
 *  these parameters are set using IF_Params_init().
 *
 *  If IF_TransferMode is set to IF_MODE_BLOCKING then IF_transfer function
 *  calls will block thread execution until the transaction has completed.
 *
 *  If IF_TransferMode is set to IF_MODE_CALLBACK then IF_transfer will not
 *  block thread execution and it will call the function specified by
 *  transferCallbackFxn. Sequential calls to IF_transfer in IF_MODE_CALLBACK
 *  mode will put the designated transaction onto an internal queue that
 *  automatically starts queued transactions after the previous transaction has
 *  completed. (regardless of error state).
 *
 *  IF_BitRate specifies the IF bus rate used for IF communications.
 *
 *  @sa     IF_Params_init()
 */
typedef struct IF_Params {
    UART_Params         uartParams;
    uintptr_t           custom;  /*!< Custom argument used by driver
                                      implementation */
} IF_Params;

/*!
 *  @brief  IF transaction
 *
 *  This structure defines the nature of the IF transaction. This structure
 *  specifies the buffer and buffer's size that is to be written to or read from
 *  the IF slave peripheral.
 *  arg is an optional user supplied argument that will be passed
 *  to the callback function when the IF driver is in IF_MODE_CALLBACK.
 *  nextPtr is to be only used by the IF driver.
 */
typedef struct IF_Transaction {
    uint32_t transferType;
    uint32_t transactionRxProtocol; /* Select between RAW, ALTONET, ALTOMULTINET */
    void    *writeBuf;    /*!< buffer containing data to be written */
    size_t  writeCount;   /*!< Number of bytes to be written to the slave */
    uint32_t writeTimeout;

    void    *readBuf;     /*!< buffer to which data is to be read into */
    size_t  readCount;    /*!< Number of bytes to be read from the slave */
    uint32_t readTimeout;

    uint32_t slaveAddress; /*!< Address of the IF slave device */

    void    *arg;         /*!< argument to be passed to the callback function */
    void    *nextPtr;     /*!< used for queuing in IF_MODE_CALLBACK mode */
} IF_Transaction;



/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              IF_close().
 */
typedef void        (*IF_CloseFxn)    (IF_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              IF_control().
 */
typedef int         (*IF_ControlFxn)  (IF_Handle handle,
                                        unsigned int cmd,
                                        void *arg);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              IF_init().
 */
typedef void        (*IF_InitFxn)     (IF_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              IF_open().
 */
typedef IF_Handle  (*IF_OpenFxn)     (IF_Handle handle,
                                        IF_Params *params);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              IF_transfer().
 */
typedef bool        (*IF_TransferFxn) (IF_Handle handle,
                                        IF_Transaction *transaction);

/*!
 *  @brief      The definition of a IF function table that contains the
 *              required set of functions to control a specific IF driver
 *              implementation.
 */
typedef struct IF_FxnTable {
    /*! Function to close the specified peripheral */
    IF_CloseFxn        closeFxn;

    /*! Function to implementation specific control function */
    IF_ControlFxn      controlFxn;

    /*! Function to initialize the given data object */
    IF_InitFxn         initFxn;

    /*! Function to open the specified peripheral */
    IF_OpenFxn         openFxn;

    /*! Function to initiate a IF data transfer */
    IF_TransferFxn     transferFxn;
} IF_FxnTable;

/*!
 *  @brief  IF Global configuration
 *
 *  The IF_Config structure contains a set of pointers used to characterize
 *  the IF driver implementation.
 *
 *  This structure needs to be defined before calling IF_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     IF_init()
 */
typedef struct IF_Config {
    /*! Pointer to a table of driver-specific implementations of IF APIs */
    IF_FxnTable const *fxnTablePtr;

    /*! Pointer to a driver specific data object */
    void               *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void              *hwAttrs;
} IF_Config;



/*!
 *  @brief  Function to close a IF peripheral specified by the IF handle
 *
 *  @pre    IF_open() had to be called first.
 *
 *  @param  handle  A IF_Handle returned from IF_open
 *
 *  @sa     IF_open()
 */
extern void vIF_close(IF_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a given
 *          IF_Handle.
 *
 *  Commands for IF_control can originate from IF.h or from implementation
 *  specific IF*.h (_IFCC26XX.h_, _IFTiva.h_, etc.. ) files.
 *  While commands from IF.h are API portable across driver implementations,
 *  not all implementations may support all these commands.
 *  Conversely, commands from driver implementation specific IF*.h files add
 *  unique driver capabilities but are not API portable across all IF driver
 *  implementations.
 *
 *  Commands supported by IF.h follow a IF_CMD_\<cmd\> naming
 *  convention.<br>
 *  Commands supported by IF*.h follow a IF*_CMD_\<cmd\> naming
 *  convention.<br>
 *  Each control command defines @b arg differently. The types of @b arg are
 *  documented with each command.
 *
 *  See @ref IF_CMD "IF_control command codes" for command codes.
 *
 *  See @ref IF_STATUS "IF_control return status codes" for status codes.
 *
 *  @pre    IF_open() has to be called first.
 *
 *  @param  handle      A IF handle returned from IF_open()
 *
 *  @param  cmd         IF.h or IF*.h commands.
 *
 *  @param  arg         An optional R/W (read/write) command argument
 *                      accompanied with cmd
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @sa     IF_open()
 */
extern int xIF_control(IF_Handle handle, unsigned int cmd, void *arg);

/*!
 *  @brief  Function to initializes the IF module
 *
 *  @pre    The IF_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other IF driver APIs. This function call does not modify any
 *          peripheral registers.
 */
extern void vIF_init(void);

/*!
 *  @brief  Function to initialize a given IF peripheral specified by the
 *          particular index value. The parameter specifies which mode the IF
 *          will operate.
 *
 *  @pre    IF controller has been initialized
 *
 *  @param  index         Logical peripheral number for the IF indexed into
 *                        the IF_config table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values. All the fields in this structure are
 *                        RO (read-only).
 *
 *  @return A IF_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     IF_init()
 *  @sa     IF_close()
 */
extern IF_Handle hIF_open(unsigned int index, IF_Params *params);

/*!
 *  @brief  Function to initialize the IF_Params struct to its defaults
 *
 *  @param  params      An pointer to IF_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      transferMode = IF_MODE_BLOCKING
 *      transferCallbackFxn = NULL
 *      bitRate = IF_100kHz
 */
extern void vIF_Params_init(IF_Params *params, teIF_ParamsType paramType);

/*!
 *  @brief  Function to perform an IF transaction with an IF slave peripheral.
 *
 *  This function will start a IF transfer and can only be called from a Task
 *  context when in IF_MODE_BLOCKING.
 *  The IF transfer procedure starts with evaluating how many bytes are to be
 *  written and how many are to be read from the IF peripheral. Due to common
 *  IF data transfer processes, to be written will always sent before any data
 *  is read.
 *
 *  The data written to the peripheral is preceded with the peripheral's 7-bit
 *  IF slave address (with the Write bit set).
 *  After all the data has been transmitted, the driver will evaluate if any
 *  data needs to be read from the device.
 *  If so, a Re-START bit is sent, along with the same 7-bit IF slave address
 *  (with the Read bit). Else, the transfer is concluded with a STOP bit.
 *  After the specified number of bytes have been read by the IF, the transfer
 *  is ended with a NACK and STOP bit.
 *
 *  In IF_MODE_BLOCKING, IF_transfer will block task execution until the
 *  transaction has completed.
 *
 *  In IF_MODE_CALLBACK, IF_transfer does not block task execution and calls a
 *  callback function specified by transferCallbackFxn when the transfer
 *  completed. Success or failure of the transaction is determined via the
 *  callback function's bool argument. If a transfer is already taking place,
 *  the transaction is put on an internal queue. The queue is serviced in a
 *  first come first served basis.
 *  The IF_Transaction structure must stay persistent until the IF_transfer
 *  function has completed!
 *
 *  @param  handle      A IF_Handle
 *
 *  @param  transaction A pointer to a IF_Transaction. All of the fields within
 *                      transaction are WO (write-only) unless otherwise noted
 *                      in the driver implementations.
 *
 *  @return In IF_MODE_BLOCKING: true on successful transfer; false on an
 *          error, such as an IF bus fault (NACK).
 *          In IF_MODE_CALLBACK: always true. The transferCallbackFxn's bool
 *          argument will be true if successful; false on an error, such as an
 *          IF bus fault (NACK).
 *
 *  @sa     IF_open
 */
extern bool bIF_transfer(IF_Handle handle, IF_Transaction *transaction);



#ifdef __cplusplus
}
#endif


#endif /* INTERFACES_INTERFACES_H_ */
