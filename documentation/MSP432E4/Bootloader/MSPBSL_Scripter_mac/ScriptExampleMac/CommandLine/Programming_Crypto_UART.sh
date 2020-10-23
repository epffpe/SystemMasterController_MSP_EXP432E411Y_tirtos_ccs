# Assume the device is empty, so the -b will result in "password is correct"
-n Crypto -i [/dev/tty.usbmodem1421,UART,9600] -b -w blinkLED_Crypto.txt -z [RESET]