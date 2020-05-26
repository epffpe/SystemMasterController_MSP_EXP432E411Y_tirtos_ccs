/*
 * includes.h
 *
 *  Created on: Oct 18, 2016
 *      Author: epenate
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* For usleep() */
#include <unistd.h>
#include <stddef.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>


///* BSD support */
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
//
//#include <ti/net/slnetutils.h>

//#include <ti/ndk/inc/netmain.h>
//
//#include <ti/ndk/slnetif/slnetifndk.h>
//#include <ti/net/slnetif.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Types.h>

#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateMutexPri.h>

#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>

//#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <ti/sysbios/hal/Hwi.h>


//#include <ti/net/http/httpserver.h>
//#include <ti/net/http/http.h>
//#include <ti/net/http/logging.h>


//typedef char           INT8;
//typedef short          INT16;
//typedef int            INT32;
//typedef unsigned char  UINT8;
//typedef unsigned short UINT16;
//typedef unsigned int   UINT32;
//#ifndef _CSL_EMAC_TYPES
///* The same type is defined in "csl_mdio.h" as well. */
//typedef unsigned int   uint;
//#endif
//typedef void *         HANDLE;
//typedef UINT32         IPN;             /* IP Address in NETWORK format */

/* NDK BSD support */
//#include <sys/socket.h>
//#include <arpa/inet.h>

//#include <ti/ndk/inc/netmain.h>
//#include <usertype.h>
//#include <os/oskern.h>
//#include <os/osif.h>



#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>
#include <ti/display/DisplayExt.h>
#include <ti/display/AnsiColor.h>

/* TI-RTOS Header files */
//#include <ti/drivers/EMAC.h>
//#include <ti/drivers/emac/EMACMSP432E4.h>

#include <ti/drivers/CAN.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/CRC.h>
//#include <ti/drivers/gpio/GPIOMSP432E4.h>
#include <ti/drivers/ADCBuf.h>
 #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
 #include <ti/drivers/SPI.h>
 #include <ti/drivers/UART.h>
// #include <ti/drivers/USBMSCHFatFs.h>
#include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>
#include <ti/drivers/PWM.h>
//#include <ti/drivers/pwm/PWMTiva.h>


//#include <inc/hw_ints.h>
//#include "inc/hw_nvic.h"
//#include "inc/hw_types.h"
//#include "inc/hw_can.h"
//#include "inc/hw_ints.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_hibernate.h"
//#include <inc/hw_gpio.h>



//#include <utils/ustdlib.h>
//#include "utils/uartstdio.h"
//#include "utils/spi_flash.h"
//
//#include "driverlib/debug.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/eeprom.h"
//#include "driverlib/interrupt.h"
//#include <driverlib/flash.h>
//#include <driverlib/pwm.h>
//#include "driverlib/fpu.h"
//#include "driverlib/can.h"
//#include "driverlib/gpio.h"
//#include "driverlib/adc.h"
//#include "driverlib/pin_map.h"
//#define TARGET_IS_TM4C129_RA2
//#include "driverlib/rom.h"
//#include "driverlib/rom_map.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/debug.h"
//#include "driverlib/hibernate.h"
//#include "driverlib/watchdog.h"
//
//
//#include <usblib/usb-ids.h>
//#include <usblib/usblib.h>
//#include <usblib/usbcdc.h>
//#include <usblib/device/usbdevice.h>
//#include <usblib/device/usbdcdc.h>

/* DriverLib Includes */
#include "ti/devices/msp432e4/driverlib/driverlib.h"

#include <third_party/spiffs/spiffs.h>
#include <third_party/spiffs/SPIFFSNVS.h>

/* Board Header file */
#include "BoardRev10.h"

//#define __LOG_TTY
//#include "cmdline.h"
//#include "tty.h"
//#include "commands.h"
//#include "ttybin.h"
//#include "Controller.h"
//#include "AP.h"
//#include "BLE.h"
//#include "hibernation.h"
//#include "cmdline.h"
//#include "commands.h"
//#include "tty.h"
//#include "USBConsole.h"
//#include "EEPROMStorage.h"
//#include "CANTest.h"
#include "DISPLAY/SMCDisplay.h"


#include "DiscreteIO/DIO.h"
//#include "USB/USBCDCD.h"
//#include "USB/DFU.h"
//#include "USB/USBTTYbin.h"
//#include "USB/packettype.h"
//#include "NDK/UDPServer.h"
#include "NDK/UDPAVDSServer.h"
#include <Deprecated/BSPSerial.h>
#include <Deprecated/ALTO/ALTOInterface.h>
#include <Deprecated/ALTO/ForteManager/ComputerBridge.h>
#include <Deprecated/ALTO/ALTOMultinet/RelayControllerWorker.h>
#include <Deprecated/ALTO/ALTOMultinet/TempControllerWorker.h>
#include <Deprecated/ALTO/ALTOMultinet/ALTOMultinet.h>
#include <Deprecated/AVDS/AVDSClient.h>
#include "Deprecated/RosenDVD/Rosen.h"
#include "Deprecated/RosenDVD/RosenBlueRayDVD.h"
#include "Deprecated/Panasonic/PassengerControlUnit.h"
#include "Deprecated/CANTest/CANTest.h"
//#include "mx66l51235f.h"
//
//#include "TrashCollection/TrashCollection.h"
//
#include "Interfaces/Interfaces.h"
#include "Interfaces/IFUART.h"

