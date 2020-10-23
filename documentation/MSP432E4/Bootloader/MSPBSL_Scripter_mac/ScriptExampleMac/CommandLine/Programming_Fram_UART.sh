# Assume the device is empty, so the -b will result in "password is correct"
-n FRxx -i [/dev/tty.usbmodem1421,UART,9600] -b -w blinkLED_FR6989.txt -v -z [SET_PC,0x4400]