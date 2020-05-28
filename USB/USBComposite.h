/*
 * USBComposite.h
 *
 *  Created on: May 27, 2020
 *      Author: epffpe
 */

#ifndef USB_USBCOMPOSITE_H_
#define USB_USBCOMPOSITE_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __USB_USBCOMPOSITE_GLOBAL
    #define __USB_USBCOMPOSITE_EXT
#else
    #define __USB_USBCOMPOSITE_EXT  extern
#endif

#define WAIT_FOREVER    (~(0))

/*!
 *  ======== USBComposite_init ========
 *  Function to initialize the USB serial/others reference module.
 *
 *  Note: This function is not reentrant safe.
 */
__USB_USBCOMPOSITE_EXT
void USBComposite_init(bool usbInternal);

/*!
 *  ======== USBCDCD_sendData ========
 *  A blocking function that sends a specified amount of data.
 *
 *  Function sends size number of bytes from the buffer pointed by pStr.
 *
 *  @param(pStr)    Pointer to a buffer of data
 *
 *  @param(size)    Number of bytes to be sent
 *
 *  @return         Number of bytes added to the USB Buffer for transmission
 */
__USB_USBCOMPOSITE_EXT
unsigned int USBCDCD_sendData(char *pStr,
                                     unsigned int length,
                                     unsigned int timeout);

/*!
 *  ======== USBCDCD_receiveData ========
 *  A blocking function to read from the USB Buffer
 *
 *  Function reads up to size number of bytes from the USB Buffer. The return
 *  value will indicate how many bytes were read from the USB Buffer. If there
 *  is no data in the USB Buffer, this function will block timeout number of
 *  ticks. Note: The WAIT_FOREVER macro can be used to block indefinitely.
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
__USB_USBCOMPOSITE_EXT
unsigned int USBCDCD_receiveData(unsigned char *pStr,
                                        unsigned int length,
                                        unsigned int timeout);

/*!
 *  ======== USBCDCD_waitForConnect ========
 *  This function blocks while the USB is not connected
 */
__USB_USBCOMPOSITE_EXT
bool USBCDCD_waitForConnect(unsigned int timeout);

#ifdef __cplusplus
}
#endif




#endif /* USB_USBCOMPOSITE_H_ */
