#!/bin/sh
# $Id$

# load the TCPU and TDIG FPGA from Eeprom 2
./pc "m s 0x202 5 0x8a 0x69 0x96 0xa5 0x5a 255"
usleep 500000
./pc "m e 0x1fc80020 5 0x8a 0x69 0x96 0xa5 0x5a 255"
usleep 1300000


# set threshold on each TDIG board to 2500mV
./xsetThreshold 255 0x7f 0x20 2500
usleep 1500000
# put TCPUs into run mode and turn on Serdes, turn off CANbus data
./pc "m s 0x202 3 0xe 0x2 0xf 255"

exit

