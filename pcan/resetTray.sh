#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <TCPU CANid (hex without 0x)> <devID> (dec)"
  exit 1
fi

if [ "x$2" = "x" ]; then
  echo $"Usage: $0 <TCPU CANid> <devID>"
  exit 1
fi

# load the TCPU FPGA from Eeprom 2
./pc "m s 0x"$1"2 5 0x8a 0x69 0x96 0xa5 0x5a $2"
usleep 70000

# load TDIG FPGAs from Eeprom 2
./pc "m e 0x1fc800"$1" 5 0x8a 0x69 0x96 0xa5 0x5a $2"
usleep 700000


# reset all of the TDCs on each TDIG
#k=0
#for j in 40 44 48 4c 50 54 58 5c; do
#    ./pc "m e 0x"$j"800"$1" 5 0xe 0x2 0x1 0x2 0x0 $2"
#    usleep 70000
    # set board position register
#    ./pc "m e 0x"$j"800"$1" 3 0xe 0xc 0x"$k" $2"
#    usleep 70000
#    k=`expr $k + 1`
#done


# put TCPUs into run mode and turn on Serdes, turn off CANbus data
usleep 70000
./pc "m s 0x"$1"2 3 0xe 0x2 0xf $2"
# set gate delay
#usleep 70000
#./pc "m s 0x"$1"2 3 0xe 0x8 0xe0 $2"


# issue bunch reset
usleep 70000
./pc "m s 0x402 2 0x99 0x1 251"

exit

