/*
 * BSPSerial.h
 *
 *  Created on: Dec 8, 2017
 *      Author: epenate
 */

#ifndef BSPSERIAL_H_
#define BSPSERIAL_H_

#define ALTO_FRAME_CMD_BUFF_SIZE        128

#ifdef __cplusplus
extern "C"  {
#endif

#ifdef  __BSPSERIAL_GLOBAL
    #define __BSPSERIAL_EXT
#else
    #define __BSPSERIAL_EXT  extern
#endif

/*!
 *  ======== vBSPSerial_init ========
 *  Function to initialize the all the UART Serial reference module.
 *
 *  Note: This function is called once in the main.
 */
__BSPSERIAL_EXT void vBSPSerial_init();

/*!
 *  ======== xBSPSerial_sendData ========
 *  A blocking function that sends a specified amount of data.
 *
 *  Function sends size number of bytes from the buffer pointed by pStr.
 *
 *  @param  index   Logical peripheral number for the UART indexed into
 *                  the UART_config table
 *
 *  @param(pStr)    Pointer to a buffer of data
 *
 *  @param(size)    Number of bytes to be sent
 *
 *  @return         Number of bytes added to the USB Buffer for transmission
 */
__BSPSERIAL_EXT unsigned int xBSPSerial_sendData(unsigned int index,
                                     const char *pStr,
                                     unsigned int length,
                                     unsigned int timeout);

/*!
 *  ======== xBSPSerial_receiveData ========
 *  A blocking function to read from the USB Buffer
 *
 *  Function reads up to size number of bytes from the USB Buffer. The return
 *  value will indicate how many bytes were read from the USB Buffer. If there
 *  is no data in the USB Buffer, this function will block timeout number of
 *  ticks. Note: The BIOS_WAIT_FOREVER macro can be used to block indefinitely.
 *
 *  @param  index   Logical peripheral number for the UART indexed into
 *                  the UART_config table
 *
 *  @param(pStr)    Pointer to a buffer to which data is written to.
 *
 *  @param(size)    Maximum number of bytes to be read
 *
 *  @param(timeout) Number of tick to block if no data is currently available in
 *                  the USB Buffer.
 *
 *  @return         Returns the number of bytes actually read from the USB
 *                  Buffer.
 */
__BSPSERIAL_EXT int xBSPSerial_receiveDataSimple(unsigned int index,
                                        char *pStr,
                                        unsigned int length,
                                        unsigned int timeout);

/*!
 *  ======== xBSPSerial_receiveData ========
 *  A blocking function to read from the USB Buffer
 *
 *  Function reads up to size number of bytes from the USB Buffer. The return
 *  value will indicate how many bytes were read from the USB Buffer. If there
 *  is no data in the USB Buffer, this function will block timeout number of
 *  ticks. Note: The BIOS_WAIT_FOREVER macro can be used to block indefinitely.
 *
 *  @param  index   Logical peripheral number for the UART indexed into
 *                  the UART_config table
 *
 *  @param(pStr)    Pointer to a buffer to which data is written to.
 *
 *  @param(size)    Maximum number of bytes to be read
 *
 *  @param(timeout) Number of tick to block if no data is currently available in
 *                  the USB Buffer.
 *
 *  @return         Returns the number of bytes actually read from the USB
 *                  Buffer.
 */
__BSPSERIAL_EXT int xBSPSerial_receiveData(unsigned int index,
                                        char *pStr,
                                        unsigned int length,
                                        unsigned int timeout);

/*!
 *  ======== BSPSerial_receiveData ========
 *  @brief   Command code used by UART_control() to determine how many unsigned
 *           chars are in the read buffer.
 *
 * This command is used to determine how many @c unsigned @c chars are available
 * to read from the UART's circular buffer using UART_read(). With this command
 * code, @b arg is a pointer to an @a integer. @b *arg contains the number of
 * @c unsigned @c chars available to read.
 *
 *  @param  index   Logical peripheral number for the UART indexed into
 *                  the UART_config table
 *
 *  @return         Returns the number of bytes actually read from the USB
 *                  Buffer.
 */
__BSPSERIAL_EXT int xBSPSerial_getRXCount(unsigned int index,
                                          unsigned int timeout);

/*!
 *  ======== BSPSerial_waitForConnect ========
 *  This function blocks while the UART is not connected
 *
 *    @param  index   Logical peripheral number for the UART indexed into
 *                  the UART_config table
 */
__BSPSERIAL_EXT bool bBSPSerial_waitForConnect(unsigned int index,
                                               unsigned int timeout);

/*!
 *  ======== xBSPSerial_receiveALTOFrame ========
 *  A blocking function to read from the USB Buffer
 *
 *  Function reads up to size number of bytes from the USB Buffer. The return
 *  value will indicate how many bytes were read from the USB Buffer. If there
 *  is no data in the USB Buffer, this function will block timeout number of
 *  ticks. Note: The BIOS_WAIT_FOREVER macro can be used to block indefinitely.
 *
 *  @param  index   Logical peripheral number for the UART indexed into
 *                  the UART_config table
 *
 *  @param(p64Buffer)    Pointer to a buffer to which data is written to.
 *
 *  @param(size)    Maximum number of bytes to be read
 *
 *  @param(timeout) Number of tick to block if no data is currently available in
 *                  the USB Buffer.
 *
 *  @return         Returns the number of bytes actually read from the USB
 *                  Buffer.
 */
__BSPSERIAL_EXT int xBSPSerial_receiveALTOFrame(unsigned int index,
                                        char *p64Buffer,
                                        unsigned int timeout);


__BSPSERIAL_EXT IArg xSerialTransferMutex_enter( unsigned int index );
__BSPSERIAL_EXT void vSerialTransferMutex_leave( unsigned int index, IArg key );

__BSPSERIAL_EXT UART_Handle hSerialGetHandler( unsigned int index );

__BSPSERIAL_EXT Semaphore_Handle hSerialSemRxGetHandler( unsigned int index );

__BSPSERIAL_EXT UART_Handle hSerialChangeSpeed( unsigned int index, unsigned int speed );

#ifdef __cplusplus
}
#endif



#endif /* BSPSERIAL_H_ */
