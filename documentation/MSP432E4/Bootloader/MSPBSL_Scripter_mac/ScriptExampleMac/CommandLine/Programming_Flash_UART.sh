#Assume the device is empty, so the -b will result in "password is correct"
-n 6xx -i [/dev/tty.usbmodem1421,UART,9600] -b -w blinkLED_f6459.txt -v -z [SET_PC,0x8000]