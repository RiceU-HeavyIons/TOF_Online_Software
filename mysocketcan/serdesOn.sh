#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <canX>"
    exit 1
fi

# load the TCPU FPGA from Eeprom 2
./cansend $1 402#911f
./cansend $1 402#921f
./cansend $1 402#931f
./cansend $1 402#941f
./cansend $1 402#951f
./cansend $1 402#961f
./cansend $1 402#971f
./cansend $1 402#9813

exit

