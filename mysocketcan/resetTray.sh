#!/bin/sh
# $Id: resetTray.sh 774 2012-08-16 14:19:49Z jschamba $

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <TCPU CANid (hex without 0x)> <devID> (canX)"
    exit 1
fi

if [ "x$2" = "x" ]; then
    echo $"Usage: $0 <TCPU CANid (hex without 0x)> <devID> (canX)"
    exit 1
fi

# load the TCPU FPGA from Eeprom 2
./cansend $2 "$1"2#8a6996a55a
usleep 500000


# load TDIG FPGAs from Eeprom 2
./cansend $2 1fc800"$1"#8a6996a55a
usleep 1000000

./cansend $2 "$1"2#0e0201

#usleep 500000

#./cansend $2 402#9901

exit