#include "Interfaces/SMC_Interfaces.h"


#include "Devices/ServiceDefinition.h"
#include "Devices/Devices.h"
#include "Devices/Utils/cmdline.h"

#include "Devices/GenericDevice/GenericDevice.h"
#include "Devices/CANBus/CANTest.h"
#include "Devices/TestDevice/TestDevice.h"
#include "Devices/ALTOAmp/ALTOAmpDevice.h"
#include "Devices/ALTOMultinetDevice/RelayController/ALTOMultinetDeviceRelayControllerWorker.h"
#include "Devices/ForteManager/ForteManagerDevice.h"


#include "System/AnalogMonitor/SystemAnalogMonitor.h"
#include "System/InternalFlashStorage/InternalFlashStorage.h"
#include "System/ExternalFlashStorage/ExternalFlashStorage.h"
#include "System/EEPROM/EEPROMStorage.h"
#include "System/I2CTempSensor/I2CTemp101.h"
#include "System/SPIDAC/SPIDAC.h"
#include "System/MAX1301/MAX1301.h"
#include "System/Bootloader/BootloaderInterface.h"



#include "Devices/RemoteController/TCPRemoteController.h"
#include "Devices/RemoteController/TCPConsoleCommands/TCPConsoleCommands.h"

#include "Devices/RemoteController/TCPRemoteControllerBinary.h"
#include "Devices/RemoteController/TCPBinaryCommands/TCPBinaryCommands.h"
#include "Devices/RemoteController/TCPBinaryCommands/ALTOAmp/TCPBinaryCMD_ALTOAmp.h"
#include "Devices/RemoteController/TCPBinaryCommands/AVDS/TCPBinaryCMD_AVDS.h"
#include "Devices/RemoteController/TCPBinaryCommands/ALTOMultinet/TCPBinaryCMD_ALTOMultinet.h"
#include "Devices/RemoteController/TCPBinaryCommands/RAWCharacteristicData/TCPBinaryCMD_RAWCharacteristicData.h"
#include "Devices/RemoteController/TCPBinaryCommands/DiscreteIO/TCPBinaryCMD_DiscreteIO.h"
#include "Devices/RemoteController/TCPBinaryCommands/SystemControl/TCPBinaryCMD_SystemControl.h"
#include "Devices/RemoteController/TCPBinaryCommands/DAC/TCPBinaryCMD_DAC.h"
#include "Devices/RemoteController/TCPBinaryCommands/ProductionTest/TCPBinaryCMD_ProductionTest.h"
#include "Devices/RemoteController/TCPBinaryCommands/FlashFileSystem/TCPBinaryCMD_FlashFileSystem.h"
#include "Devices/RemoteController/TCPBinaryCommands/ROSEN/TCPBinaryCMD_Rosen.h"
#include "Devices/GenericDevice/TFTest/TFUARTTestDevice.h"
#include "Devices/GenericDevice/DUTTest/DUTUartTestDevice.h"

#include "Devices/AVDS/AVDSDevice.h"
#include "Devices/AVDS/AVDSRS485Device.h"
#include "Devices/Rosen/RosenDevice.h"
#include "Devices/Rosen/RosenRS485Device.h"

#include "HTTPServer/urlsimple.h"
#include "HTTPServer/URLHandler/URLHandler.h"


#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define MAX_SYSTEM_PRIORITY         10

/*
 *
 * http://www.ti.com/lit/ug/spnu151n/spnu151n.pdf
 * PAG 155
 *
 */
#define OS_ENTER_CRITICAL() _disable_IRQ()
#define OS_EXIT_CRITICAL()  _enable_IRQ()
//#define OS_ENTER_CRITICAL() IntMasterDisable()
//#define OS_EXIT_CRITICAL()  IntMasterEnable()

//#define os_enter_critical() __disable_irq() // PRIMARIMASK -> CPSID I
//                                                                                        // __disable_fiq(); // FAULTMASK -> CPSID F
//                                                                                        //          __set_BASEPRI(30);
//#define os_exit_critical()  __enable_irq()  // PRIMARIMASK -> CPSIE I
//                                                                                        // __enable_fiq();  // FAULTMASK -> CPSID F
//                                                                                        // __set_BASEPRI(0);

#ifdef __cplusplus
extern "C"  {
#endif

#ifdef	__INCLUDES_GLOBAL
	#define	__INCLUDES_EXT
#else
	#define __INCLUDES_EXT	extern
#endif


#ifdef __cplusplus
}
#endif





#ifdef __cplusplus


#endif



#endif /* INCLUDES_H_ */
